#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <memory>
#include <sys/stat.h>
#include <cmath>  // Добавлен для std::fabs

// Утилиты для тестирования
class TestUtils {
public:
    // Проверка hex строки
    static bool isValidHex(const std::string& str, bool uppercase = true) {
        if (str.empty()) return false;
        
        for (char c : str) {
            if (uppercase) {
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
                    return false;
                }
            } else {
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
                    return false;
                }
            }
        }
        return true;
    }
    
    // Проверка, что строка в верхнем регистре
    static bool isUppercase(const std::string& str) {
        for (char c : str) {
            if (std::islower(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        return true;
    }
    
    // Создание временного файла
    static std::string createTempFile(const std::string& content) {
        std::string filename = "/tmp/test_" + std::to_string(rand()) + ".conf";
        std::ofstream file(filename);
        if (file) {
            file << content;
            file.close();
        }
        return filename;
    }
    
    // Создание пустого временного файла
    static std::string createTempFile() {
        return createTempFile("");
    }
    
    // Удаление временного файла
    static bool deleteTempFile(const std::string& filename) {
        return std::remove(filename.c_str()) == 0;
    }
    
    // Проверка существования файла
    static bool fileExists(const std::string& filename) {
        struct stat buffer;
        return (stat(filename.c_str(), &buffer) == 0);
    }
    
    // Чтение содержимого файла
    static std::string readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return "";
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }
    
    // Генерация тестовых векторных данных
    static std::vector<uint8_t> createTestVectorData(size_t numVectors, size_t vectorSize) {
        std::vector<uint8_t> data;
        
        // Добавляем количество векторов (little-endian)
        uint32_t nv = static_cast<uint32_t>(numVectors);
        for (size_t i = 0; i < sizeof(uint32_t); i++) {
            data.push_back(static_cast<uint8_t>(nv & 0xFF));
            nv >>= 8;
        }
        
        for (size_t vecIdx = 0; vecIdx < numVectors; vecIdx++) {
            // Размер вектора (little-endian)
            uint32_t vs = static_cast<uint32_t>(vectorSize);
            for (size_t i = 0; i < sizeof(uint32_t); i++) {
                data.push_back(static_cast<uint8_t>(vs & 0xFF));
                vs >>= 8;
            }
            
            // Данные вектора (double в бинарном формате)
            for (size_t elemIdx = 0; elemIdx < vectorSize; elemIdx++) {
                double val = static_cast<double>(vecIdx * 10 + elemIdx);
                uint8_t* bytes = reinterpret_cast<uint8_t*>(&val);
                for (size_t i = 0; i < sizeof(double); i++) {
                    data.push_back(bytes[i]);
                }
            }
        }
        
        return data;
    }
    
    // Создание простого тестового вектора
    static std::vector<uint8_t> createSimpleVector(const std::vector<double>& values) {
        std::vector<uint8_t> data;
        
        // Количество векторов = 1
        uint32_t numVectors = 1;
        uint8_t* p = reinterpret_cast<uint8_t*>(&numVectors);
        data.insert(data.end(), p, p + sizeof(uint32_t));
        
        // Размер вектора
        uint32_t vectorSize = static_cast<uint32_t>(values.size());
        p = reinterpret_cast<uint8_t*>(&vectorSize);
        data.insert(data.end(), p, p + sizeof(uint32_t));
        
        // Данные вектора
        for (double val : values) {
            p = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), p, p + sizeof(double));
        }
        
        return data;
    }
    
    // Создание тестового клиента
    static std::vector<uint8_t> createTestClientData(const std::vector<std::vector<double>>& vectors) {
        std::vector<uint8_t> data;
        
        // Количество векторов
        uint32_t numVectors = static_cast<uint32_t>(vectors.size());
        uint8_t* p = reinterpret_cast<uint8_t*>(&numVectors);
        data.insert(data.end(), p, p + sizeof(uint32_t));
        
        // Каждый вектор
        for (const auto& vec : vectors) {
            // Размер вектора
            uint32_t vecSize = static_cast<uint32_t>(vec.size());
            p = reinterpret_cast<uint8_t*>(&vecSize);
            data.insert(data.end(), p, p + sizeof(uint32_t));
            
            // Данные вектора
            for (double val : vec) {
                p = reinterpret_cast<uint8_t*>(&val);
                data.insert(data.end(), p, p + sizeof(double));
            }
        }
        
        return data;
    }
    
    // Генерация случайной строки
    static std::string randomString(size_t length, const std::string& charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") {
        std::string result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; i++) {
            result += charset[rand() % charset.length()];
        }
        
        return result;
    }
    
    // Генерация случайного hex
    static std::string randomHex(size_t length, bool uppercase = true) {
        std::string charset = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
        return randomString(length, charset);
    }
    
    // Ожидание с проверкой условия
    template<typename Func>
    static bool waitForCondition(Func condition, int timeoutMs = 5000, int intervalMs = 100) {
        auto start = std::chrono::steady_clock::now();
        
        while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(timeoutMs)) {
            if (condition()) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }
        
        return false;
    }
};

// Mock объект для тестирования Server без реальных сокетов
class MockClient {
private:
    std::vector<uint8_t> sendBuffer;
    std::vector<uint8_t> receiveBuffer;
    size_t readPos = 0;
    
public:
    void queueReceive(const void* data, size_t size) {
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        receiveBuffer.insert(receiveBuffer.end(), ptr, ptr + size);
    }
    
