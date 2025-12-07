#include <iostream>
#include <csignal>
#include <memory>
#include "Server.h"
#include "Config.h"

std::unique_ptr<Server> server;

void signalHandler(int signal) {
    (void)signal;  // Явно указываем, что параметр не используется
    if (server) {
        std::cout << "\nПолучен сигнал завершения. Остановка сервера..." << std::endl;
        server->stop();
    }
}

void printHelp() {
    std::cout << "Векторный калькулятор сервер v1.0\n";
    std::cout << "Использование: vcalc_server [ПАРАМЕТРЫ]\n\n";
    std::cout << "Параметры:\n";
    std::cout << "  -h, --help            Показать эту справку\n";
    std::cout << "  -c, --config FILE     Файл базы клиентов (по умолчанию: " 
              << Config::DEFAULT_CLIENT_DB << ")\n";
    std::cout << "  -l, --log FILE        Файл журнала (по умолчанию: " 
              << Config::DEFAULT_LOG_FILE << ")\n";
    std::cout << "  -p, --port PORT       Порт сервера (по умолчанию: " 
              << Config::DEFAULT_PORT << ")\n";
    std::cout << "\nПримеры:\n";
    std::cout << "  vcalc_server\n";
    std::cout << "  vcalc_server -c ./clients.conf -l ./vcalc.log -p 44444\n";
    std::cout << "\nТестовый клиент использует:\n";
    std::cout << "  Логин: user, Пароль: P@ssW0rd\n";
    std::cout << "  Адрес: 127.0.0.1, Порт: 33333\n";
}

int main(int argc, char* argv[]) {
    // Параметры по умолчанию
    std::string clientDbFile = Config::DEFAULT_CLIENT_DB;
    std::string logFile = Config::DEFAULT_LOG_FILE;
    int port = Config::DEFAULT_PORT;
    
    // Обработка аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }
        else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
            clientDbFile = argv[++i];
        }
        else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            logFile = argv[++i];
        }
        else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
            try {
                port = std::stoi(argv[++i]);
                if (port < 1 || port > 65535) {
                    std::cerr << "Ошибка: порт должен быть в диапазоне 1-65535\n";
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "Ошибка: некорректный номер порта\n";
                return 1;
            }
        }
        else {
            std::cerr << "Неизвестный параметр: " << arg << "\n";
            printHelp();
            return 1;
        }
    }
    
    // Если запущен без параметров, показываем справку
    if (argc == 1) {
        printHelp();
        return 0;
    }
    
    // Регистрация обработчиков сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        std::cout << "Запуск сервера...\n";
        std::cout << "Файл базы клиентов: " << clientDbFile << "\n";
        std::cout << "Файл журнала: " << logFile << "\n";
        std::cout << "Порт: " << port << "\n";
        
        // Создание и запуск сервера
        server = std::make_unique<Server>(port, clientDbFile, logFile);
        
        if (!server->initialize()) {
            std::cerr << "Ошибка инициализации сервера\n";
            return 1;
        }
        
        std::cout << "Сервер запущен. Для остановки нажмите Ctrl+C\n";
        server->start();
        server->waitForStop();
        
        std::cout << "Сервер остановлен.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
