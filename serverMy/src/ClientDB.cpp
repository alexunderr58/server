#include "ClientDB.h"
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <openssl/sha.h>
#include <iostream>
#include <algorithm>
#include <cctype>

ClientDB::ClientDB(const std::string& filename) : filename(filename) {}

bool ClientDB::loadFromFile() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    clients.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        // Пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string login = line.substr(0, delimiterPos);
            std::string passwordHash = line.substr(delimiterPos + 1);
            
            // Удаляем пробелы
            login.erase(0, login.find_first_not_of(" \t"));
            login.erase(login.find_last_not_of(" \t") + 1);
            passwordHash.erase(0, passwordHash.find_first_not_of(" \t"));
            passwordHash.erase(passwordHash.find_last_not_of(" \t") + 1);
            
            if (!login.empty() && !passwordHash.empty()) {
                clients[login] = passwordHash;
            }
        }
    }
    
    file.close();
    return true;
}

bool ClientDB::saveToFile() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# База клиентов векторного калькулятора\n";
    file << "# Формат: логин:хэш_пароля\n\n";
    
    for (const auto& [login, hash] : clients) {
        file << login << ":" << hash << "\n";
    }
    
    file.close();
    return true;
}

bool ClientDB::clientExists(const std::string& login) const {
    std::lock_guard<std::mutex> lock(dbMutex);
    return clients.find(login) != clients.end();
}

bool ClientDB::verifyPassword(const std::string& login, const std::string& passwordHash) const {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    auto it = clients.find(login);
    if (it == clients.end()) {
        return false;
    }
    
    return it->second == passwordHash;
}

std::string ClientDB::generateHash(const std::string& salt, const std::string& password) {
    // SHA-1 согласно ТЗ
    std::string input = salt + password;
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    // Преобразование в hex строку
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        oss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    
    // Преобразование в верхний регистр как в ТЗ
    std::string result = oss.str();
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string ClientDB::generateSalt() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    
    uint64_t salt = dis(gen);
    
    // Преобразование в 16-ричную строку с дополнением нулями слева
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << salt;
    
    std::string result = oss.str();
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool ClientDB::addClient(const std::string& login, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    // Генерируем соль и хэш для нового клиента
    std::string salt = generateSalt();
    std::string hash = generateHash(salt, password);
    
    // Сохраняем только хэш (соль не сохраняем, генерируем новую при каждой аутентификации)
    clients[login] = hash;
    return saveToFile();
}

bool ClientDB::removeClient(const std::string& login) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    auto it = clients.find(login);
    if (it == clients.end()) {
        return false;
    }
    
    clients.erase(it);
    return saveToFile();
}
