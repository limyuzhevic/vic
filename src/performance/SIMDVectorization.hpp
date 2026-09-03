#pragma once

/**
 * SIMDVectorization - SIMD vectorization for neural computation
 * 
 * Provides vectorized operations for neural simulation using:
 * - SSE (Streaming SIMD Extensions) - 4 floats
 * - AVX2 (Advanced Vector Extensions 2) - 8 floats  
 * - AVX-512 - 16 floats
 * - Fallback to scalar operations
 * 
 * Targets:
 * - Membrane potential updates
 * - Synaptic integration
 * - Refractory calculations
 * - Plasticity calculations
 * 
 * Features:
 * - Automatic detection of best SIMD width
 * - Portable high-level interface
 * - Scalar fallback for correctness
 * - Aligned memory operations
 */

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <array>
#include <algorithm>
#include <type_traits>

// SIMD configuration
#if defined(__AVX512F__) && defined(__AVX512DQ__)
    #define NLM_SIMD_AVX512 1
    #define NLM_SIMD_WIDTH 16
    #define NLM_SIMD_FLOAT_MUL_ADD _mm512_mul_ps
    #define NLM_SIMD_FLOAT_ADD _mm512_add_ps
    #define NLM_SIMD_FLOAT_STORE _mm512_storeu_ps
    #define NLM_SIMD_FLOAT_LOAD _mm512_loadu_ps
    #define NLM_SIMD_MASK_STORE _mm512_mask_storeu_ps
    #include <immintrin.h>
#elif defined(__AVX2__)
    #define NLM_SIMD_AVX2 1
    #define NLM_SIMD_WIDTH 8
    #define NLM_SIMD_FLOAT_MUL_ADD _mm256_mul_ps
    #define NLM_SIMD_FLOAT_ADD _mm256_add_ps
    #define NLM_SIMD_FLOAT_STORE _mm256_storeu_ps
    #define NLM_SIMD_FLOAT_LOAD _mm256_loadu_ps
    #include <immintrin.h>
#elif defined(__SSE4_1__) || defined(__SSE3__)
    #define NLM_SIMD_SSE 1
    #define NLM_SIMD_WIDTH 4
    #define NLM_SIMD_FLOAT_MUL_ADD _mm_mul_ps
    #define NLM_SIMD_FLOAT_ADD _mm_add_ps
    #define NLM_SIMD_FLOAT_STORE _mm_storeu_ps
    #define NLM_SIMD_FLOAT_LOAD _mm_loadu_ps
    #include <xmmintrin.h>
    #include <pmmintrin.h>
#else
    #define NLM_SIMD_SCALAR 1
    #define NLM_SIMD_WIDTH 1
#endif

namespace nlm {

/**
 * SIMD utilities for portable vectorized operations
 */
class SIMDUtils {
public:
    static constexpr size_t SIMD_WIDTH = NLM_SIMD_WIDTH;
    
    /**
     * Check if SIMD is available
     */
    static bool isSIMDAvailable() {
#if defined(NLM_SIMD_AVX512) || defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
        return true;
#else
        return false;
#endif
    }
    
    /**
     * Get SIMD type name
     */
    static const char* getSIMDType() {
#if defined(NLM_SIMD_AVX512)
        return "AVX-512";
#elif defined(NLM_SIMD_AVX2)
        return "AVX2";
#elif defined(NLM_SIMD_SSE)
        return "SSE";
#else
        return "Scalar";
#endif
    }
    
    /**
     * Get optimal memory alignment for SIMD loads/stores
     */
    static constexpr size_t alignment() {
#if defined(NLM_SIMD_AVX512)
        return 64;
#elif defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
        return 32;
#else
        return 16;
#endif
    }
};

/**
 * Vectorized LIF (Leaky Integrate-and-Fire) update
 * 
 * Performs batch update of multiple neurons using SIMD.
 * 
 * Mathematical model (per neuron):
 *   dV/dt = (V_rest - V)/tau + I/C
 *   
 * Discrete update:
 *   V_new = V + dt * ((V_rest - V)/tau + I/C)
 *   
 *   if V_new >= threshold:
 *       spike = true
 *       V = V_reset
 */
struct VectorizedLIF {
    // LIF parameters (broadcast to all lanes)
    float V_rest;
    float V_reset;
    float V_threshold;
    float tau;           // Time constant (ms)
    float C;             // Membrane capacitance (nF)
    float dt;            // Timestep (ms)
    float refractoryDecay;  // Refractory period decay
    
