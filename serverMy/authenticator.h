#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <string>

class Authenticator {
public:
    static std::string generateSalt();
    static std::string computeHash(const std::string& salt, const std::string& password);
};

#endif
