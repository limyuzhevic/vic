// Novelty Optimizer - Optimized novelty calculation for neural systems
// Provides O(n) implementation with SIMD vectorization

#pragma once

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <limits>

namespace nlm {
/**
 * NoveltyVectorizer - SIMD-optimized novelty calculation utilities
 * 
 * Provides vectorized implementations for computing novelty scores
 * with O(n) complexity instead of O(n²), using SIMD optimizations
 * and template specialization for different array sizes.
 */
class NoveltyVectorizer {
public:
    // SIMD capability detection
    static constexpr bool HAS_AVX512 = (
        defined(__AVX512F__) && defined(__AVX512DQ__)
    );
    static constexpr bool HAS_AVX2 = defined(__AVX2__);
    static constexpr bool HAS_SSE4_1 = defined(__SSE4_1__);
    static constexpr bool HAS_SSE3 = defined(__SSE3__);
    
    // SIMD widths
    static constexpr size_t AVX512_WIDTH = 16;
    static constexpr size_t AVX2_WIDTH = 8;
    static constexpr size_t SSE_WIDTH = 4;
    
    /**
     * Get optimal SIMD width for current hardware
     */
    static constexpr size_t getSIMDWidth() {
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        return AVX512_WIDTH;
#elif defined(__AVX2__)
        return AVX2_WIDTH;
#elif defined(__SSE4_1__) || defined(__SSE3__)
        return SSE_WIDTH;
#else
        return 1;
#endif
    }
    
    /**
     * SIMD-optimized absolute difference sum
     * Uses scalar fallback for non-SIMD architectures
     */
    template<typename ArrayA, typename ArrayB>
    static float computeAbsDiffSum(const ArrayA& a, const ArrayB& b, size_t count) {
        if (count == 0) return 0.0f;
        
        float totalDiff = 0.0f;
        size_t i = 0;
        
        // Scalar fallback for very small arrays
        if (count < 4) {
            for (; i < count; ++i) {
                float diff = a[i] - b[i];
                totalDiff += (diff > 0.0f) ? diff : -diff;
            }
            return totalDiff;
        }
        
        // Use SIMD for larger arrays
        const size_t width = getSIMDWidth();
        
#if defined(__AVX512F__) && defined(__AVX512DQ__) && defined(__x86_64__)
        // AVX-512 implementation (16 floats at once)
        const size_t vectorSize = count - (count % 16);
        
        for (; i < vectorSize; i += 16) {
            __m512 va = _mm512_loadu_ps(&a[i]);
            __m512 vb = _mm512_loadu_ps(&b[i]);
            __m512 vd = _mm512_sub_ps(va, vb);
            
            // Compute absolute value using bit manipulation
            vd = _mm512_and_ps(vd, _mm512_set1_ps(0x7FFFFFFF));
            
            // Horizontal add - accumulate using mask operations
            alignas(64) float temp[16];
            _mm512_storeu_ps(temp, vd);
            for (int j = 0; j < 16; ++j) {
                totalDiff += temp[j];
            }
        }
#elif defined(__AVX2__) && defined(__x86_64__)
        // AVX2 implementation (8 floats at once)
        const size_t vectorSize = count - (count % 8);
        
        for (; i < vectorSize; i += 8) {
            __m256 va = _mm256_loadu_ps(&a[i]);
            __m256 vb = _mm256_loadu_ps(&b[i]);
            __m256 vd = _mm256_sub_ps(va, vb);
            vd = _mm256_and_ps(vd, _mm256_set1_ps(0x7FFFFFFF));
            
            alignas(32) float temp[8];
            _mm256_storeu_ps(temp, vd);
            for (int j = 0; j < 8; ++j) {
                totalDiff += temp[j];
            }
        }
#elif defined(__SSE4_1__) && defined(__x86_64__)
        // SSE4.1 implementation (4 floats at once)
        const size_t vectorSize = count - (count % 4);
        
        for (; i < vectorSize; i += 4) {
            __m128 va = _mm_loadu_ps(&a[i]);
            __m128 vb = _mm_loadu_ps(&b[i]);
            __m128 vd = _mm_sub_ps(va, vb);
            vd = _mm_and_ps(vd, _mm_set1_ps(0x7FFFFFFF));
            
            alignas(16) float temp[4];
            _mm_storeu_ps(temp, vd);
            for (int j = 0; j < 4; ++j) {
                totalDiff += temp[j];
            }
        }
#endif
        
        // Scalar fallback for remaining elements
        for (; i < count; ++i) {
            float diff = a[i] - b[i];
            totalDiff += (diff > 0.0f) ? diff : -diff;
        }
        
        return totalDiff;
    }
    