    VectorizedLIF()
        : V_rest(-70.0f), V_reset(-70.0f), V_threshold(-55.0f)
        , tau(20.0f), C(1.0f), dt(1.0f), refractoryDecay(1.0f) {}
    
    /**
     * Update membrane potentials for a batch of neurons
     * @param V_inout Array of membrane potentials (modified in place)
     * @param I Array of synaptic inputs
     * @param refractoryRemaining Array of refractory counters
     * @param count Number of neurons
     * @param fired Output array indicating which neurons fired
     * @return Number of neurons that fired
     */
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    size_t update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                   FArray& fired, size_t count);
};

/**
 * Vectorized synapse update
 */
struct VectorizedSynapse {
    float tau_facilitation;   // Facilitation time constant
    float tau_depression;     // Depression time constant
    float U_max;             // Max utilization
    
    VectorizedSynapse()
        : tau_facilitation(100.0f), tau_depression(200.0f), U_max(1.0f) {}
    
    /**
     * Update short-term plasticity state
     */
    template<typename DArray, typename FArray>
    void updateSTD(DArray& depression, FArray& facilitation,
                   const float* lastPreTime, const float* lastPostTime,
                   float currentTime, size_t count);
};

/**
 * Vectorized STDP update
 */
struct VectorizedSTDP {
    float tau_plus;   // LTP time constant
    float tau_minus;  // LTD time constant
    float A_plus;     // LTP amplitude
    float A_minus;    // LTD amplitude
    float w_max;      // Max weight
    float w_min;      // Min weight
    
    VectorizedSTDP()
        : tau_plus(20.0f), tau_minus(20.0f), A_plus(0.01f), A_minus(0.012f)
        , w_max(1.0f), w_min(-1.0f) {}
    
    /**
     * Calculate weight update for a pair of pre/post spike times
     */
    float calculateDeltaW(float dt);
    
    /**
     * Vectorized weight update
     */
    template<typename WArray, typename PreArray, typename PostArray>
    void updateWeights(WArray& weights, const PreArray& preSpikes,
                       const PostArray& postSpikes, size_t numPre, size_t numPost);
};

/**
 * Vectorized statistics computation
 */
struct VectorizedStats {
    /**
     * Compute sum of array elements
     */
    template<typename Array>
    static float sum(const Array& arr, size_t count);
    
    /**
     * Compute mean of array elements
     */
    template<typename Array>
    static float mean(const Array& arr, size_t count);
    
    /**
     * Compute variance of array elements
     */
    template<typename Array>
    static float variance(const Array& arr, size_t count);
    
    /**
     * Compute max of array elements
     */
    template<typename Array>
    static float max(const Array& arr, size_t count);
    
    /**
     * Clamp array values to range
     */
    template<typename Array>
    static void clamp(Array& arr, size_t count, float minVal, float maxVal);
};

// Inline SIMD implementations

inline float VectorizedSTDP::calculateDeltaW(float dt) {
    if (dt > 0.0f) {
        // Pre before post: LTP
        return A_plus * std::exp(-dt / tau_plus);
    } else if (dt < 0.0f) {
        // Post before pre: LTD
        return -A_minus * std::exp(dt / tau_minus);  // dt is negative
    }
    return 0.0f;
}

// Scalar fallback implementations

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

// Vectorized LIF update implementation

