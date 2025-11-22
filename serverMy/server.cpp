#include "server.h"
#include "clientdb.h"
#include "authenticator.h"
#include "vectorprocessor.h"
#include "logger.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <poll.h>
#include <cstring>
#include <algorithm>

// Глобальные объекты
ClientDB clientDB;
Logger logger;

Server::Server() : port(33333), clientDBFile("/etc/vcalc.conf"), 
                   logFile("/var/log/vcalc.log"), serverRunning(false), serverSocket(-1) {}

bool Server::initialize(int argc, char* argv[]) {
    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h") {
            showHelp();
            return false;
        } else if (arg == "-d" && i + 1 < argc) {
            clientDBFile = argv[++i];
        } else if (arg == "-l" && i + 1 < argc) {
            logFile = argv[++i];
        } else if (arg == "-p" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        }
    }

    logger.setLogFile(logFile);

    // Загрузка базы клиентов
    if (!clientDB.load(clientDBFile)) {
        std::cerr << "Ошибка загрузки базы клиентов: " << clientDBFile << std::endl;
        logger.log("Ошибка загрузки базы клиентов: " + clientDBFile, "ERROR");
        return false;
    }

    logger.log("База клиентов загружена: " + clientDBFile);
    return true;
}

bool Server::authenticateClient(int clientSocket) {
    char buffer[256];
    
    // Прием LOGIN
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        logger.log("Ошибка приема LOGIN", "ERROR");
        return false;
    }
    buffer[bytesRead] = '\0';
    std::string login(buffer);
    logger.log("Получен LOGIN: " + login);
    
    // Проверка LOGIN в БД
    if (!clientDB.userExists(login)) {
        send(clientSocket, "ERR", 3, 0);
        logger.log("Неизвестный LOGIN: " + login, "WARNING");
        return false;
    }
    logger.log("LOGIN найден в БД");
    
    // Генерация и отправка SALT
    std::string salt = Authenticator::generateSalt();
    logger.log("Сгенерирован SALT: " + salt);
    
    // Отправляем SALT клиенту
    size_t saltLength = salt.length();
    ssize_t sent = send(clientSocket, salt.c_str(), saltLength, 0);
    if (sent != static_cast<ssize_t>(saltLength)) {
        logger.log("Ошибка отправки SALT", "ERROR");
        return false;
    }
    logger.log("SALT отправлен клиенту");
    
    // Прием HASH
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        logger.log("Ошибка приема HASH", "ERROR");
        return false;
    }
    buffer[bytesRead] = '\0';
    std::string clientHash(buffer);
    logger.log("Получен HASH от клиента: " + clientHash);
    
    // Проверка HASH
    std::string password = clientDB.getPassword(login);
    logger.log("Пароль из БД для " + login + ": " + password);
    
    std::string serverHash = Authenticator::computeHash(salt, password);
    logger.log("Вычисленный HASH на сервере: " + serverHash);
    
    // Сравниваем хэши без учета регистра
    std::string clientHashUpper = clientHash;
    std::string serverHashUpper = serverHash;
    std::transform(clientHashUpper.begin(), clientHashUpper.end(), clientHashUpper.begin(), ::toupper);
    std::transform(serverHashUpper.begin(), serverHashUpper.end(), serverHashUpper.begin(), ::toupper);  // Исправлено: begin() вместо end()
    
    logger.log("Сравнение хэшей (верхний регистр): клиентский='" + clientHashUpper + "' серверный='" + serverHashUpper + "'");
    
    if (clientHashUpper != serverHashUpper) {
        send(clientSocket, "ERR", 3, 0);
        logger.log("Ошибка аутентификации: клиентский хэш не совпадает с серверным", "WARNING");
        return false;
    }
    
    sent = send(clientSocket, "OK", 2, 0);
    if (sent != 2) {
        logger.log("Ошибка отправки OK", "ERROR");
        return false;
    }
    
    logger.log("Успешная аутентификация: " + login);
    return true;
}

