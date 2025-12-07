#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
private:
    std::ofstream logFile;
    mutable std::mutex logMutex;  // mutable для использования в const методах
    std::string filename;
    
    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;
    
public:
    Logger(const std::string& filename);
    ~Logger();
    
    bool initialize();
    void log(LogLevel level, const std::string& message, const std::string& params = "");
    void logError(bool isCritical, const std::string& message, const std::string& params = "");
    
    // Запрет копирования
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#endif // LOGGER_H
