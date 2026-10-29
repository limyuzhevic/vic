// SIMDUtils - SIMD detection and utility functions
// 
// Provides runtime detection of SIMD capabilities and utility functions
// for optimizing neural computations with vector instructions.
// 
// Features:
// - Runtime SIMD capability detection
// - Utility functions for vector operations
// - Fallback implementations for non-SIMD systems
// - Performance-optimized math functions
// - Memory-aligned allocation for SIMD
// 
// Biological motivation:
// - Neural computations benefit from vector processing
// - SIMD optimization mimics biological parallel processing
// - Memory access patterns optimized for cache efficiency

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace nlm {
namespace SIMDUtils {

/**
 * SIMD capability detection
 */
enum class SIMDLevel {
    None = 0,
    SSE = 1,
    SSE2 = 2,
    AVX = 3,
    AVX2 = 4,
    AVX512 = 5
};

/**
 * Detect the highest available SIMD level
 * @return SIMD level detected at runtime
 */
SIMDLevel detectSIMDLevel();

/**
 * Check if specific SIMD level is available
 * @param level SIMD level to check
 * @return true if level is available
 */
bool hasSIMD(SIMDLevel level);

/**
 * Align memory to cache line for SIMD
 * @param ptr Pointer to align
 * @param alignment Alignment in bytes (typically 64)
 * @return Aligned pointer
 */
void* alignForSIMD(void* ptr, size_t alignment = 64);

/**
 * Check if pointer is properly aligned for SIMD
 * @param ptr Pointer to check
 * @param alignment Alignment in bytes
 * @return true if aligned
 */
bool isAlignedForSIMD(const void* ptr, size_t alignment = 64);

/**
 * Vectorized exponential function
 * @param values Input vector
 * @param result Output vector (same size as input)
 * @param size Number of elements to process
 */
template<typename T>
void exponential(const T* values, T* result, size_t size);

/**
 * Vectorized sigmoid function
 * @param values Input vector
 * @param result Output vector (same size as input)
 * @param size Number of elements to process
 */
template<typename T>
void sigmoid(const T* values, T* result, size_t size);

/**
 * Vectorized ReLU function
 * @param values Input vector
 * @param result Output vector (same size as input)
 * @param size Number of elements to process
 */
template<typename T>
void relu(const T* values, T* result, size_t size);

/**
 * Vectorized addition
 * @param a First vector
 * @param b Second vector
 * @param result Output vector (same size as input)
 * @param size Number of elements to process
 */
template<typename T>
void add(const T* a, const T* b, T* result, size_t size);

/**
 * Vectorized multiplication
 * @param a First vector
 * @param b Second vector
 * @param result Output vector (same size as input)
 * @param size Number of elements to process
 */
template<typename T>
void multiply(const T* a, const T* b, T* result, size_t size);

/**
 * Vectorized dot product
 * @param a First vector
 * @param b Second vector
 * @param size Number of elements to process
 * @return Dot product result
 */
template<typename T>
T dotProduct(const T* a, const T* b, size_t size);

/**
 * Vectorized vector copy
 * @param source Source vector
 * @param destination Destination vector
 * @param size Number of elements to copy
 */
template<typename T>
void copy(const T* source, T* destination, size_t size);

/**
 * Vectorized fill
 * @param destination Destination vector
 * @param value Fill value
 * @param size Number of elements to fill
 */
template<typename T>
void fill(T* destination, T value, size_t size);

/**
 * Get optimal SIMD block size for given number of elements
 * @param elementCount Number of elements
 * @return Optimal block size
 */
size_t getOptimalBlockSize(size_t elementCount);

/**
 * Process vector in optimal SIMD blocks
 * @param values Input vector
 * @param result Output vector
 * @param size Number of elements
 * @param processor Function to process each block
 */
template<typename T, typename Processor>
void processInBlocks(const T* values, T* result, size_t size, Processor processor);

/**
 * Check if SIMD can be used for given operation
 * @param size Number of elements
 * @return true if SIMD can be used
 */
bool canUseSIMD(size_t size);

/**
 * Get current SIMD level for debugging
 * @return Current SIMD level
 */
SIMDLevel getCurrentSIMDLevel();

} // namespace SIMDUtils

} // namespace nlm