bool Server::processVectors(int clientSocket) {
    uint32_t numVectors;
    
    // Прием количества векторов
    if (recv(clientSocket, &numVectors, sizeof(numVectors), 0) != sizeof(numVectors)) {
        logger.log("Ошибка приема количества векторов", "ERROR");
        return false;
    }
    
    logger.log("Количество векторов: " + std::to_string(numVectors));
    
    if (numVectors == 0) {
        logger.log("Получен пустой набор векторов", "INFO");
        uint32_t numResults = 0;
        send(clientSocket, &numResults, sizeof(numResults), 0);
        return true;
    }
    
    // Подготовка ответа - количество результатов
    uint32_t numResults = numVectors;
    ssize_t sent = send(clientSocket, &numResults, sizeof(numResults), 0);
    if (sent != sizeof(numResults)) {
        logger.log("Ошибка отправки количества результатов, отправлено " + std::to_string(sent) + " из " + std::to_string(sizeof(numResults)) + " байт", "ERROR");
        return false;
    }
    logger.log("Количество результатов отправлено: " + std::to_string(sent) + " байт");
    
    // Обработка каждого вектора
    for (uint32_t i = 0; i < numVectors; i++) {
        uint32_t vectorSize;
        
        // Прием размера вектора
        if (recv(clientSocket, &vectorSize, sizeof(vectorSize), 0) != sizeof(vectorSize)) {
            logger.log("Ошибка приема размера вектора " + std::to_string(i+1), "ERROR");
            return false;
        }
        
        logger.log("Размер вектора " + std::to_string(i+1) + ": " + std::to_string(vectorSize));
        
        if (vectorSize == 0) {
            double result = 0.0;
            sent = send(clientSocket, &result, sizeof(result), 0);
            if (sent != sizeof(result)) {
                logger.log("Ошибка отправки результата для нулевого вектора, отправлено " + std::to_string(sent) + " из " + std::to_string(sizeof(result)) + " байт", "ERROR");
                return false;
            }
            logger.log("Обработан нулевой вектор " + std::to_string(i+1) + ", отправлено " + std::to_string(sent) + " байт");
            continue;
        }
        
        // Прием данных вектора
        std::vector<double> vectorData(vectorSize);
        ssize_t totalBytes = vectorSize * sizeof(double);
        ssize_t bytesRead = 0;
        
        logger.log("Ожидаем " + std::to_string(totalBytes) + " байт данных для вектора " + std::to_string(i+1));
        
        while (bytesRead < totalBytes) {
            ssize_t result = recv(clientSocket, 
                                reinterpret_cast<char*>(vectorData.data()) + bytesRead, 
                                totalBytes - bytesRead, 0);
            if (result <= 0) {
                logger.log("Ошибка приема данных вектора " + std::to_string(i+1) + 
                          ", получено " + std::to_string(bytesRead) + " из " + std::to_string(totalBytes) + " байт", "ERROR");
                return false;
            }
            bytesRead += result;
        }
        
        logger.log("Успешно принят вектор " + std::to_string(i+1) + ", байт: " + std::to_string(bytesRead));
        
        // Вычисление суммы
        double result = VectorProcessor::calculateSum(vectorData.data(), vectorSize);
        logger.log("Вычислена сумма для вектора " + std::to_string(i+1) + ": " + std::to_string(result));
        
        // Отправляем результат как double (8 байт)
        logger.log("Размер double: " + std::to_string(sizeof(result)) + " байт");
        
        sent = send(clientSocket, &result, sizeof(result), 0);
        if (sent != sizeof(result)) {
            logger.log("Ошибка отправки результата для вектора " + std::to_string(i+1) + 
                      ", отправлено " + std::to_string(sent) + " из " + std::to_string(sizeof(result)) + " байт", "ERROR");
            return false;
        }
        
        logger.log("Результат для вектора " + std::to_string(i+1) + " отправлен, байт: " + std::to_string(sent));
        
        // Ждем немного перед следующим вектором
        usleep(10000); // 10ms
    }
    
    logger.log("Обработка всех векторов завершена успешно");
    return true;
}

void Server::start() {
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        logger.log("Ошибка создания сокета", "ERROR");
        return;
    }
    
    // Разрешаем повторное использование адреса
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Настройка адреса
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        logger.log("Ошибка привязки сокета к порту " + std::to_string(port), "ERROR");
        close(serverSocket);
        return;
    }
    
    // Прослушивание
    if (listen(serverSocket, 5) < 0) {
        logger.log("Ошибка прослушивания порта", "ERROR");
        close(serverSocket);
        return;
    }
    
    logger.log("Сервер запущен на порту " + std::to_string(port));
    serverRunning = true;
    
    // Настройка poll для неблокирующего accept
    struct pollfd fds[1];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;
    
    while (serverRunning) {
        // Ожидание подключения с таймаутом 1 секунда
        int pollResult = poll(fds, 1, 1000);
        
        if (pollResult < 0) {
            if (serverRunning) {
                logger.log("Ошибка в poll", "ERROR");
            }
            break;
        }
        
        if (pollResult == 0) {
            // Таймаут - проверяем флаг остановки
            continue;
        }
        
        if (fds[0].revents & POLLIN) {
            // Есть новое подключение
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
            if (clientSocket < 0) {
                if (serverRunning) {
                    logger.log("Ошибка принятия соединения", "ERROR");
                }
                continue;
            }
            
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            logger.log("Новое подключение от " + std::string(clientIP));
            
            // Аутентификация клиента
            if (authenticateClient(clientSocket)) {
                // Обработка векторов
                processVectors(clientSocket);
            }
            
            close(clientSocket);
            logger.log("Соединение с " + std::string(clientIP) + " закрыто");
        }
    }
    
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    
    logger.log("Сервер остановлен");
}

void Server::stop() {
    serverRunning = false;
    if (serverSocket != -1) {
        // Принудительно закрываем сокет чтобы выйти из accept
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::showHelp() {
    std::cout << "Использование: server [OPTIONS]" << std::endl;
    std::cout << "Опции:" << std::endl;
    std::cout << "  -h          Показать эту справку" << std::endl;
    std::cout << "  -d FILE     Файл базы клиентов (по умолчанию: /etc/vcalc.conf)" << std::endl;
    std::cout << "  -l FILE     Файл журнала (по умолчанию: /var/log/vcalc.log)" << std::endl;
    std::cout << "  -p PORT     Порт сервера (по умолчанию: 33333)" << std::endl;
    std::cout << "Управление: Ctrl+C для остановки сервера" << std::endl;
}