template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, 
                              RArray& refractoryRemaining,
                              FArray& fired, size_t count) {
    size_t numFired = 0;
    
    // Pre-compute constants
    float leakCoeff = dt / tau;  // (dt * 1000) factor handled in caller
    float inputCoeff = dt / C;
    float expLeak = std::exp(-dt / tau);
    
#if defined(NLM_SIMD_AVX512)
    __m512 vmRest = _mm512_set1_ps(V_rest);
    __m512 vmReset = _mm512_set1_ps(V_reset);
    __m512 vmThresh = _mm512_set1_ps(V_threshold);
    __m512 vLeakCoeff = _mm512_set1_ps(leakCoeff);
    __m512 vInputCoeff = _mm512_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        // Load membrane potentials
        __m512 vV = _mm512_loadu_ps(&V_inout[i]);
        __m512 vI = _mm512_loadu_ps(&I[i]);
        
        // Load refractory counters as float
        alignas(64) float refractoryFloat[16];
        for (int j = 0; j < 16; ++j) {
            refractoryFloat[j] = static_cast<float>(refractoryRemaining[i + j]);
        }
        __m512 vRefractory = _mm512_load_ps(refractoryFloat);
        
        // Create mask for neurons still in refractory
        __m512 maskRefractory = _mm512_cmp_ps(vRefractory, _mm512_setzero_ps(), _CMP_GT_OQ);
        
        // Calculate leak term: (V_rest - V) / tau
        __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term: I / C
        __m512 vInput = _mm512_mul_ps(vI, vInputCoeff);
        
        // Update V = V + dt * (leak + input)
        __m512 vUpdate = _mm512_mul_ps(_mm512_add_ps(vLeak, vInput), _mm512_set1_ps(1000.0f));
        vV = _mm512_add_ps(vV, vUpdate);
        
        // Apply refractory: V = V_rest during refractory
        vV = _mm512_blend_ps(vV, vmRest, maskRefractory);
        
        // Decay refractory counters
        vRefractory = _mm512_sub_ps(vRefractory, _mm512_set1_ps(1.0f));
        
        // Check for spikes: V >= threshold
        __m512 maskSpike = _mm512_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        vV = _mm512_blend_ps(vV, vmReset, maskSpike);
        
        // Store updated values
        _mm512_storeu_ps(&V_inout[i], vV);
        
        // Update refractory counters
        alignas(64) float refractoryOut[16];
        _mm512_store_ps(refractoryOut, vRefractory);
        for (int j = 0; j < 16; ++j) {
            refractoryRemaining[i + j] = static_cast<uint32_t>(refractoryOut[j]);
        }
        
        // Record fired neurons
        alignas(64) uint32_t firedMask[16];
        _mm512_storeu_si512(firedMask, _mm512_castps_si32(maskSpike));
        for (int j = 0; j < 16; ++j) {
            if (firedMask[j] & 0x80000000) {
                fired[i + j] = 1;
                ++numFired;
            } else {
                fired[i + j] = 0;
            }
        }
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
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
                ++numFired;
            } else {
                fired[i] = 0;
            }
        }
    }
    
