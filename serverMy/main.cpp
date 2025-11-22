#include "server.h"
#include <csignal>
#include <atomic>
#include <iostream>

std::atomic<bool> shutdownFlag(false);
Server* serverInstance = nullptr;

void signalHandler(int signal) {
    (void)signal;  // Используем параметр чтобы убрать warning
    std::cout << "\nПолучен сигнал остановки. Завершаем работу..." << std::endl;
    shutdownFlag.store(true);
    if (serverInstance != nullptr) {
        serverInstance->stop();
    }
}

int main(int argc, char* argv[]) {
    // Установка обработчиков сигналов
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    Server server;
    serverInstance = &server;
    
    if (!server.initialize(argc, argv)) {
        return 1;
    }
    
    std::cout << "Сервер запущен. Для остановки нажмите Ctrl+C" << std::endl;
    
    // Запуск сервера
    server.start();
    
    std::cout << "Сервер завершил работу." << std::endl;
    return 0;
}
