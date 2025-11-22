#include "vectorprocessor.h"
#include <cmath>

double VectorProcessor::calculateSum(const double* vector, uint32_t size) {
    double sum = 0.0;
    for (uint32_t i = 0; i < size; i++) {
        sum += vector[i];
    }
    return sum;
}

