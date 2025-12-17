/**
 * @file VectorProcessor.h
 * @author Alexander Melekhin
 * @version 1.0
 * @date 2024
 * @brief Класс для обработки векторных данных
 * 
 * @details Обеспечивает вычисление суммы элементов векторов
 * с поддержкой больших чисел и переполнения.
 */

#ifndef VECTORPROCESSOR_H
#define VECTORPROCESSOR_H

#include <vector>
#include <cstdint>
#include <cstddef>  // Добавлено для size_t

/**
 * @brief Класс для обработки векторных данных
 * 
 * @details Реализует парсинг бинарных данных векторов и вычисление
 * суммы их элементов с использованием алгоритма Кэхэна для уменьшения
 * ошибок округления.
 */
class VectorProcessor {
public:
    /**
     * @brief Структура результата обработки векторов
     */
    struct VectorResult {
        uint32_t count; ///< Количество обработанных векторов
        std::vector<double> sums; ///< Суммы элементов каждого вектора
    };
    
    /**
     * @brief Обработка бинарных векторных данных
     * @param[in] binaryData Бинарные данные векторов
     * @return Структура с результатами обработки
     */
    static VectorResult processVectors(const std::vector<uint8_t>& binaryData);
    
    /**
     * @brief Вычисление суммы элементов вектора
     * @param[in] vector Вектор для вычисления суммы
     * @return Сумма элементов вектора
     * @note Использует алгоритм Кэхэна для уменьшения ошибок округления
     */
    static double calculateVectorSum(const std::vector<double>& vector);
    
private:
    /**
     * @brief Чтение 32-битного целого числа из бинарных данных
     * @param[in] data Указатель на бинарные данные
     * @param[in,out] offset Текущая позиция в данных (смещается при чтении)
     * @param[in] maxSize Максимальный размер данных
     * @param[out] value Прочитанное значение
     * @return true если чтение успешно, false в случае ошибки
     */
    static bool readUInt32(const uint8_t* data, size_t& offset, size_t maxSize, uint32_t& value);
    
    /**
     * @brief Чтение числа double из бинарных данных
     * @param[in] data Указатель на бинарные данные
     * @param[in,out] offset Текущая позиция в данных (смещается при чтении)
     * @param[in] maxSize Максимальный размер данных
     * @param[out] value Прочитанное значение
     * @return true если чтение успешно, false в случае ошибки
     */
    static bool readDouble(const uint8_t* data, size_t& offset, size_t maxSize, double& value);
};

#endif // VECTORPROCESSOR_H