    void queueReceiveString(const std::string& str) {
        queueReceive(str.c_str(), str.length());
    }
    
    bool mockSend(const void* data, size_t size) {
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        sendBuffer.insert(sendBuffer.end(), ptr, ptr + size);
        return true;
    }
    
    bool mockReceive(void* data, size_t size) {
        if (readPos + size > receiveBuffer.size()) {
            return false;
        }
        
        uint8_t* ptr = static_cast<uint8_t*>(data);
        std::memcpy(ptr, receiveBuffer.data() + readPos, size);
        readPos += size;
        return true;
    }
    
    std::vector<uint8_t> getSentData() const {
        return sendBuffer;
    }
    
    std::string getSentString() const {
        return std::string(sendBuffer.begin(), sendBuffer.end());
    }
    
    void clear() {
        sendBuffer.clear();
        receiveBuffer.clear();
        readPos = 0;
    }
    
    size_t available() const {
        return receiveBuffer.size() - readPos;
    }
};

// Генератор тестовых сценариев
class TestScenario {
public:
    struct AuthScenario {
        std::string login;
        std::string password;
        bool shouldSucceed;
        std::string description;
    };
    
    struct VectorScenario {
        std::vector<std::vector<double>> vectors;
        std::vector<double> expectedSums;
        std::string description;
    };
    
    // Сценарии аутентификации
    static AuthScenario createValidAuth() {
        return {"user", "P@ssW0rd", true, "Valid authentication"};
    }
    
    static AuthScenario createInvalidLogin() {
        return {"wronguser", "P@ssW0rd", false, "Invalid login"};
    }
    
    static AuthScenario createInvalidPassword() {
        return {"user", "wrongpass", false, "Invalid password"};
    }
    
    // Сценарии векторов
    static VectorScenario createSimpleVectors() {
        return {
            {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}},
            {6.0, 15.0},
            "Two simple vectors"
        };
    }
    
    static VectorScenario createEmptyVectors() {
        return {
            {{}, {1.0}, {2.0, 3.0}},
            {0.0, 1.0, 5.0},
            "Vectors including empty one"
        };
    }
    
    static VectorScenario createLargeVectors() {
        std::vector<std::vector<double>> vectors;
        std::vector<double> expectedSums;
        
        // 5 векторов по 1000 элементов
        for (int i = 0; i < 5; i++) {
            std::vector<double> vec(1000, static_cast<double>(i + 1));
            vectors.push_back(vec);
            expectedSums.push_back(static_cast<double>((i + 1) * 1000));
        }
        
        return {vectors, expectedSums, "5 vectors of 1000 elements each"};
    }
    
    // Создание тестового файла клиентов
    static std::string createTestClientFile() {
        std::string filename = "/tmp/test_clients_" + std::to_string(rand()) + ".conf";
        std::ofstream file(filename);
        
        file << "# Test client database\n";
        file << "# Format: login:hash\n\n";
        file << "user:FE78BDB4183B677A55E711B17E9CB6CCC2AE7315\n";  // P@ssW0rd
        file << "admin:5BA5E8871A3D6B55F6D2C9B5F6A8C7D9E0F1A2B3\n";
        file << "test:TESTHASH1234567890ABCDEF1234567890ABCDEF\n";
        
        file.close();
        return filename;
    }
    
    // Создание тестового лог файла
    static std::string createTestLogFile() {
        std::string filename = "/tmp/test_log_" + std::to_string(rand()) + ".log";
        std::ofstream file(filename);
        file.close();
        return filename;
    }
    
    // Создание поврежденного файла
    static std::string createCorruptedFile() {
        std::string filename = "/tmp/corrupted_" + std::to_string(rand()) + ".conf";
        std::ofstream file(filename);
        
        file << "This is not a valid format\n";
        file << "user_without_colon\n";
        file << ":password_without_login\n";
        file << "too:many:colons:here\n";
        file << "valid:but_with_weird_chars!@#$%\n";
        
        file.close();
        return filename;
    }
};

// Утилиты для сравнения double
class DoubleComparer {
public:
    static bool equals(double a, double b, double epsilon = 1e-9) {
        return fabs(a - b) < epsilon;
    }
    
    static bool equals(const std::vector<double>& a, const std::vector<double>& b, double epsilon = 1e-9) {
        if (a.size() != b.size()) return false;
        
        for (size_t i = 0; i < a.size(); i++) {
            if (!equals(a[i], b[i], epsilon)) {
                return false;
            }
        }
        
        return true;
    }
};

// Счетчик времени для тестов производительности
class Timer {
private:
    std::chrono::steady_clock::time_point startTime;
    
public:
    Timer() {
        start();
    }
    
    void start() {
        startTime = std::chrono::steady_clock::now();
    }
    
    long long elapsedMs() const {
        auto endTime = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    }
    
    long long elapsedMicros() const {
        auto endTime = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    }
    
    void printElapsed(const std::string& testName) {
        auto ms = elapsedMs();
        std::cout << "  " << testName << " took " << ms << " ms";
        if (ms > 1000) {
            std::cout << " (" << (ms / 1000.0) << " seconds)";
        }
        std::cout << "\n";
    }
};

#endif // TEST_UTILS_H
