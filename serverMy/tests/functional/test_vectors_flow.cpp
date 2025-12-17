#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <chrono>
#include "../test_utils.h"
#include "../../include/VectorProcessor.h"

// Симулятор клиента, отправляющего векторные данные
class VectorTestClient {
private:
    std::vector<std::vector<double>> vectors;
    
public:
    void addVector(const std::vector<double>& vec) {
        vectors.push_back(vec);
    }
    
    // Генерация бинарных данных для отправки
    std::vector<uint8_t> prepareBinaryData() const {
        return TestUtils::createTestClientData(vectors);
    }
    
    // Проверка результатов
    bool verifyResults(const VectorProcessor::VectorResult& result) const {
        if (result.count != vectors.size()) {
            std::cout << "  Error: Expected " << vectors.size() 
                      << " vectors, got " << result.count << "\n";
            return false;
        }
        
        for (size_t i = 0; i < vectors.size(); i++) {
            double expected = 0.0;
            for (double val : vectors[i]) {
                expected += val;
            }
            
            if (std::abs(result.sums[i] - expected) > 0.0001) {
                std::cout << "  Error: Vector " << i 
                          << ": expected " << expected 
                          << ", got " << result.sums[i] << "\n";
                return false;
            }
        }
        
        return true;
    }
};

void test_vector_processing_flow() {
    std::cout << "=== Vector Processing Flow Test ===\n";
    
    // 1. Тест 1: Один простой вектор
    std::cout << "Test 1: Single vector processing\n";
    {
        VectorTestClient client;
        client.addVector({1.0, 2.0, 3.0, 4.0});
        
        auto binaryData = client.prepareBinaryData();
        auto result = VectorProcessor::processVectors(binaryData);
        
        assert(client.verifyResults(result) == true);
        std::cout << "  ✓ Single vector processed correctly\n";
    }
    
    // 2. Тест 2: Несколько векторов
    std::cout << "Test 2: Multiple vectors processing\n";
    {
        VectorTestClient client;
        client.addVector({1.0, 2.0, 3.0});
        client.addVector({10.0, 20.0, 30.0});
        client.addVector({-1.0, -2.0, -3.0});
        
        auto binaryData = client.prepareBinaryData();
        auto result = VectorProcessor::processVectors(binaryData);
        
        assert(client.verifyResults(result) == true);
        std::cout << "  ✓ Multiple vectors processed correctly\n";
    }
    
    // 3. Тест 3: Пустой вектор
    std::cout << "Test 3: Empty vector handling\n";
    {
        VectorTestClient client;
        client.addVector({});
        
        auto binaryData = client.prepareBinaryData();
        auto result = VectorProcessor::processVectors(binaryData);
        
        assert(result.count == 1);
        assert(result.sums.size() == 1);
        assert(result.sums[0] == 0.0);
        std::cout << "  ✓ Empty vector handled correctly\n";
    }
    
    // 4. Тест 4: Большой вектор
    std::cout << "Test 4: Large vector performance\n";
    {
        VectorTestClient client;
        std::vector<double> largeVector(1000);  // Уменьшил до 1000 для скорости
        double expected = 0.0;
        
        for (size_t i = 0; i < largeVector.size(); i++) {
            largeVector[i] = static_cast<double>(i) * 0.1;
            expected += largeVector[i];
        }
        
        client.addVector(largeVector);
        
        auto start = std::chrono::high_resolution_clock::now();
        auto binaryData = client.prepareBinaryData();
        auto result = VectorProcessor::processVectors(binaryData);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        assert(client.verifyResults(result) == true);
        std::cout << "  ✓ Large vector (" << largeVector.size() 
                  << " elements) processed in " << duration.count() << "ms\n";
    }
    
    // 5. Тест 5: Смешанные типы данных
    std::cout << "Test 5: Mixed data types\n";
    {
        VectorTestClient client;
        client.addVector({1.5, 2.5, 3.5});          // Дробные положительные
        client.addVector({-1.1, -2.2, -3.3});       // Дробные отрицательные
        client.addVector({0.0, 0.0, 0.0});          // Нули
        client.addVector({1000.0, 2000.0});         // Большие числа
        
        auto binaryData = client.prepareBinaryData();
        auto result = VectorProcessor::processVectors(binaryData);
        
        assert(client.verifyResults(result) == true);
        std::cout << "  ✓ Mixed data types processed correctly\n";
    }
    
    std::cout << "=== Vector processing flow test completed ===\n";
}