    /**
     * Optimized novelty calculation with template specialization
     */
    template<typename ArrayA, typename ArrayB>
    static float computeNovelty(const ArrayA& current,
                               const ArrayB& previous,
                               float threshold = 1.0f,
                               float decay = 1.0f) {
        if (current.empty() || previous.empty()) {
            return 0.0f;
        }
        
        size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) {
            return 0.0f;
        }
        
        // Compute sum of absolute differences
        float totalDiff = computeAbsDiffSum(current, previous, minSize);
        
        // Normalize by size
        float avgDiff = totalDiff / static_cast<float>(minSize);
        
        // Apply threshold and decay
        return std::min(1.0f, avgDiff / threshold) * decay;
    }
};

/**
 * NoveltyOptimizer - Main interface for optimized novelty calculations
 * 
 * Provides O(n) implementation of novelty detection with vectorization
 * optimizations for improved performance over the original O(n²) approach.
 */
class NoveltyOptimizer {
public:
    /**
     * Compute sum of absolute differences between two vectors
     * O(n) implementation with loop unrolling
     */
    template<typename ArrayA, typename ArrayB>
    static float computeAbsDiffSum(const ArrayA& a, const ArrayB& b, size_t count) {
        if (count == 0) return 0.0f;
        
        float totalDiff = 0.0f;
        size_t i = 0;
        
        // Unroll loop for better instruction-level parallelism (4x)
        for (; i + 4 <= count; i += 4) {
            // Process 4 elements per iteration
            float diff1 = (a[i] > b[i]) ? (a[i] - b[i]) : (b[i] - a[i]);
            float diff2 = (a[i+1] > b[i+1]) ? (a[i+1] - b[i+1]) : (b[i+1] - a[i+1]);
            float diff3 = (a[i+2] > b[i+2]) ? (a[i+2] - b[i+2]) : (b[i+2] - a[i+2]);
            float diff4 = (a[i+3] > b[i+3]) ? (a[i+3] - b[i+3]) : (b[i+3] - a[i+3]);
            
            totalDiff += diff1 + diff2 + diff3 + diff4;
        }
        
        // Handle remaining elements
        for (; i < count; ++i) {
            float diff = (a[i] > b[i]) ? (a[i] - b[i]) : (b[i] - a[i]);
            totalDiff += diff;
        }
        
        return totalDiff;
    }
    
    /**
     * Compute normalized novelty level from two patterns
     * O(n) implementation with template specialization
     */
    template<typename ArrayA, typename ArrayB>
    static float computeNovelty(const ArrayA& current, const ArrayB& previous,
                               float threshold = 1.0f, float decay = 1.0f) {
        if (current.empty() || previous.empty()) {
            return 0.0f;
        }
        
        size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) {
            return 0.0f;
        }
        
        // Compute sum of absolute differences
        float totalDiff = computeAbsDiffSum(current, previous, minSize);
        
        // Normalize by size
        float avgDiff = totalDiff / static_cast<float>(minSize);
        
        // Apply threshold and decay
        return std::min(1.0f, avgDiff / threshold) * decay;
    }
    
    /**
     * Optimized scalar absolute difference calculation
     */
    static inline float absDiff(float a, float b) {
        return std::abs(a - b);
    }
    
    /**
     * Unrolled absolute difference sum for very small arrays
     */
    template<typename ArrayA, typename ArrayB>
    static float computeSmallAbsDiffSum(const ArrayA& a, const ArrayB& b, size_t count) {
        if (count == 0) return 0.0f;
        
        float totalDiff = 0.0f;
        size_t i = 0;
        
        // Process 4 elements at a time for small arrays
        if (count >= 4) {
            for (; i <= count - 4; i += 4) {
                totalDiff += absDiff(a[i], b[i]);
                totalDiff += absDiff(a[i+1], b[i+1]);
                totalDiff += absDiff(a[i+2], b[i+2]);
                totalDiff += absDiff(a[i+3], b[i+3]);
            }
        }
        
        // Process remaining elements
        for (; i < count; ++i) {
            totalDiff += absDiff(a[i], b[i]);
        }
        
        return totalDiff;
    }
};

} // namespace nlm
