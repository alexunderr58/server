#ifndef CLIENTDB_H
#define CLIENTDB_H

#include <string>
#include <unordered_map>

class ClientDB {
private:
    std::unordered_map<std::string, std::string> users;
    
public:
    bool load(const std::string& filename);
    bool userExists(const std::string& login);
    std::string getPassword(const std::string& login);
};

#endif
