#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    // Константы по умолчанию
    const std::string DEFAULT_CLIENT_DB = "/etc/vcalc.conf";
    const std::string DEFAULT_LOG_FILE = "/var/log/vcalc.log";
    const int DEFAULT_PORT = 33333;
    
    // Размеры данных
    const int SALT_BITS = 64;
    const int SALT_HEX_LENGTH = 16;
    const int MAX_LOGIN_LENGTH = 32;
    const int MAX_PASSWORD_LENGTH = 256;
    
    // Протокол
    const int BUFFER_SIZE = 4096;
    const std::string ERR_MSG = "ERR";
    const std::string OK_MSG = "OK";
}

#endif // CONFIG_H
