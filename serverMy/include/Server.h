/**
 * @file Server.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Основной класс сервера векторного калькулятора
 * 
 * @details Реализует многопоточный TCP сервер с аутентификацией клиентов
 * и обработкой векторных вычислений.
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

// Предварительные объявления
class Logger;
class ClientDB;

/**
 * @brief Основной класс сервера векторного калькулятора
 * 
 * @details Управляет сетевыми подключениями, аутентификацией клиентов,
 * обработкой векторных данных и многопоточностью.
 */
class Server {
private:
    int port; ///< Порт сервера
    int serverSocket; ///< Основной сокет сервера
    std::atomic<bool> running; ///< Флаг работы сервера (atomic для потокобезопасности)
    std::unique_ptr<Logger> logger; ///< Логгер сервера
    std::unique_ptr<ClientDB> clientDB; ///< База данных клиентов
    
    std::vector<std::thread> clientThreads; ///< Потоки обработки клиентов
    
    /**
     * @brief Инициализация сетевого сокета
     * @return true если сокет успешно инициализирован, false в случае ошибки
     */
    bool initializeSocket();
    
    /**
     * @brief Очистка ресурсов сервера
     */
    void cleanup();
    
    /**
     * @brief Обработка подключения клиента
     * @param[in] clientSocket Сокет клиента
     */
    void handleClient(int clientSocket);
    
    /**
     * @brief Сессия работы с клиентом
     * @param[in] clientSocket Сокет клиента
     */
    void clientSession(int clientSocket);
    
    /**
     * @brief Аутентификация клиента
     * @param[in] clientSocket Сокет клиента
     * @param[out] clientLogin Логин аутентифицированного клиента
     * @return true если аутентификация успешна, false в противном случае
     */
    bool authenticateClient(int clientSocket, std::string& clientLogin);
    
public:
    /**
     * @brief Конструктор класса Server
     * @param[in] port Порт для прослушивания
     * @param[in] clientDbFile Файл базы данных клиентов
     * @param[in] logFile Файл журнала
     */
    Server(int port, const std::string& clientDbFile, const std::string& logFile);
    
    /**
     * @brief Деструктор класса Server
     * @details Автоматически останавливает сервер и очищает ресурсы
     */
    ~Server();
    
    /**
     * @brief Инициализация сервера
     * @return true если инициализация успешна, false в случае ошибки
     */
    bool initialize();
    
    /**
     * @brief Запуск сервера
     * @details Начинает прослушивание порта и обработку подключений
     */
    void start();
    
    /**
     * @brief Остановка сервера
     * @details Graceful shutdown сервера
     */
    void stop();
    
    /**
     * @brief Ожидание остановки сервера
     * @details Ожидает завершения всех клиентских потоков
     */
    void waitForStop();
    
    /**
     * @brief Получение количества подключенных клиентов
     * @return Количество активных клиентских потоков
     */
    size_t getConnectedClients() const;
};

#endif // SERVER_H
