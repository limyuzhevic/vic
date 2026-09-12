#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <limits>

namespace nlm {
/**
 * SIMD-accelerated utility functions for neural computation
 * 
 * Provides optimized implementations of common neural network operations
 * using SIMD intrinsics when available, with scalar fallbacks.
 */
class SIMDUtils {
public:
    // SIMD detection flags
    static constexpr bool HAS_AVX512 = (__AVX512F__ && __AVX512DQ__);
    static constexpr bool HAS_AVX2 = __AVX2__;
    static constexpr bool HAS_SSE4_1 = __SSE4_1__;
    static constexpr bool HAS_SSE3 = __SSE3__;
    static constexpr bool HAS_SSE2 = __SSE2__;
    
    // SIMD width in float elements
    static constexpr size_t AVX512_WIDTH = 16;
    static constexpr size_t AVX2_WIDTH = 8;
    static constexpr size_t SSE_WIDTH = 4;
    
    /**
     * Determine best available SIMD width
     */
    static constexpr size_t getBestSIMDWidth() {
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        return AVX512_WIDTH;
#elif defined(__AVX2__)
        return AVX2_WIDTH;
#elif defined(__SSE4_1__) || defined(__SSE3__) || defined(__SSE2__)
        return SSE_WIDTH;
#else
        return 1;
#endif
    }
    
    /**
     * Get runtime SIMD capability
     */
    static size_t getRuntimeSIMDWidth() {
        size_t width = 1;
        
#if defined(_MSC_VER)
        // Windows: use __cpuid
        int cpuInfo[4];
        // Simplified check - in production would check all features
        width = 1;
#else
        // Linux/Mac: assume best available at compile time
        width = getBestSIMDWidth();
#endif
        
        return width;
    }
    
    /**
     * Compute absolute difference with scalar fallback
     */
    static inline float absDiff(float a, float b) {
        return std::abs(a - b);
    }
    
    /**
     * SIMD-optimized sum of absolute differences
     * 
     * @param a First array
     * @param b Second array  
     * @param count Number of elements
     * @return Sum of absolute differences
     */
    template<typename ArrayA, typename ArrayB>
    static float absDiffSum(const ArrayA& a, const ArrayB& b, size_t count) {
        if (count == 0) return 0.0f;
        
        float total = 0.0f;
        size_t i = 0;
        
        // Use scalar fallback for very small arrays
        if (count < 4) {
            for (; i < count; ++i) {
                total += absDiff(a[i], b[i]);
            }
            return total;
        }
        
        // Use SIMD for larger arrays
        const size_t width = getRuntimeSIMDWidth();
        
        if (width >= 16 && count >= 16) {
            // AVX-512 implementation
            const size_t vectorSize = count - (count % 16);
            
            for (; i < vectorSize; i += 16) {
                __m512 va = _mm512_loadu_ps(&a[i]);
                __m512 vb = _mm512_loadu_ps(&b[i]);
                __m512 vd = _mm512_sub_ps(va, vb);
                vd = _mm512_and_ps(vd, _mm512_set1_ps(std::numeric_limits<float>::max() * 0.5f));
                
                // Horizontal add - accumulate manually
                alignas(64) float temp[16];
                _mm512_storeu_ps(temp, vd);
                for (int j = 0; j < 16; ++j) {
                    total += temp[j];
                }
            }
        } else if (width >= 8 && count >= 8) {
            // AVX2 implementation
            const size_t vectorSize = count - (count % 8);
            
            for (; i < vectorSize; i += 8) {
                __m256 va = _mm256_loadu_ps(&a[i]);
                __m256 vb = _mm256_loadu_ps(&b[i]);
                __m256 vd = _mm256_sub_ps(va, vb);
                vd = _mm256_and_ps(vd, _mm256_set1_ps(std::numeric_limits<float>::max() * 0.5f));
                
                alignas(32) float temp[8];
                _mm256_storeu_ps(temp, vd);
                for (int j = 0; j < 8; ++j) {
                    total += temp[j];
                }
            }
        } else if (width >= 4 && count >= 4) {
            // SSE implementation
            const size_t vectorSize = count - (count % 4);
            
            for (; i < vectorSize; i += 4) {
                __m128 va = _mm_loadu_ps(&a[i]);
                __m128 vb = _mm_loadu_ps(&b[i]);
                __m128 vd = _mm_sub_ps(va, vb);
                vd = _mm_and_ps(vd, _mm_set1_ps(std::numeric_limits<float>::max() * 0.5f));
                
                alignas(16) float temp[4];
                _mm_storeu_ps(temp, vd);
                for (int j = 0; j < 4; ++j) {
                    total += temp[j];
                }
            }
        }
        
        // Scalar fallback for remaining elements
        for (; i < count; ++i) {
            total += absDiff(a[i], b[i]);
        }
        
        return total;
    }
    
