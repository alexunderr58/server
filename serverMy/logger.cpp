#include "logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>

void Logger::setLogFile(const std::string& filename) {
    logFile = filename;
}

void Logger::log(const std::string& message, const std::string& severity) {
    std::ofstream file(logFile, std::ios::app);
    if (file.is_open()) {
        std::time_t now = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&now);
        
        file << "[" << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "] " 
             << "[" << severity << "] " << message << std::endl;
        file.close();
    }
}
