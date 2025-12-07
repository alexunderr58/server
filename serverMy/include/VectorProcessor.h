#ifndef VECTORPROCESSOR_H
#define VECTORPROCESSOR_H

#include <vector>
#include <cstdint>
#include <cstddef>  // Добавьте эту строку для size_t

class VectorProcessor {
public:
    struct VectorResult {
        uint32_t count;
        std::vector<double> sums;
    };
    
    static VectorResult processVectors(const std::vector<uint8_t>& binaryData);
    static double calculateVectorSum(const std::vector<double>& vector);
    
private:
    static bool readUInt32(const uint8_t* data, size_t& offset, size_t maxSize, uint32_t& value);
    static bool readDouble(const uint8_t* data, size_t& offset, size_t maxSize, double& value);
};

#endif // VECTORPROCESSOR_H
