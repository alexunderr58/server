#include <UnitTest++/UnitTest++.h>
#include <cmath>
#include <limits>
#include <cstring>
#include "../../include/VectorProcessor.h"

namespace {

TEST(VectorProcessor_Vector4Numbers) {
    std::vector<double> vec = {1.0, 2.0, 3.0, 4.0};
    double sum = VectorProcessor::calculateVectorSum(vec);
    CHECK_CLOSE(10.0, sum, 0.0001);
}

TEST(VectorProcessor_VectorLargeNumbers) {
    std::vector<double> vec = {80000.0, 100000.0, 120000.0, 120000.0};
    double sum = VectorProcessor::calculateVectorSum(vec);
    CHECK_CLOSE(420000.0, sum, 0.0001);
}

TEST(VectorProcessor_VectorNegativeNumbers) {
    std::vector<double> vec = {-4.4, -5.5, -6.6, -7.7};
    double sum = VectorProcessor::calculateVectorSum(vec);
    CHECK_CLOSE(-24.2, sum, 0.0001);
}

TEST(VectorProcessor_VectorVeryLargeNumbers) {
    std::vector<double> vec = {1e100, 2e100, 3e100, 4e100};
    double sum = VectorProcessor::calculateVectorSum(vec);
    if (std::isfinite(sum)) {
        CHECK_CLOSE(10e100, sum, 10e95);
    }
}

TEST(VectorProcessor_VectorVerySmallNumbers) {
    std::vector<double> vec = {-1e100, -2e100, -3e100, -4e100};
    double sum = VectorProcessor::calculateVectorSum(vec);
    if (std::isfinite(sum)) {
        CHECK_CLOSE(-10e100, sum, 10e95);
    }
}

TEST(VectorProcessor_VectorFractionalNumbers) {
    std::vector<double> vec = {1.1, 2.2, 3.3};
    double sum = VectorProcessor::calculateVectorSum(vec);
    CHECK_CLOSE(6.6, sum, 0.0000001);
}

TEST(VectorProcessor_EmptyVector) {
    std::vector<double> vec;
    double sum = VectorProcessor::calculateVectorSum(vec);
    CHECK_EQUAL(0.0, sum);
}

TEST(VectorProcessor_BoundaryValues) {
    std::vector<double> zeros(1000, 0.0);
    CHECK_EQUAL(0.0, VectorProcessor::calculateVectorSum(zeros));
    
    std::vector<double> mixed = {1.0, -1.0, 2.0, -2.0, 3.0, -3.0};
    CHECK_CLOSE(0.0, VectorProcessor::calculateVectorSum(mixed), 0.0001);
}

TEST(VectorProcessor_ProcessVectorsSimple) {
    std::vector<uint8_t> data;
    
    uint32_t numVectors = 1;
    data.insert(data.end(), 
               reinterpret_cast<uint8_t*>(&numVectors),
               reinterpret_cast<uint8_t*>(&numVectors) + sizeof(uint32_t));
    
    uint32_t vectorSize = 3;
    data.insert(data.end(),
               reinterpret_cast<uint8_t*>(&vectorSize),
               reinterpret_cast<uint8_t*>(&vectorSize) + sizeof(uint32_t));
    
    double values[] = {1.0, 2.0, 3.0};
    data.insert(data.end(),
               reinterpret_cast<uint8_t*>(values),
               reinterpret_cast<uint8_t*>(values) + 3 * sizeof(double));
    
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(data);
    
    CHECK_EQUAL(1, result.count);
    CHECK_EQUAL(1, result.sums.size());
    CHECK_CLOSE(6.0, result.sums[0], 0.0001);
}

TEST(VectorProcessor_ProcessVectorsEmpty) {
    std::vector<uint8_t> emptyData;
    VectorProcessor::VectorResult result = VectorProcessor::processVectors(emptyData);
    CHECK_EQUAL(0, result.count);
    CHECK(result.sums.empty());
}

} // unnamed namespace
