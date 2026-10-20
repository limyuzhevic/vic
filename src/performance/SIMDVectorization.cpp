// SIMDVectorization.cpp - Implementation of SIMD optimizations for neural computation

#include "SIMDVectorization.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>

namespace nlm {

// Vectorized statistics implementation (scalar fallback)

template<typename Array>
inline float VectorizedStats::sum(const Array& arr, size_t count) {
    float result = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        result += arr[i];
    }
    return result;
}

template<typename Array>
inline float VectorizedStats::mean(const Array& arr, size_t count) {
    if (count == 0) return 0.0f;
    return sum(arr, count) / static_cast<float>(count);
}

template<typename Array>
inline float VectorizedStats::variance(const Array& arr, size_t count) {
    if (count < 2) return 0.0f;
    float m = mean(arr, count);
    float sumSq = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
}

template<typename Array>
inline float VectorizedStats::max(const Array& arr, size_t count) {
    if (count == 0) return 0.0f;
    float result = arr[0];
    for (size_t i = 1; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
}

template<typename Array>
inline void VectorizedStats::clamp(Array& arr, size_t count, float minVal, float maxVal) {
    for (size_t i = 0; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
}

// Vectorized synapse STD (short-term depression/facilitation) implementation

template<typename DArray, typename FArray>
void VectorizedSynapse::updateSTD(DArray& depression, FArray& facilitation,
                                 const float* lastPreTime, const float* lastPostTime,
                                 float currentTime, size_t count) {
    // Scalar implementation for short-term plasticity
    for (size_t i = 0; i < count; ++i) {
        float lastPre = lastPreTime[i];
        float lastPost = lastPostTime[i];
        
        if (lastPre >= 0.0f) {
            float delta = currentTime - lastPre;
            facilitation[i] *= std::exp(-delta / tau_depression);
        }
        
        if (lastPre >= 0.0f || lastPost >= 0.0f) {
            float delta = std::max(
                lastPost >= 0.0f ? currentTime - lastPost : 0.0f,
                lastPre >= 0.0f ? currentTime - lastPre : 0.0f
            );
            // Recovery toward U_max
            depression[i] += (U_max - depression[i]) * (1.0f - std::exp(-delta / tau_facilitation));
            
            // Cap depression
            if (depression[i] > U_max) depression[i] = U_max;
        }
    }
}

// Optimized VectorizedLIF update - Scalar implementation for clarity
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                              FArray& fired, size_t count) {
    size_t numFired = 0;
    
    // Pre-compute constants
    float leakCoeff = dt / tau;
    float inputCoeff = dt / C;
    
    // Vectorized processing using SIMD when available
    if constexpr (sizeof(float) * SIMDUtils::SIMD_WIDTH > 4) {
        #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
        return vectorizedUpdateAVX512(V_inout, I, refractoryRemaining, fired, count, leakCoeff, inputCoeff);
        #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
        return vectorizedUpdateAVX2(V_inout, I, refractoryRemaining, fired, count, leakCoeff, inputCoeff);
        #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
        return vectorizedUpdateSSE(V_inout, I, refractoryRemaining, fired, count, leakCoeff, inputCoeff);
        #endif
    }
    
    // Scalar fallback
    for (size_t i = 0; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            refractoryRemaining[i]--;
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                numFired++;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

// Vectorized statistics sum implementation
inline float VectorizedStats::sum(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    float result = 0.0f;
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vResult = _mm512_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        vResult = _mm512_add_ps(vResult, v);
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vResult);
    for (size_t j = 0; j < 16; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vResult = _mm256_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        vResult = _mm256_add_ps(vResult, v);
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vResult);
    for (size_t j = 0; j < 8; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vResult = _mm_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        vResult = _mm_add_ps(vResult, v);
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vResult);
    for (size_t j = 0; j < 4; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    #else
    for (size_t i = 0; i < count; ++i) {
        result += arr[i];
    }
    return result;
    #endif
}

// Vectorized statistics mean implementation
inline float VectorizedStats::mean(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    float sum = sum(arr, count);
    return sum / static_cast<float>(count);
}

// Vectorized statistics variance implementation
inline float VectorizedStats::variance(const float* arr, size_t count) {
    if (count < 2) return 0.0f;
    
    float m = mean(arr, count);
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMean = _mm512_set1_ps(m);
    __m512 vSumSq = _mm512_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        __m512 vDiff = _mm512_sub_ps(v, vMean);
        vSumSq = _mm512_add_ps(vSumSq, _mm512_mul_ps(vDiff, vDiff));
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 16; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMean = _mm256_set1_ps(m);
    __m256 vSumSq = _mm256_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        __m256 vDiff = _mm256_sub_ps(v, vMean);
        vSumSq = _mm256_add_ps(vSumSq, _mm256_mul_ps(vDiff, vDiff));
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 8; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMean = _mm_set1_ps(m);
    __m128 vSumSq = _mm_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        __m128 vDiff = _mm_sub_ps(v, vMean);
        vSumSq = _mm_add_ps(vSumSq, _mm_mul_ps(vDiff, vDiff));
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 4; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    #else
    float sumSq = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    #endif
}

// Vectorized statistics max implementation
inline float VectorizedStats::max(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMax = _mm512_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        vMax = _mm512_max_ps(vMax, v);
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 16; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMax = _mm256_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        vMax = _mm256_max_ps(vMax, v);
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 8; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMax = _mm_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        vMax = _mm_max_ps(vMax, v);
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 4; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    #else
    float result = arr[0];
    for (size_t i = 1; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    #endif
}

// Vectorized statistics clamp implementation
inline void VectorizedStats::clamp(float* arr, size_t count, float minVal, float maxVal) {
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMin = _mm512_set1_ps(minVal);
    __m512 vMax = _mm512_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        v = _mm512_max_ps(v, vMin);
        v = _mm512_min_ps(v, vMax);
        _mm512_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMin = _mm256_set1_ps(minVal);
    __m256 vMax = _mm256_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        v = _mm256_max_ps(v, vMin);
        v = _mm256_min_ps(v, vMax);
        _mm256_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMin = _mm_set1_ps(minVal);
    __m128 vMax = _mm_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        v = _mm_max_ps(v, vMin);
        v = _mm_min_ps(v, vMax);
        _mm_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    #else
    for (size_t i = 0; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    #endif
}

// SIMD-optimized implementations
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::vectorizedUpdateAVX512(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                                            FArray& fired, size_t count, float leakCoeff, float inputCoeff) {
    __m512 vmRest = _mm512_set1_ps(V_rest);
    __m512 vmReset = _mm512_set1_ps(V_reset);
    __m512 vmThresh = _mm512_set1_ps(V_threshold);
    __m512 vLeakCoeff = _mm512_set1_ps(leakCoeff);
    __m512 vInputCoeff = _mm512_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    size_t numFired = 0;
    
    for (; i < vectorSize; i += 16) {
        // Load membrane potentials and inputs
        __m512 vV = _mm512_loadu_ps(&V_inout[i]);
        __m512 vI = _mm512_loadu_ps(&I[i]);
        
        // Calculate leak term
        __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term
        __m512 vInput = _mm512_mul_ps(vI, vInputCoeff);
        
        // Update membrane potential
        __m512 vUpdate = _mm512_mul_ps(_mm512_add_ps(vLeak, vInput), _mm512_set1_ps(1000.0f));
        vV = _mm512_add_ps(vV, vUpdate);
        
        // Check for spikes
        __m512 maskSpike = _mm512_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        vV = _mm512_blend_ps(vV, vmReset, maskSpike);
        
        // Store updated membrane potentials
        _mm512_storeu_ps(&V_inout[i], vV);
        
        // Set fired flags
        for (int j = 0; j < 16; ++j) {
            fired[i + j] = (_mm512_extract_epi32(_mm512_castps_si512(maskSpike), j) >> 31) & 1;
            if (fired[i + j]) numFired++;
        }
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                numFired++;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

// Vectorized statistics sum implementation
inline float VectorizedStats::sum(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    float result = 0.0f;
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vResult = _mm512_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        vResult = _mm512_add_ps(vResult, v);
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vResult);
    for (size_t j = 0; j < 16; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vResult = _mm256_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        vResult = _mm256_add_ps(vResult, v);
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vResult);
    for (size_t j = 0; j < 8; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vResult = _mm_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        vResult = _mm_add_ps(vResult, v);
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vResult);
    for (size_t j = 0; j < 4; ++j) {
        result += partial[j];
    }
    
    for (; i < count; ++i) {
        result += arr[i];
    }
    return result;
    #else
    for (size_t i = 0; i < count; ++i) {
        result += arr[i];
    }
    return result;
    #endif
}

// Vectorized statistics mean implementation
inline float VectorizedStats::mean(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    float sum = sum(arr, count);
    return sum / static_cast<float>(count);
}

// Vectorized statistics variance implementation
inline float VectorizedStats::variance(const float* arr, size_t count) {
    if (count < 2) return 0.0f;
    
    float m = mean(arr, count);
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMean = _mm512_set1_ps(m);
    __m512 vSumSq = _mm512_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        __m512 vDiff = _mm512_sub_ps(v, vMean);
        vSumSq = _mm512_add_ps(vSumSq, _mm512_mul_ps(vDiff, vDiff));
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 16; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMean = _mm256_set1_ps(m);
    __m256 vSumSq = _mm256_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        __m256 vDiff = _mm256_sub_ps(v, vMean);
        vSumSq = _mm256_add_ps(vSumSq, _mm256_mul_ps(vDiff, vDiff));
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 8; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMean = _mm_set1_ps(m);
    __m128 vSumSq = _mm_setzero_ps();
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        __m128 vDiff = _mm_sub_ps(v, vMean);
        vSumSq = _mm_add_ps(vSumSq, _mm_mul_ps(vDiff, vDiff));
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vSumSq);
    float sumSq = 0.0f;
    for (size_t j = 0; j < 4; ++j) {
        sumSq += partial[j];
    }
    
    for (; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    #else
    float sumSq = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
    #endif
}

// Vectorized statistics max implementation
inline float VectorizedStats::max(const float* arr, size_t count) {
    if (count == 0) return 0.0f;
    
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMax = _mm512_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        vMax = _mm512_max_ps(vMax, v);
    }
    
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 16; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMax = _mm256_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        vMax = _mm256_max_ps(vMax, v);
    }
    
    alignas(32) float partial[8];
    _mm256_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 8; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMax = _mm_loadu_ps(arr);
    size_t i = 1;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        vMax = _mm_max_ps(vMax, v);
    }
    
    alignas(16) float partial[4];
    _mm_store_ps(partial, vMax);
    float result = partial[0];
    for (size_t j = 1; j < 4; ++j) {
        if (partial[j] > result) result = partial[j];
    }
    
    for (; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    #else
    float result = arr[0];
    for (size_t i = 1; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
    #endif
}

// Vectorized statistics clamp implementation
inline void VectorizedStats::clamp(float* arr, size_t count, float minVal, float maxVal) {
    #if defined(NLM_SIMD_AVX512) && defined(NLM_SIMD_WIDTH)
    __m512 vMin = _mm512_set1_ps(minVal);
    __m512 vMax = _mm512_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(arr + i);
        v = _mm512_max_ps(v, vMin);
        v = _mm512_min_ps(v, vMax);
        _mm512_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    
    #elif defined(NLM_SIMD_AVX2) && defined(NLM_SIMD_WIDTH)
    __m256 vMin = _mm256_set1_ps(minVal);
    __m256 vMax = _mm256_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 v = _mm256_loadu_ps(arr + i);
        v = _mm256_max_ps(v, vMin);
        v = _mm256_min_ps(v, vMax);
        _mm256_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    
    #elif defined(NLM_SIMD_SSE) && defined(NLM_SIMD_WIDTH)
    __m128 vMin = _mm_set1_ps(minVal);
    __m128 vMax = _mm_set1_ps(maxVal);
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 v = _mm_loadu_ps(arr + i);
        v = _mm_max_ps(v, vMin);
        v = _mm_min_ps(v, vMax);
        _mm_storeu_ps(arr + i, v);
    }
    
    for (; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    #else
    for (size_t i = 0; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
    #endif
}

// SIMD-optimized implementations
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::vectorizedUpdateAVX512(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                                            FArray& fired, size_t count, float leakCoeff, float inputCoeff) {
    __m512 vmRest = _mm512_set1_ps(V_rest);
    __m512 vmReset = _mm512_set1_ps(V_reset);
    __m512 vmThresh = _mm512_set1_ps(V_threshold);
    __m512 vLeakCoeff = _mm512_set1_ps(leakCoeff);
    __m512 vInputCoeff = _mm512_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    size_t numFired = 0;
    
    for (; i < vectorSize; i += 16) {
        // Load membrane potentials and inputs
        __m512 vV = _mm512_loadu_ps(&V_inout[i]);
        __m512 vI = _mm512_loadu_ps(&I[i]);
        
        // Calculate leak term
        __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term
        __m512 vInput = _mm512_mul_ps(vI, vInputCoeff);
        
        // Update membrane potential
        __m512 vUpdate = _mm512_mul_ps(_mm512_add_ps(vLeak, vInput), _mm512_set1_ps(1000.0f));
        vV = _mm512_add_ps(vV, vUpdate);
        
        // Check for spikes
        __m512 maskSpike = _mm512_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        vV = _mm512_blend_ps(vV, vmReset, maskSpike);
        
        // Store updated membrane potentials
        _mm512_storeu_ps(&V_inout[i], vV);
        
        // Set fired flags
        for (int j = 0; j < 16; ++j) {
            fired[i + j] = (_mm512_extract_epi32(_mm512_castps_si512(maskSpike), j) >> 31) & 1;
            if (fired[i + j]) numFired++;
        }
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                numFired++;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

// SIMD-optimized implementations
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::vectorizedUpdateAVX2(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                                         FArray& fired, size_t count, float leakCoeff, float inputCoeff) {
    __m256 vmRest = _mm256_set1_ps(V_rest);
    __m256 vmReset = _mm256_set1_ps(V_reset);
    __m256 vmThresh = _mm256_set1_ps(V_threshold);
    __m256 vLeakCoeff = _mm256_set1_ps(leakCoeff);
    __m256 vInputCoeff = _mm256_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    size_t numFired = 0;
    
    for (; i < vectorSize; i += 8) {
        __m256 vV = _mm256_loadu_ps(&V_inout[i]);
        __m256 vI = _mm256_loadu_ps(&I[i]);
        
        __m256 vLeak = _mm256_mul_ps(_mm256_sub_ps(vmRest, vV), vLeakCoeff);
        __m256 vInput = _mm256_mul_ps(vI, vInputCoeff);
        
        __m256 vUpdate = _mm256_mul_ps(_mm256_add_ps(vLeak, vInput), _mm256_set1_ps(1000.0f));
        vV = _mm256_add_ps(vV, vUpdate);
        
        __m256 maskSpike = _mm256_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        vV = _mm256_blend_ps(vV, vmReset, maskSpike);
        
        _mm256_storeu_ps(&V_inout[i], vV);
        
        for (int j = 0; j < 8; ++j) {
            fired[i + j] = (_mm256_extract_epi32(_mm256_castps_si256(maskSpike), j) >> 31) & 1;
            if (fired[i + j]) numFired++;
        }
    }
    
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                numFired++;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

// SIMD-optimized implementations
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::vectorizedUpdateSSE(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                                        FArray& fired, size_t count, float leakCoeff, float inputCoeff) {
    __m128 vmRest = _mm_set1_ps(V_rest);
    __m128 vmReset = _mm_set1_ps(V_reset);
    __m128 vmThresh = _mm_set1_ps(V_threshold);
    __m128 vLeakCoeff = _mm_set1_ps(leakCoeff);
    __m128 vInputCoeff = _mm_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    size_t numFired = 0;
    
    for (; i < vectorSize; i += 4) {
        __m128 vV = _mm_loadu_ps(&V_inout[i]);
        __m128 vI = _mm_loadu_ps(&I[i]);
        
        __m128 vLeak = _mm_mul_ps(_mm_sub_ps(vmRest, vV), vLeakCoeff);
        __m128 vInput = _mm_mul_ps(vI, vInputCoeff);
        
        __m128 vUpdate = _mm_mul_ps(_mm_add_ps(vLeak, vInput), _mm_set1_ps(1000.0f));
        vV = _mm_add_ps(vV, vUpdate);
        
        __m128 maskSpike = _mm_cmpge_ps(vV, vmThresh);
        vV = _mm_blend_ps(vV, vmReset, maskSpike);
        
        _mm_storeu_ps(&V_inout[i], vV);
        
        for (int j = 0; j < 4; ++j) {
            fired[i + j] = ((_mm_extract_epi32(_mm_castps_si128(maskSpike), j) >> 31) & 1);
            if (fired[i + j]) numFired++;
        }
    }
    
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                numFired++;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

} // namespace nlm