#elif defined(NLM_SIMD_AVX2)
    __m256 vmRest = _mm256_set1_ps(V_rest);
    __m256 vmReset = _mm256_set1_ps(V_reset);
    __m256 vmThresh = _mm256_set1_ps(V_threshold);
    __m256 vLeakCoeff = _mm256_set1_ps(leakCoeff);
    __m256 vInputCoeff = _mm256_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 vV = _mm256_loadu_ps(&V_inout[i]);
        __m256 vI = _mm256_loadu_ps(&I[i]);
        
        // Refractory handling
        alignas(32) float refractoryFloat[8];
        for (int j = 0; j < 8; ++j) {
            refractoryFloat[j] = static_cast<float>(refractoryRemaining[i + j]);
        }
        __m256 vRefractory = _mm256_load_ps(refractoryFloat);
        __m256 maskRefractory = _mm256_cmp_ps(vRefractory, _mm256_setzero_ps(), _CMP_GT_OQ);
        
        // LIF update
        __m256 vLeak = _mm256_mul_ps(_mm256_sub_ps(vmRest, vV), vLeakCoeff);
        __m256 vInput = _mm256_mul_ps(vI, vInputCoeff);
        __m256 vUpdate = _mm256_mul_ps(_mm256_add_ps(vLeak, vInput), _mm256_set1_ps(1000.0f));
        vV = _mm256_add_ps(vV, vUpdate);
        vV = _mm256_blend_ps(vV, vmRest, maskRefractory);
        
        // Spike detection
        __m256 maskSpike = _mm256_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        
        // Reset and decay
        vV = _mm256_blend_ps(vV, vmReset, maskSpike);
        vRefractory = _mm256_sub_ps(vRefractory, _mm256_set1_ps(1.0f));
        
        _mm256_storeu_ps(&V_inout[i], vV);
        
        alignas(32) float refractoryOut[8];
        _mm256_store_ps(refractoryOut, vRefractory);
        for (int j = 0; j < 8; ++j) {
            refractoryRemaining[i + j] = static_cast<uint32_t>(refractoryOut[j]);
        }
        
        alignas(32) uint32_t firedMask[8];
        _mm256_storeu_si256(firedMask, _mm256_castps_si32(maskSpike));
        for (int j = 0; j < 8; ++j) {
            fired[i + j] = (firedMask[j] & 0x80000000) ? 1 : 0;
            if (fired[i + j]) ++numFired;
        }
    }
    
    // Handle remaining
    for (; i < count; ++i) {
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
                ++numFired;
            } else {
                fired[i] = 0;
            }
        }
    }
    
#elif defined(NLM_SIMD_SSE)
    __m128 vmRest = _mm_set1_ps(V_rest);
    __m128 vmReset = _mm_set1_ps(V_reset);
    __m128 vmThresh = _mm_set1_ps(V_threshold);
    __m128 vLeakCoeff = _mm_set1_ps(leakCoeff);
    __m128 vInputCoeff = _mm_set1_ps(inputCoeff);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 vV = _mm_loadu_ps(&V_inout[i]);
        __m128 vI = _mm_loadu_ps(&I[i]);
        
        alignas(16) float refractoryFloat[4];
        for (int j = 0; j < 4; ++j) {
            refractoryFloat[j] = static_cast<float>(refractoryRemaining[i + j]);
        }
        __m128 vRefractory = _mm_load_ps(refractoryFloat);
        __m128 maskRefractory = _mm_cmpgt_ps(vRefractory, _mm_setzero_ps());
        
        __m128 vLeak = _mm_mul_ps(_mm_sub_ps(vmRest, vV), vLeakCoeff);
        __m128 vInput = _mm_mul_ps(vI, vInputCoeff);
        __m128 vUpdate = _mm_mul_ps(_mm_add_ps(vLeak, vInput), _mm_set1_ps(1000.0f));
        vV = _mm_add_ps(vV, vUpdate);
        vV = _mm_blend_ps(vV, vmRest, maskRefractory);
        
        __m128 maskSpike = _mm_cmpge_ps(vV, vmThresh);
        vV = _mm_blend_ps(vV, vmReset, maskSpike);
        vRefractory = _mm_sub_ps(vRefractory, _mm_set1_ps(1.0f));
        
        _mm_storeu_ps(&V_inout[i], vV);
        
        alignas(16) float refractoryOut[4];
        _mm_store_ps(refractoryOut, vRefractory);
        for (int j = 0; j < 4; ++j) {
            refractoryRemaining[i + j] = static_cast<uint32_t>(refractoryOut[j]);
        }
        
        alignas(16) uint32_t firedMask[4];
        _mm_storeu_si128(firedMask, _mm_castps_si32(maskSpike));
        for (int j = 0; j < 4; ++j) {
            fired[i + j] = (firedMask[j] & 0x80000000) ? 1 : 0;
            if (fired[i + j]) ++numFired;
        }
    }
    
    for (; i < count; ++i) {
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
                ++numFired;
            } else {
                fired[i] = 0;
            }
        }
    }
    
#else
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
                ++numFired;
            } else {
                fired[i] = 0;
            }
        }
    }
#endif
    
    return numFired;
}

} // namespace nlm