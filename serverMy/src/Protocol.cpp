#include "Protocol.h"
#include "Config.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <algorithm>  // Добавлено для std::transform
#include <cctype>     // Добавлено для ::toupper

bool Protocol::sendSalt(int clientSocket, const std::string& salt) {
    if (salt.length() != Config::SALT_HEX_LENGTH) {
        return false;
    }
    
    return sendAll(clientSocket, salt.c_str(), salt.length());
}

bool Protocol::sendError(int clientSocket) {
    return sendAll(clientSocket, Config::ERR_MSG.c_str(), Config::ERR_MSG.length());
}

bool Protocol::sendOk(int clientSocket) {
    return sendAll(clientSocket, Config::OK_MSG.c_str(), Config::OK_MSG.length());
}

bool Protocol::receiveLogin(int clientSocket, std::string& login) {
    char buffer[256] = {0};
    
    // Читаем логин с таймаутом
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (received <= 0) {
        return false;
    }
    
    // Преобразуем в строку и обрезаем символы новой строки
    login = std::string(buffer, received);
    
    // Удаляем \r, \n, пробелы
    size_t endPos = login.find_first_of("\r\n");
    if (endPos != std::string::npos) {
        login = login.substr(0, endPos);
    }
    
    // Удаляем пробелы по краям
    login.erase(0, login.find_first_not_of(" \t"));
    login.erase(login.find_last_not_of(" \t") + 1);
    
    return !login.empty();
}

bool Protocol::receiveHash(int clientSocket, std::string& hash) {
    char buffer[256] = {0};
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (received <= 0) {
        return false;
    }
    
    hash = std::string(buffer, received);
    
    // Удаляем \r, \n, пробелы
    size_t endPos = hash.find_first_of("\r\n");
    if (endPos != std::string::npos) {
        hash = hash.substr(0, endPos);
    }
    
    hash.erase(0, hash.find_first_not_of(" \t"));
    hash.erase(hash.find_last_not_of(" \t") + 1);
    
    return !hash.empty();
}

bool Protocol::sendVectorResults(int clientSocket, const std::vector<double>& results) {
    // Удаляем эту функцию - теперь отправляем результат после каждого вектора
    // Клиент ожидает результат ПОСЛЕ каждого вектора, а не все сразу
    return true;
}

bool Protocol::receiveVectorData(int clientSocket, std::vector<uint8_t>& binaryData) {
    // Убираем таймаут совсем
    struct timeval tv;
    tv.tv_sec = 0;  // Без таймаута
    tv.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // Читаем количество векторов
    uint32_t numVectors = 0;
    
    if (!recvAll(clientSocket, &numVectors, sizeof(uint32_t))) {
        std::cout << "DEBUG: Ошибка чтения количества векторов" << std::endl;
        return false;
    }
    
    std::cout << "DEBUG: Количество векторов: " << numVectors << std::endl;
    
    // Создаем буфер и добавляем количество векторов
    binaryData.clear();
    binaryData.resize(sizeof(uint32_t));
    memcpy(binaryData.data(), &numVectors, sizeof(uint32_t));
    
    // Читаем векторы по одному КАК В ТЗ
    for (uint32_t i = 0; i < numVectors; i++) {
        // Читаем размер вектора
        uint32_t vectorSize = 0;
        if (!recvAll(clientSocket, &vectorSize, sizeof(uint32_t))) {
            std::cout << "DEBUG: Ошибка чтения размера вектора " << i+1 << std::endl;
            return false;
        }
        
        std::cout << "DEBUG: Размер вектора " << i+1 << ": " << vectorSize << std::endl;
        
        // Добавляем размер вектора в буфер
        size_t currentSize = binaryData.size();
        binaryData.resize(currentSize + sizeof(uint32_t));
        memcpy(binaryData.data() + currentSize, &vectorSize, sizeof(uint32_t));
        
        // Читаем данные вектора
        size_t vectorBytes = vectorSize * sizeof(double);
        currentSize = binaryData.size();
        binaryData.resize(currentSize + vectorBytes);
        
        if (!recvAll(clientSocket, binaryData.data() + currentSize, vectorBytes)) {
            std::cout << "DEBUG: Ошибка чтения данных вектора " << i+1 
                      << ", нужно байт: " << vectorBytes << std::endl;
            return false;
        }
        
        // Сразу обрабатываем вектор и отправляем результат
        // Это важно - клиент ждет результат после каждого вектора
        std::vector<double> currentVector(vectorSize);
        memcpy(currentVector.data(), binaryData.data() + currentSize, vectorBytes);
        
        double sum = 0;
        for (double val : currentVector) {
            sum += val;
        }
        
        std::cout << "DEBUG: Сумма вектора " << i+1 << ": " << sum << std::endl;
        
        // Отправляем результат сразу
        if (!sendAll(clientSocket, &sum, sizeof(double))) {
            std::cout << "DEBUG: Ошибка отправки результата вектора " << i+1 << std::endl;
            return false;
        }
    }
    
    std::cout << "DEBUG: Всего получено байт: " << binaryData.size() << std::endl;
    return true;
}


bool Protocol::sendAll(int socket, const void* buffer, size_t length) {
    const char* ptr = static_cast<const char*>(buffer);
    size_t bytesSent = 0;
    
    while (bytesSent < length) {
        ssize_t sent = send(socket, ptr + bytesSent, length - bytesSent, 0);
        
        if (sent <= 0) {
            return false;
        }
        
        bytesSent += sent;
    }
    
    return true;
}

bool Protocol::recvAll(int socket, void* buffer, size_t length) {
    char* ptr = static_cast<char*>(buffer);
    size_t bytesReceived = 0;
    
    // Убираем таймаут для этой функции
    struct timeval tv;
    tv.tv_sec = 30;  // Увеличиваем таймаут до 30 секунд
    tv.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    while (bytesReceived < length) {
        ssize_t received = recv(socket, ptr + bytesReceived, length - bytesReceived, 0);
        
        if (received <= 0) {
            std::cout << "DEBUG: recvAll ошибка, получено: " << received 
                      << ", ожидалось: " << length << " байт, получено всего: " << bytesReceived << std::endl;
            return false;
        }
        
        bytesReceived += received;
    }
    
    return true;
}

