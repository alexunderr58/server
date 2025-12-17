/**
 * @file ClientDB.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Класс для работы с базой данных клиентов
 * 
 * @details Обеспечивает хранение, загрузку, сохранение и проверку 
 * учетных данных клиентов. Использует SHA-1 для хэширования паролей.
 * @warning Потокобезопасный доступ к базе клиентов
 */

#ifndef CLIENTDB_H
#define CLIENTDB_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>

/**
 * @brief Класс для управления базой данных клиентов
 * 
 * @details Реализует хранение пар логин-хэш_пароля в памяти 
 * с возможностью сохранения в файл и загрузки из него.
 * Потокобезопасен за счет использования мьютекса.
 */
class ClientDB {
private:
    std::unordered_map<std::string, std::string> clients; ///< Хранилище клиентов: логин -> хэш пароля
    mutable std::mutex dbMutex;  ///< Мьютекс для потокобезопасности (mutable для const методов)
    std::string filename; ///< Имя файла базы данных
    
public:
    /**
     * @brief Конструктор класса ClientDB
     * @param[in] filename Имя файла для хранения базы клиентов
     */
    ClientDB(const std::string& filename);
    
    /**
     * @brief Загрузка клиентов из файла
     * @return true если файл успешно загружен, false в случае ошибки
     */
    bool loadFromFile();
    
    /**
     * @brief Сохранение клиентов в файл
     * @return true если файл успешно сохранен, false в случае ошибки
     */
    bool saveToFile();
    
    /**
     * @brief Проверка существования клиента
     * @param[in] login Логин клиента для проверки
     * @return true если клиент существует, false в противном случае
     */
    bool clientExists(const std::string& login) const;
    
    /**
     * @brief Проверка хэша пароля клиента
     * @param[in] login Логин клиента
     * @param[in] passwordHash Хэш пароля для проверки
     * @return true если хэш совпадает, false в противном случае
     */
    bool verifyPassword(const std::string& login, const std::string& passwordHash) const;
    
    /**
     * @brief Генерация хэша SHA-1 для проверки пароля
     * @param[in] salt Соль для хэширования
     * @param[in] password Пароль для хэширования
     * @return Хэш SHA-1 в виде hex строки в верхнем регистре
     */
    static std::string generateHash(const std::string& salt, const std::string& password);
    
    /**
     * @brief Генерация случайной соли
     * @return Случайная соль длиной 16 hex символов в верхнем регистре
     */
    static std::string generateSalt();
    
    /**
     * @brief Добавление нового клиента
     * @param[in] login Логин нового клиента
     * @param[in] password Пароль нового клиента
     * @return true если клиент успешно добавлен, false в случае ошибки
     */
    bool addClient(const std::string& login, const std::string& password);
    
    /**
     * @brief Удаление клиента
     * @param[in] login Логин клиента для удаления
     * @return true если клиент успешно удален, false если клиент не найден
     */
    bool removeClient(const std::string& login);
};

#endif // CLIENTDB_H
