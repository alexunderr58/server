#ifndef SERVER_H
#define SERVER_H

#include <string>

class Server {
private:
    int port;
    std::string clientDBFile;
    std::string logFile;
    bool serverRunning;
    int serverSocket;

    bool authenticateClient(int clientSocket);
    bool processVectors(int clientSocket);
    void logMessage(const std::string& message, const std::string& severity = "INFO");

public:
    Server();
    bool initialize(int argc, char* argv[]);
    void start();
    void stop();
    void showHelp();
    
    // Добавляем метод для проверки состояния
    bool isRunning() const { return serverRunning; }
};

#endif
