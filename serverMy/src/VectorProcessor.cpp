#include "VectorProcessor.h"
#include <cstring>
#include <limits>
#include <cmath>
#include <iostream>

VectorProcessor::VectorResult VectorProcessor::processVectors(const std::vector<uint8_t>& binaryData) {
    VectorResult result;
    result.count = 0;
    
    size_t offset = 0;  // Используйте size_t здесь
    size_t dataSize = binaryData.size();
    
    // Читаем количество векторов (4 байта, uint32_t)
    uint32_t numVectors = 0;
    if (!readUInt32(binaryData.data(), offset, dataSize, numVectors)) {
        return result;
    }
    
    result.count = numVectors;
    
    // Обрабатываем каждый вектор
    for (uint32_t i = 0; i < numVectors; i++) {
        // Читаем размер вектора
        uint32_t vectorSize = 0;
        if (!readUInt32(binaryData.data(), offset, dataSize, vectorSize)) {
            return result;
        }
        
        // Читаем значения вектора
        std::vector<double> vector;
        vector.reserve(vectorSize);
        
        for (uint32_t j = 0; j < vectorSize; j++) {
            double value = 0.0;
            if (!readDouble(binaryData.data(), offset, dataSize, value)) {
                return result;
            }
            vector.push_back(value);
        }
        
        // Вычисляем сумму вектора
        double sum = calculateVectorSum(vector);
        result.sums.push_back(sum);
    }
    
    return result;
}

double VectorProcessor::calculateVectorSum(const std::vector<double>& vector) {
    double sum = 0.0;
    
    // Используем алгоритм Кэхэна для уменьшения ошибки округления
    double compensation = 0.0;
    
    for (double value : vector) {
        double y = value - compensation;
        double t = sum + y;
        compensation = (t - sum) - y;
        sum = t;
    }
    
    // Проверка на переполнение
    if (sum == std::numeric_limits<double>::infinity()) {
        return std::numeric_limits<double>::infinity();
    }
    else if (sum == -std::numeric_limits<double>::infinity()) {
        return -std::numeric_limits<double>::infinity();
    }
    
    return sum;
}

bool VectorProcessor::readUInt32(const uint8_t* data, size_t& offset, size_t maxSize, uint32_t& value) {
    if (offset + sizeof(uint32_t) > maxSize) {
        return false;
    }
    
    // Предполагаем little-endian (как в большинстве систем)
    std::memcpy(&value, data + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    return true;
}

bool VectorProcessor::readDouble(const uint8_t* data, size_t& offset, size_t maxSize, double& value) {
    if (offset + sizeof(double) > maxSize) {
        return false;
    }
    
    std::memcpy(&value, data + offset, sizeof(double));
    offset += sizeof(double);
    
    return true;
}
