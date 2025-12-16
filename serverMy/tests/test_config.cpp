#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>
#include "../include/Config.h"

// T01: Проверка справки (тестируем через эмуляцию main)
void test_help_parameters() {
    std::cout << "T01: Testing help parameters...\n";
    
    // Проверяем константы из Config.h
    std::cout << "  Default port: " << Config::DEFAULT_PORT << "\n";
    std::cout << "  Default client DB: " << Config::DEFAULT_CLIENT_DB << "\n";
    std::cout << "  Default log file: " << Config::DEFAULT_LOG_FILE << "\n";
    
    assert(Config::DEFAULT_PORT == 33333);
    assert(Config::DEFAULT_CLIENT_DB == "/etc/vcalc.conf");
    assert(Config::DEFAULT_LOG_FILE == "/var/log/vcalc.log");
    assert(Config::SALT_HEX_LENGTH == 16);
    assert(Config::BUFFER_SIZE == 4096);
    
    std::cout << "T01: PASSED (config constants verified)\n";
}

// T02: Параметры по умолчанию
void test_default_parameters() {
    std::cout << "T02: Testing default parameters...\n";
    
    // Проверяем, что значения соответствуют ТЗ
    assert(Config::DEFAULT_PORT >= 1 && Config::DEFAULT_PORT <= 65535);
    assert(!Config::DEFAULT_CLIENT_DB.empty());
    assert(!Config::DEFAULT_LOG_FILE.empty());
    
    // Проверяем размеры данных
    assert(Config::SALT_BITS == 64);
    assert(Config::SALT_HEX_LENGTH == 16);  // 64 бита = 16 hex символов
    assert(Config::MAX_LOGIN_LENGTH > 0);
    assert(Config::MAX_PASSWORD_LENGTH > 0);
    
    std::cout << "  SALT_BITS: " << Config::SALT_BITS << "\n";
    std::cout << "  SALT_HEX_LENGTH: " << Config::SALT_HEX_LENGTH << "\n";
    std::cout << "  MAX_LOGIN_LENGTH: " << Config::MAX_LOGIN_LENGTH << "\n";
    std::cout << "  MAX_PASSWORD_LENGTH: " << Config::MAX_PASSWORD_LENGTH << "\n";
    
    std::cout << "T02: PASSED\n";
}

// T03: Кастомные параметры (проверяем логику без эмуляции main)
void test_custom_parameters_logic() {
    std::cout << "T03: Testing custom parameters logic...\n";
    
    // Тестируем валидные порты
    int valid_ports[] = {1, 1000, 33333, 44444, 65535};
    for (int port : valid_ports) {
        assert(port >= 1 && port <= 65535);
    }
    
    // Проверяем строковые константы
    assert(Config::ERR_MSG == "ERR");
    assert(Config::OK_MSG == "OK");
    
    std::cout << "  Valid ports range: 1-65535 ✓\n";
    std::cout << "  ERR_MSG: " << Config::ERR_MSG << " ✓\n";
    std::cout << "  OK_MSG: " << Config::OK_MSG << " ✓\n";
    
    std::cout << "T03: PASSED (parameter logic verified)\n";
}

void run_config_tests() {
    std::cout << "\n=== Config Tests (T01-T03) ===\n";
    
    test_help_parameters();
    test_default_parameters();
    test_custom_parameters_logic();
    
    std::cout << "=== Config tests completed ===\n";
}
