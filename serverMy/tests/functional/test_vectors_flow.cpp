#include <UnitTest++/UnitTest++.h>
#include <chrono>
#include <cstring>
#include <vector> 
#include "../../include/VectorProcessor.h"

namespace {

// Простые утилиты для тестов
std::vector<uint8_t> createSimpleVector(const std::vector<double>& values) {
    std::vector<uint8_t> data;
    
    uint32_t numVectors = 1;
    uint8_t* p = reinterpret_cast<uint8_t*>(&numVectors);
    data.insert(data.end(), p, p + sizeof(uint32_t));
    
    uint32_t vectorSize = static_cast<uint32_t>(values.size());
    p = reinterpret_cast<uint8_t*>(&vectorSize);
    data.insert(data.end(), p, p + sizeof(uint32_t));
    
    for (double val : values) {
        p = reinterpret_cast<uint8_t*>(&val);
        data.insert(data.end(), p, p + sizeof(double));
    }
    
    return data;
}

std::vector<uint8_t> createTestClientData(const std::vector<std::vector<double>>& vectors) {
    std::vector<uint8_t> data;
    
    uint32_t numVectors = static_cast<uint32_t>(vectors.size());
    uint8_t* p = reinterpret_cast<uint8_t*>(&numVectors);
    data.insert(data.end(), p, p + sizeof(uint32_t));
    
    for (const auto& vec : vectors) {
        uint32_t vecSize = static_cast<uint32_t>(vec.size());
        p = reinterpret_cast<uint8_t*>(&vecSize);
        data.insert(data.end(), p, p + sizeof(uint32_t));
        
        for (double val : vec) {
            p = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), p, p + sizeof(double));
        }
    }
    
    return data;
}

TEST(VectorsFlow_SingleVectorProcessing) {
    std::vector<uint8_t> data = createSimpleVector({1.0, 2.0, 3.0, 4.0});
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    
    CHECK_EQUAL(1, result.count);
    CHECK_EQUAL(1, result.sums.size());
    CHECK_CLOSE(10.0, result.sums[0], 0.0001);
}

TEST(VectorsFlow_MultipleVectorsProcessing) {
    std::vector<std::vector<double>> vectors = {
        {1.0, 2.0, 3.0},
        {10.0, 20.0, 30.0},
        {-1.0, -2.0, -3.0}
    };
    
    std::vector<uint8_t> data = createTestClientData(vectors);
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    
    CHECK_EQUAL(3, result.count);
    CHECK_EQUAL(3, result.sums.size());
    CHECK_CLOSE(6.0, result.sums[0], 0.0001);
    CHECK_CLOSE(60.0, result.sums[1], 0.0001);
    CHECK_CLOSE(-6.0, result.sums[2], 0.0001);
}

TEST(VectorsFlow_EmptyVectorHandling) {
    std::vector<uint8_t> data = createSimpleVector({});
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    
    CHECK_EQUAL(1, result.count);
    CHECK_EQUAL(1, result.sums.size());
    CHECK_EQUAL(0.0, result.sums[0]);
}

TEST(VectorsFlow_LargeVectorPerformance) {
    std::vector<double> largeVector(1000);
    double expected = 0.0;
    
    for (size_t i = 0; i < largeVector.size(); i++) {
        largeVector[i] = static_cast<double>(i) * 0.1;
        expected += largeVector[i];
    }
    
    std::vector<uint8_t> data = createSimpleVector(largeVector);
    
    auto start = std::chrono::high_resolution_clock::now();
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    CHECK_EQUAL(1, result.count);
    CHECK_CLOSE(expected, result.sums[0], 0.0001);
    CHECK(duration.count() < 100);
}

TEST(VectorsFlow_MixedDataTypes) {
    std::vector<std::vector<double>> vectors = {
        {1.5, 2.5, 3.5},
        {-1.1, -2.2, -3.3},
        {0.0, 0.0, 0.0},
        {1000.0, 2000.0}
    };
    
    std::vector<uint8_t> data = createTestClientData(vectors);
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    
    CHECK_EQUAL(4, result.count);
    CHECK_CLOSE(7.5, result.sums[0], 0.0001);
    CHECK_CLOSE(-6.6, result.sums[1], 0.0001);
    CHECK_EQUAL(0.0, result.sums[2]);
    CHECK_CLOSE(3000.0, result.sums[3], 0.0001);
}

} // unnamed namespace
