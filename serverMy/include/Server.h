#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

class Logger;
class ClientDB;

class Server {
private:
    int port;
    int serverSocket;
    std::atomic<bool> running;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<ClientDB> clientDB;
    
    std::vector<std::thread> clientThreads;
    
    bool initializeSocket();
    void cleanup();
    void handleClient(int clientSocket);
    void clientSession(int clientSocket);
    
    // Аутентификация клиента
    bool authenticateClient(int clientSocket, std::string& clientLogin);
    
public:
    Server(int port, const std::string& clientDbFile, const std::string& logFile);
    ~Server();
    
    bool initialize();
    void start();
    void stop();
    void waitForStop();
    
    // Статистика
    size_t getConnectedClients() const;
};

#endif // SERVER_H
