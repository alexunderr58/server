#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <regex>
#include "test_utils.h"
#include "../include/Logger.h"

// T27: Логирование успеха
void test_success_logging() {
    std::cout << "T27: Testing success logging...\n";
    
    std::string filename = TestUtils::createTempFile("");
    Logger logger(filename);
    
    assert(logger.initialize() == true);
    logger.log(LogLevel::INFO, "Test info message", "param1=value1");
    
    // Проверяем, что файл создан и не пустой
    std::ifstream file(filename);
    assert(file.good());
    
    std::string line;
    std::getline(file, line);
    std::cout << "  Log entry: " << line << "\n";
    
    assert(!line.empty());
    
    TestUtils::deleteTempFile(filename);
    std::cout << "T27: PASSED\n";
}

// T28: Логирование ошибок
void test_error_logging() {
    std::cout << "T28: Testing error logging...\n";
    
    std::string filename = TestUtils::createTempFile("");
    Logger logger(filename);
    logger.initialize();
    
    // Логируем ошибки разных уровней
    logger.log(LogLevel::ERROR, "Test error message", "err_code=500");
    logger.log(LogLevel::CRITICAL, "Test critical message", "system=failed");
    logger.logError(false, "Non-critical error", "user=test");
    logger.logError(true, "Critical error", "system=down");
    
    // Проверяем файл
    std::ifstream file(filename);
    std::string line;
    int lineCount = 0;
    
    while (std::getline(file, line)) {
        lineCount++;
        std::cout << "  Line " << lineCount << ": " << line << "\n";
    }
    
    assert(lineCount >= 4);  // Должно быть минимум 4 записи
    
    TestUtils::deleteTempFile(filename);
    std::cout << "T28: PASSED\n";
}

// T29: Формат логов
void test_log_format() {
    std::cout << "T29: Testing log format...\n";
    
    std::string filename = TestUtils::createTempFile("");
    Logger logger(filename);
    logger.initialize();
    
    logger.log(LogLevel::INFO, "Format test", "test=value");
    
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    
    std::cout << "  Log format: " << line << "\n";
    
    // Проверяем формат: [ВРЕМЯ] [УРОВЕНЬ] СООБЩЕНИЕ | Параметры: ...
    // Регулярное выражение для проверки формата
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} \[(INFO|WARNING|ERROR|CRITICAL)\] .+)");
    
    assert(std::regex_match(line, pattern));
    
    // Проверяем наличие параметров
    assert(line.find("| Параметры:") != std::string::npos);
    
    TestUtils::deleteTempFile(filename);
    std::cout << "T29: PASSED\n";
}

void run_logger_tests() {
    std::cout << "\n=== Logger Tests (T27-T29) ===\n";
    
    test_success_logging();
    test_error_logging();
    test_log_format();
    
    std::cout << "=== Logger tests completed ===\n";
}
