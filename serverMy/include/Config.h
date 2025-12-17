/**
 * @file Config.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Заголовочный файл с конфигурационными константами сервера
 * 
 * @details Содержит константы по умолчанию для порта, файлов, 
 * размеров данных и сообщений протокола
 * @warning Изменение констант может повлиять на совместимость с клиентами
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    // Константы по умолчанию
    const std::string DEFAULT_CLIENT_DB = "/etc/vcalc.conf"; ///< Файл базы клиентов по умолчанию
    const std::string DEFAULT_LOG_FILE = "/var/log/vcalc.log"; ///< Файл журнала по умолчанию
    const int DEFAULT_PORT = 33333; ///< Порт сервера по умолчанию
    
    // Размеры данных
    const int SALT_BITS = 64; ///< Размер соли в битах
    const int SALT_HEX_LENGTH = 16; ///< Длина соли в hex символах
    const int MAX_LOGIN_LENGTH = 32; ///< Максимальная длина логина
    const int MAX_PASSWORD_LENGTH = 256; ///< Максимальная длина пароля
    
    // Протокол
    const int BUFFER_SIZE = 4096; ///< Размер буфера для сетевых операций
    const std::string ERR_MSG = "ERR"; ///< Сообщение об ошибке
    const std::string OK_MSG = "OK"; ///< Сообщение об успехе
}

#endif // CONFIG_H