    /**
     * Optimized novelty calculation template
     */
    template<typename ArrayA, typename ArrayB>
    static float computeNoveltyLevel(const ArrayA& current, 
                                     const ArrayB& previous,
                                     float threshold = 1.0f) {
        if (current.empty() || previous.empty()) {
            return 0.0f;
        }
        
        const size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) {
            return 0.0f;
        }
        
        // Compute sum of absolute differences
        float totalDiff = absDiffSum(current, previous, minSize);
        
        // Normalize by size
        float avgDiff = totalDiff / static_cast<float>(minSize);
        
        // Compute novelty level
        return std::min(1.0f, avgDiff / threshold);
    }
};

/**
 * Specialized vectorized novelty computation for different sizes
 */
template<size_t SIZE>
struct NoveltyVectorized {
    static float compute(const std::vector<float>& current,
                         const std::vector<float>& previous,
                         float decay) {
        // Generic implementation for specific sizes
        const size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) return 0.0f;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            totalDiff += SIMDUtils::absDiff(current[i], previous[i]);
        }
        
        return (totalDiff / static_cast<float>(minSize)) * decay;
    }
};

// Template specializations for common sizes

template<size_t SIZE>
struct NoveltyVectorized<SIZE * 2> {
    static float compute(const std::vector<float>& current,
                         const std::vector<float>& previous,
                         float decay) {
        const size_t minSize = std::min(current.size(), previous.size());
        if (minSize < SIZE * 2) {
            return NoveltyVectorized<SIZE>::compute(current, previous, decay);
        }
        
        float totalDiff = 0.0f;
        
        // Process in blocks of SIZE for optimal cache utilization
        for (size_t i = 0; i < minSize; i += SIZE) {
            const size_t blockSize = std::min(SIZE, minSize - i);
            for (size_t j = 0; j < blockSize; ++j) {
                totalDiff += SIMDUtils::absDiff(current[i + j], previous[i + j]);
            }
        }
        
        return (totalDiff / static_cast<float>(minSize)) * decay;
    }
};

// Specialized implementations for common sizes
namespace NoveltyOptimized {
    
    /**
     * Compute absolute difference sum with template specialization
     */
    template<typename ArrayA, typename ArrayB>
    float computeAbsDiff(const ArrayA& a, const ArrayB& b, size_t count) {
        return SIMDUtils::absDiffSum(a, b, count);
    }
    
    /**
     * Compute novelty level with automatic SIMD
     */
    template<typename ArrayA, typename ArrayB>
    float computeNovelty(const ArrayA& current,
                        const ArrayB& previous,
                        float threshold = 1.0f,
                        float decay = 1.0f) {
        if (current.empty() || previous.empty()) {
            return 0.0f;
        }
        
        const size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) return 0.0f;
        
        float totalDiff = computeAbsDiff(current, previous, minSize);
        float avgDiff = totalDiff / static_cast<float>(minSize);
        
        return std::min(1.0f, avgDiff / threshold) * decay;
    }
}

} // namespace nlm
