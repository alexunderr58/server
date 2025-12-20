#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class TestUtils {
public:
    static std::vector<uint8_t> createSimpleVector(const std::vector<double>& values);
    static std::vector<uint8_t> createTestClientData(const std::vector<std::vector<double>>& vectors);
};

#endif // TEST_UTILS_H
