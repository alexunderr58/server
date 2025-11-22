#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
private:
    std::string logFile;
    
public:
    void setLogFile(const std::string& filename);
    void log(const std::string& message, const std::string& severity = "INFO");
};

#endif
