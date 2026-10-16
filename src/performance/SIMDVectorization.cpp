#include "performance/Performance.hpp"
#include <iostream>
#include <cmath>
#include <xmmintrin.h>  // SSE instructions

namespace nlm {

void SIMDVectorizer::add(const float* a, const float* b, float* result, size_t size) {
    // Use SIMD if available (SSE/AVX)
    size_t i = 0;
    
    // Process 4 floats at a time with SSE
    for (; i + 3 < size; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 vr = _mm_add_ps(va, vb);
        _mm_storeu_ps(&result[i], vr);
    }
    
    // Process remaining elements
    for (; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}

float SIMDVectorizer::dotProduct(const float* a, const float* b, size_t size) {
    float result = 0.0f;
    size_t i = 0;
    
    // Use SIMD for dot product
    __m128 sum = _mm_setzero_ps();
    
    for (; i + 3 < size; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        sum = _mm_add_ps(sum, _mm_mul_ps(va, vb));
    }
    
    // Horizontal sum
    alignas(16) float temp[4];
    _mm_store_ps(temp, sum);
    result = temp[0] + temp[1] + temp[2] + temp[3];
    
    // Process remaining elements
    for (; i < size; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

void SIMDVectorizer::normalize(float* vector, size_t size) {
    float magnitude = 0.0f;
    size_t i = 0;
    
    // Use SIMD for magnitude calculation
    __m128 sum = _mm_setzero_ps();
    
    for (; i + 3 < size; i += 4) {
        __m128 v = _mm_loadu_ps(&vector[i]);
        sum = _mm_add_ps(sum, _mm_mul_ps(v, v));
    }
    
    // Horizontal sum
    alignas(16) float temp[4];
    _mm_store_ps(temp, sum);
    for (int j = 0; j < 4; ++j) magnitude += temp[j];
    
    // Process remaining elements
    for (; i < size; ++i) {
        magnitude += vector[i] * vector[i];
    }
    
    magnitude = std::sqrt(magnitude);
    
    if (magnitude > 0.0f) {
        float invMagnitude = 1.0f / magnitude;
        __m128 invMag = _mm_set1_ps(invMagnitude);
        
        i = 0;
        for (; i + 3 < size; i += 4) {
            __m128 v = _mm_loadu_ps(&vector[i]);
            v = _mm_mul_ps(v, invMag);
            _mm_storeu_ps(&vector[i], v);
        }
        
        for (; i < size; ++i) {
            vector[i] *= invMagnitude;
        }
    }
}

// ParallelProcessing implementation

// OptimizedBrain implementation

// EventQueue implementation

// CheckpointSystem implementation

} // namespace nlm