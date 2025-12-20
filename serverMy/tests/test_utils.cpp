#include "test_utils.h"
#include <cstring>

std::vector<uint8_t> TestUtils::createSimpleVector(const std::vector<double>& values) {
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

std::vector<uint8_t> TestUtils::createTestClientData(const std::vector<std::vector<double>>& vectors) {
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
