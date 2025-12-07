#ifndef CLIENTDB_H
#define CLIENTDB_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>

class ClientDB {
private:
    std::unordered_map<std::string, std::string> clients; // login -> password hash
    mutable std::mutex dbMutex;  // mutable для const методов
    std::string filename;
    
public:
    ClientDB(const std::string& filename);
    
    bool loadFromFile();
    bool saveToFile();
    
    bool clientExists(const std::string& login) const;
    bool verifyPassword(const std::string& login, const std::string& passwordHash) const;
    
    // Генерация хэша для проверки
    static std::string generateHash(const std::string& salt, const std::string& password);
    
    // Генерация случайной соли
    static std::string generateSalt();
    
    // Добавление/удаление клиентов (для администрирования)
    bool addClient(const std::string& login, const std::string& password);
    bool removeClient(const std::string& login);
};

#endif // CLIENTDB_H
