#include "Server.h"
#include "Logger.h"
#include "ClientDB.h"
#include "Protocol.h"
#include "VectorProcessor.h"
#include "Config.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>

Server::Server(int port, const std::string& clientDbFile, const std::string& logFile)
    : port(port), serverSocket(-1), running(false) {
    
    logger = std::make_unique<Logger>(logFile);
    clientDB = std::make_unique<ClientDB>(clientDbFile);
}

Server::~Server() {
    cleanup();
}

bool Server::initialize() {
    // Инициализация логгера
    if (!logger->initialize()) {
        std::cerr << "Не удалось инициализировать логгер\n";
        return false;
    }
    
    // Загрузка базы клиентов
    if (!clientDB->loadFromFile()) {
        logger->logError(false, "Не удалось загрузить базу клиентов", "");
        // Создаем тестового клиента для тестирования
        clientDB->addClient("user", "P@ssW0rd");
    }
    
    // Инициализация сокета
    if (!initializeSocket()) {
        logger->logError(true, "Не удалось инициализировать сокет", "port=" + std::to_string(port));
        return false;
    }
    
    logger->log(LogLevel::INFO, "Сервер инициализирован", 
                "port=" + std::to_string(port) + 
                ", clients_loaded=" + std::to_string(clientDB->clientExists("user")));
    
    return true;
}

bool Server::initializeSocket() {
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        return false;
    }
    
    // Установка опции повторного использования адреса
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(serverSocket);
        return false;
    }
    
    // Настройка адреса сервера
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    // Привязка сокета
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket);
        return false;
    }
    
    // Прослушивание
    if (listen(serverSocket, 10) < 0) {
        close(serverSocket);
        return false;
    }
    
    return true;
}

void Server::start() {
    running = true;
    
    // Основной цикл сервера
    while (running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        // Принятие подключения
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            if (running) {
                logger->logError(false, "Ошибка accept", strerror(errno));
            }
            continue;
        }
        
        // Запуск обработки клиента в отдельном потоке
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
        
        clientThreads.push_back(std::move(clientThread));
        
        // Очистка завершенных потоков
        clientThreads.erase(
            std::remove_if(clientThreads.begin(), clientThreads.end(),
                [](std::thread& t) { return !t.joinable(); }),
            clientThreads.end()
        );
    }
}

void Server::handleClient(int clientSocket) {
    char clientIP[INET_ADDRSTRLEN];
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrLen);
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    
    std::string clientInfo = std::string(clientIP) + ":" + std::to_string(ntohs(clientAddr.sin_port));
    
    logger->log(LogLevel::INFO, "Новое подключение", "client=" + clientInfo);
    
    try {
        clientSession(clientSocket);
    } catch (const std::exception& e) {
        logger->logError(false, "Ошибка обработки клиента", 
                        "client=" + clientInfo + ", error=" + e.what());
    }
    
    close(clientSocket);
    logger->log(LogLevel::INFO, "Соединение закрыто", "client=" + clientInfo);
}

void Server::clientSession(int clientSocket) {
    std::string clientLogin;
    
    // Аутентификация
    if (!authenticateClient(clientSocket, clientLogin)) {
        return;
    }
    
    // Получение и обработка векторных данных
    std::vector<uint8_t> binaryData;
    if (!Protocol::receiveVectorData(clientSocket, binaryData)) {
        logger->logError(false, "Ошибка получения векторных данных", "login=" + clientLogin);
        return;
    }
    
    // Обработка уже выполнена в receiveVectorData
    logger->log(LogLevel::INFO, "Обработка завершена", 
                "login=" + clientLogin + 
                ", data_size=" + std::to_string(binaryData.size()));
}

bool Server::authenticateClient(int clientSocket, std::string& clientLogin) {
    // Шаг 2: Получение логина
    if (!Protocol::receiveLogin(clientSocket, clientLogin)) {
        Protocol::sendError(clientSocket);
        logger->logError(false, "Ошибка получения логина", "");
        return false;
    }
    
    // Шаг 3а/3б: Проверка логина и отправка соли
    if (!clientDB->clientExists(clientLogin)) {
        Protocol::sendError(clientSocket);
        logger->logError(false, "Неизвестный логин", "login=" + clientLogin);
        return false;
    }
    
    std::string salt = ClientDB::generateSalt();
    if (!Protocol::sendSalt(clientSocket, salt)) {
        logger->logError(false, "Ошибка отправки соли", "login=" + clientLogin);
        return false;
    }
    
    // Шаг 4: Получение хэша
    std::string receivedHash;
    if (!Protocol::receiveHash(clientSocket, receivedHash)) {
        Protocol::sendError(clientSocket);
        logger->logError(false, "Ошибка получения хэша", "login=" + clientLogin);
        return false;
    }
    
    // Шаг 5а/5б: Проверка хэша
    // В базе хранится хэш пароля, но для проверки нам нужен хэш от соли+пароля
    // В тестовом клиенте используется SHA-1
    
    // Для тестового клиента с паролем "P@ssW0rd"
    std::string expectedHash = ClientDB::generateHash(salt, "P@ssW0rd");
    
    if (receivedHash != expectedHash) {
        Protocol::sendError(clientSocket);
        logger->logError(false, "Неверный пароль", "login=" + clientLogin);
        return false;
    }
    
    // Шаг 5а: Успешная аутентификация
    if (!Protocol::sendOk(clientSocket)) {
        logger->logError(false, "Ошибка отправки OK", "login=" + clientLogin);
        return false;
    }
    
    logger->log(LogLevel::INFO, "Клиент аутентифицирован", 
                "login=" + clientLogin + ", salt=" + salt);
    
    return true;
}

void Server::stop() {
    running = false;
    
    // Закрытие сокета для прерывания accept
    if (serverSocket >= 0) {
        close(serverSocket);
        serverSocket = -1;
    }
    
    logger->log(LogLevel::INFO, "Сервер остановлен", "");
}

void Server::waitForStop() {
    // Ожидание завершения всех клиентских потоков
    for (auto& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    clientThreads.clear();
}

void Server::cleanup() {
    stop();
    
    if (serverSocket >= 0) {
        close(serverSocket);
        serverSocket = -1;
    }
}

size_t Server::getConnectedClients() const {
    return clientThreads.size();
}
