#include "clientdb.h"
#include <fstream>

bool ClientDB::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            users[login] = password;
        }
    }
    file.close();
    return true;
}

bool ClientDB::userExists(const std::string& login) {
    return users.find(login) != users.end();
}

std::string ClientDB::getPassword(const std::string& login) {
    auto it = users.find(login);
    return (it != users.end()) ? it->second : "";
}
