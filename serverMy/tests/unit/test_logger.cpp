#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <regex>
#include <cstdlib>
#include "../../include/Logger.h"

namespace {

struct LoggerFixture {
    std::string tempFile;
    Logger* logger;
    
    LoggerFixture() {
        tempFile = "/tmp/test_logger_" + std::to_string(rand()) + ".log";
        logger = new Logger(tempFile);
        logger->initialize();
    }
    
    ~LoggerFixture() {
        delete logger;
        std::remove(tempFile.c_str());
    }
};

TEST_FIXTURE(LoggerFixture, Logger_LogInfoMessage) {
    logger->log(LogLevel::INFO, "Test info message", "param1=value1");
    
    std::ifstream file(tempFile);
    std::string line;
    std::getline(file, line);
    
    CHECK(!line.empty());
    CHECK(line.find("[INFO]") != std::string::npos);
    CHECK(line.find("Test info message") != std::string::npos);
}

TEST_FIXTURE(LoggerFixture, Logger_LogErrorMessage) {
    logger->log(LogLevel::ERROR, "Test error message", "err_code=500");
    
    std::ifstream file(tempFile);
    std::string line;
    std::getline(file, line);
    
    CHECK(line.find("[ERROR]") != std::string::npos);
}

TEST_FIXTURE(LoggerFixture, Logger_LogFormat) {
    logger->log(LogLevel::INFO, "Format test", "test=value");
    
    std::ifstream file(tempFile);
    std::string line;
    std::getline(file, line);
    
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} \[(INFO|WARNING|ERROR|CRITICAL)\] .+)");
    CHECK(std::regex_match(line, pattern));
}

TEST_FIXTURE(LoggerFixture, Logger_LogLevels) {
    logger->log(LogLevel::WARNING, "Warning message");
    
    std::ifstream file(tempFile);
    std::string line;
    std::getline(file, line);
    
    CHECK(line.find("[WARNING]") != std::string::npos);
}

} // unnamed namespace
