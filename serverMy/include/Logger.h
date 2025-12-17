/**
 * @file Logger.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Класс для логирования событий сервера
 * 
 * @details Обеспечивает запись логов в файл с различными уровнями важности.
 * Потокобезопасный. Автоматически закрывает файл при уничтожении.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

/**
 * @brief Уровни логирования
 */
enum class LogLevel {
    INFO,      ///< Информационные сообщения
    WARNING,   ///< Предупреждения
    ERROR,     ///< Ошибки
    CRITICAL   ///< Критические ошибки
};

/**
 * @brief Класс для ведения журнала событий
 * 
 * @details Реализует потокобезопасное логирование в файл
 * с поддержкой различных уровней важности сообщений.
 */
class Logger {
private:
    std::ofstream logFile; ///< Файл для записи логов
    mutable std::mutex logMutex;  ///< Мьютекс для потокобезопасности
    std::string filename; ///< Имя файла журнала
    
    /**
     * @brief Получение текущего времени в формате строки
     * @return Строка с текущим временем в формате "ГГГГ-ММ-ДД ЧЧ:ММ:СС"
     */
    std::string getCurrentTime() const;
    
    /**
     * @brief Преобразование уровня логирования в строку
     * @param[in] level Уровень логирования
     * @return Строковое представление уровня
     */
    std::string levelToString(LogLevel level) const;
    
public:
    /**
     * @brief Конструктор класса Logger
     * @param[in] filename Имя файла для записи логов
     */
    Logger(const std::string& filename);
    
    /**
     * @brief Деструктор класса Logger
     * @details Автоматически закрывает файл логов
     */
    ~Logger();
    
    /**
     * @brief Инициализация логгера
     * @return true если файл успешно открыт, false в случае ошибки
     */
    bool initialize();
    
    /**
     * @brief Запись сообщения в лог
     * @param[in] level Уровень важности сообщения
     * @param[in] message Текст сообщения
     * @param[in] params Дополнительные параметры (опционально)
     */
    void log(LogLevel level, const std::string& message, const std::string& params = "");
    
    /**
     * @brief Запись ошибки в лог
     * @param[in] isCritical Признак критичности ошибки
     * @param[in] message Текст сообщения об ошибке
     * @param[in] params Дополнительные параметры (опционально)
     */
    void logError(bool isCritical, const std::string& message, const std::string& params = "");
    
    // Запрет копирования
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#endif // LOGGER_H
