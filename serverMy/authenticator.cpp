#include "authenticator.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>  // Добавляем для std::transform

std::string Authenticator::generateSalt() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    
    uint64_t salt = dis(gen);
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << salt;
    return ss.str();
}

std::string Authenticator::computeHash(const std::string& salt, const std::string& password) {
    std::string data = salt + password;
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    // Преобразуем в верхний регистр для совместимости с клиентом
    std::string result = ss.str();
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    
    return result;
}
