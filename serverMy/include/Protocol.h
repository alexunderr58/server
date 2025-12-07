#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

class Protocol {
public:
    // Аутентификация
    static bool sendSalt(int clientSocket, const std::string& salt);
    static bool sendError(int clientSocket);
    static bool sendOk(int clientSocket);
    
    static bool receiveLogin(int clientSocket, std::string& login);
    static bool receiveHash(int clientSocket, std::string& hash);
    
    // Работа с векторами (бинарный формат)
    static bool sendVectorResults(int clientSocket, 
                                  const std::vector<double>& results);
    static bool receiveVectorData(int clientSocket, 
                                  std::vector<uint8_t>& binaryData);
    
    // Вспомогательные функции
    static bool sendAll(int socket, const void* buffer, size_t length);
    static bool recvAll(int socket, void* buffer, size_t length);
    static std::string binaryToHex(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> hexToBinary(const std::string& hex);
    
private:
    static const int SEND_RECV_TIMEOUT = 10; // секунд
};

#endif // PROTOCOL_H
