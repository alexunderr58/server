#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

Logger::Logger(const std::string& filename) : filename(filename) {}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

bool Logger::initialize() {
    std::lock_guard<std::mutex> lock(logMutex);
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть файл журнала: " << filename << std::endl;
        return false;
    }
    return true;
}

std::string Logger::getCurrentTime() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& params) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (!logFile.is_open()) {
        return;
    }
    
    std::string logEntry = getCurrentTime() + " [" + levelToString(level) + "] " + message;
    if (!params.empty()) {
        logEntry += " | Параметры: " + params;
    }
    
    logFile << logEntry << std::endl;
    logFile.flush();
    
    // Также выводим критические ошибки в stderr
    if (level == LogLevel::CRITICAL || level == LogLevel::ERROR) {
        std::cerr << logEntry << std::endl;
    }
}

void Logger::logError(bool isCritical, const std::string& message, const std::string& params) {
    log(isCritical ? LogLevel::CRITICAL : LogLevel::ERROR, message, params);
}
