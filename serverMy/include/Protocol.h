/**
 * @file Protocol.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Класс для работы с сетевым протоколом сервера
 * 
 * @details Реализует бинарный протокол обмена данными между сервером и клиентами,
 * включая аутентификацию и передачу векторных данных.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

/**
 * @brief Класс для работы с сетевым протоколом
 * 
 * @details Содержит статические методы для отправки и приема данных
 * по сетевому протоколу векторного калькулятора.
 */
class Protocol {
public:
    // Аутентификация
    /**
     * @brief Отправка соли клиенту
     * @param[in] clientSocket Сокет клиента
     * @param[in] salt Соль для отправки
     * @return true если соль успешно отправлена, false в случае ошибки
     */
    static bool sendSalt(int clientSocket, const std::string& salt);
    
    /**
     * @brief Отправка сообщения об ошибке
     * @param[in] clientSocket Сокет клиента
     * @return true если сообщение успешно отправлено, false в случае ошибки
     */
    static bool sendError(int clientSocket);
    
    /**
     * @brief Отправка сообщения об успехе
     * @param[in] clientSocket Сокет клиента
     * @return true если сообщение успешно отправлено, false в случае ошибки
     */
    static bool sendOk(int clientSocket);
    
    /**
     * @brief Получение логина от клиента
     * @param[in] clientSocket Сокет клиента
     * @param[out] login Полученный логин
     * @return true если логин успешно получен, false в случае ошибки
     */
    static bool receiveLogin(int clientSocket, std::string& login);
    
    /**
     * @brief Получение хэша от клиента
     * @param[in] clientSocket Сокет клиента
     * @param[out] hash Полученный хэш
     * @return true если хэш успешно получен, false в случае ошибки
     */
    static bool receiveHash(int clientSocket, std::string& hash);
    
    // Работа с векторами (бинарный формат)
    /**
     * @brief Отправка результатов вычисления векторов
     * @param[in] clientSocket Сокет клиента
     * @param[in] results Результаты вычислений
     * @return true если результаты успешно отправлены, false в случае ошибки
     */
    static bool sendVectorResults(int clientSocket, 
                                  const std::vector<double>& results);
    
    /**
     * @brief Получение векторных данных от клиента
     * @param[in] clientSocket Сокет клиента
     * @param[out] binaryData Полученные бинарные данные
     * @return true если данные успешно получены, false в случае ошибки
     */
    static bool receiveVectorData(int clientSocket, 
                                  std::vector<uint8_t>& binaryData);
    
    // Вспомогательные функции
    /**
     * @brief Отправка всех данных через сокет
     * @param[in] socket Сокет для отправки
     * @param[in] buffer Буфер с данными
     * @param[in] length Длина данных
     * @return true если все данные отправлены, false в случае ошибки
     */
    static bool sendAll(int socket, const void* buffer, size_t length);
    
    /**
     * @brief Получение всех данных через сокет
     * @param[in] socket Сокет для приема
     * @param[out] buffer Буфер для данных
     * @param[in] length Ожидаемая длина данных
     * @return true если все данные получены, false в случае ошибки
     */
    static bool recvAll(int socket, void* buffer, size_t length);
    
    /**
     * @brief Преобразование бинарных данных в hex строку
     * @param[in] data Бинарные данные
     * @return Hex строка в верхнем регистре
     */
    static std::string binaryToHex(const std::vector<uint8_t>& data);
    
    /**
     * @brief Преобразование hex строки в бинарные данные
     * @param[in] hex Hex строка
     * @return Бинарные данные
     */
    static std::vector<uint8_t> hexToBinary(const std::string& hex);
    
private:
    static const int SEND_RECV_TIMEOUT = 10; ///< Таймаут отправки/приема в секундах
};

#endif // PROTOCOL_H
