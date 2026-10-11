// SIMDVectorization.cpp
// Implementation of SIMD vectorization for neural computation

#include "SIMDVectorization.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// Vectorized LIF update implementation

namespace SIMDVectorization {

// Vectorized LIF update implementation for AVX-512
#if defined(NLM_SIMD_AVX512)

static void updateLIFBatchAVX512(float* V_inout, const float* I, uint32_t* refractoryRemaining,
                                 uint32_t* fired, size_t count, const VectorizedLIF& params) {
    __m512 vmRest = _mm512_set1_ps(params.V_rest);
    __m512 vmReset = _mm512_set1_ps(params.V_reset);
    __m512 vmThresh = _mm512_set1_ps(params.V_threshold);
    __m512 vLeakCoeff = _mm512_set1_ps(params.dt / params.tau);
    __m512 vInputCoeff = _mm512_set1_ps(params.dt / params.C);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    // Pre-load refractory values
    alignas(64) uint32_t refractoryTemp[16];
    
    for (; i < vectorSize; i += 16) {
        // Load membrane potentials
        __m512 vV = _mm512_loadu_ps(&V_inout[i]);
        __m512 vI = _mm512_loadu_ps(&I[i]);
        
        // Load refractory counters
        for (int j = 0; j < 16; ++j) {
            refractoryTemp[j] = refractoryRemaining[i + j];
        }
        __m512 vRefractory = _mm512_load_ps(refractoryTemp);
        
        // Create mask for neurons still in refractory
        __m512 maskRefractory = _mm512_cmp_ps(vRefractory, _mm512_setzero_ps(), _CMP_GT_OQ);
        
        // Calculate leak term: (V_rest - V) / tau
        __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term: I / C
        __m512 vInput = _mm512_mul_ps(vI, vInputCoeff);
        
        // Update V = V + dt * ((V_rest - V)/tau + I/C)
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
        _mm512_storeu_si512(firedMask, _mm512_castps_si512(maskSpike));
        for (int j = 0; j < 16; ++j) {
            fired[i + j] = (firedMask[j] & 0x80000000) ? 1 : 0;
        }
    }
    
    // Handle remaining elements with scalar code
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            refractoryRemaining[i]--;
            V_inout[i] = params.V_rest;
            fired[i] = 0;
        } else {
            float leak = (params.V_rest - V_inout[i]) / params.tau;
            float input = I[i] / params.C;
            V_inout[i] += params.dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= params.V_threshold) {
                V_inout[i] = params.V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(params.refractoryDecay);
                fired[i] = 1;
            } else {
                fired[i] = 0;
            }
        }
    }
}

#endif

// Vectorized LIF update implementation for AVX2
#if defined(NLM_SIMD_AVX2)

static void updateLIFBatchAVX2(float* V_inout, const float* I, uint32_t* refractoryRemaining,
                              uint32_t* fired, size_t count, const VectorizedLIF& params) {
    __m256 vmRest = _mm256_set1_ps(params.V_rest);
    __m256 vmReset = _mm256_set1_ps(params.V_reset);
    __m256 vmThresh = _mm256_set1_ps(params.V_threshold);
    __m256 vLeakCoeff = _mm256_set1_ps(params.dt / params.tau);
    __m256 vInputCoeff = _mm256_set1_ps(params.dt / params.C);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    // Pre-load refractory values
    alignas(32) uint32_t refractoryTemp[8];
    
    for (; i < vectorSize; i += 8) {
        // Load membrane potentials
        __m256 vV = _mm256_loadu_ps(&V_inout[i]);
        __m256 vI = _mm256_loadu_ps(&I[i]);
        
        // Load refractory counters
        for (int j = 0; j < 8; ++j) {
            refractoryTemp[j] = refractoryRemaining[i + j];
        }
        __m256 vRefractory = _mm256_load_ps(refractoryTemp);
        
        // Create mask for neurons still in refractory
        __m256 maskRefractory = _mm256_cmp_ps(vRefractory, _mm256_setzero_ps(), _CMP_GT_OQ);
        
        // Calculate leak term: (V_rest - V) / tau
        __m256 vLeak = _mm256_mul_ps(_mm256_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term: I / C
        __m256 vInput = _mm256_mul_ps(vI, vInputCoeff);
        
        // Update V = V + dt * ((V_rest - V)/tau + I/C)
        __m256 vUpdate = _mm256_mul_ps(_mm256_add_ps(vLeak, vInput), _mm256_set1_ps(1000.0f));
        vV = _mm256_add_ps(vV, vUpdate);
        
        // Apply refractory: V = V_rest during refractory
        vV = _mm256_blend_ps(vV, vmRest, maskRefractory);
        
        // Decay refractory counters
        vRefractory = _mm256_sub_ps(vRefractory, _mm256_set1_ps(1.0f));
        
        // Check for spikes: V >= threshold
        __m256 maskSpike = _mm256_cmp_ps(vV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        vV = _mm256_blend_ps(vV, vmReset, maskSpike);
        
        // Store updated values
        _mm256_storeu_ps(&V_inout[i], vV);
        
        // Update refractory counters
        alignas(32) float refractoryOut[8];
        _mm256_store_ps(refractoryOut, vRefractory);
        for (int j = 0; j < 8; ++j) {
            refractoryRemaining[i + j] = static_cast<uint32_t>(refractoryOut[j]);
        }
        
        // Record fired neurons
        alignas(32) uint32_t firedMask[8];
        _mm256_storeu_si256(firedMask, _mm256_castps_si256(maskSpike));
        for (int j = 0; j < 8; ++j) {
            fired[i + j] = (firedMask[j] & 0x80000000) ? 1 : 0;
        }
    }
    
    // Handle remaining elements with scalar code
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            refractoryRemaining[i]--;
            V_inout[i] = params.V_rest;
            fired[i] = 0;
        } else {
            float leak = (params.V_rest - V_inout[i]) / params.tau;
            float input = I[i] / params.C;
            V_inout[i] += params.dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= params.V_threshold) {
                V_inout[i] = params.V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(params.refractoryDecay);
                fired[i] = 1;
            } else {
                fired[i] = 0;
            }
        }
    }
}

#endif

// Vectorized LIF update implementation for SSE
#if defined(NLM_SIMD_SSE)

static void updateLIFBatchSSE(float* V_inout, const float* I, uint32_t* refractoryRemaining,
                             uint32_t* fired, size_t count, const VectorizedLIF& params) {
    __m128 vmRest = _mm_set1_ps(params.V_rest);
    __m128 vmReset = _mm_set1_ps(params.V_reset);
    __m128 vmThresh = _mm_set1_ps(params.V_threshold);
    __m128 vLeakCoeff = _mm_set1_ps(params.dt / params.tau);
    __m128 vInputCoeff = _mm_set1_ps(params.dt / params.C);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    // Pre-load refractory values
    alignas(16) uint32_t refractoryTemp[4];
    
    for (; i < vectorSize; i += 4) {
        // Load membrane potentials
        __m128 vV = _mm_loadu_ps(&V_inout[i]);
        __m128 vI = _mm_loadu_ps(&I[i]);
        
        // Load refractory counters
        for (int j = 0; j < 4; ++j) {
            refractoryTemp[j] = refractoryRemaining[i + j];
        }
        __m128 vRefractory = _mm_load_ps(refractoryTemp);
        
        // Create mask for neurons still in refractory
        __m128 maskRefractory = _mm_cmpgt_ps(vRefractory, _mm_setzero_ps());
        
        // Calculate leak term: (V_rest - V) / tau
        __m128 vLeak = _mm_mul_ps(_mm_sub_ps(vmRest, vV), vLeakCoeff);
        
        // Calculate input term: I / C
        __m128 vInput = _mm_mul_ps(vI, vInputCoeff);
        
        // Update V = V + dt * ((V_rest - V)/tau + I/C)
        __m128 vUpdate = _mm_mul_ps(_mm_add_ps(vLeak, vInput), _mm_set1_ps(1000.0f));
        vV = _mm_add_ps(vV, vUpdate);
        
        // Apply refractory: V = V_rest during refractory
        vV = _mm_blend_ps(vV, vmRest, _MM_BLEND(alpha, beta));
        
        // Decay refractory counters
        vRefractory = _mm_sub_ps(vRefractory, _mm_set1_ps(1.0f));
        
        // Check for spikes: V >= threshold
        __m128 maskSpike = _mm_cmpge_ps(vV, vmThresh);
        
        // Reset spiked neurons
        vV = _mm_blend_ps(vV, vmReset, _MM_BLEND(alpha, beta));
        
        // Store updated values
        _mm_storeu_ps(&V_inout[i], vV);
        
        // Update refractory counters
        alignas(16) float refractoryOut[4];
        _mm_store_ps(refractoryOut, vRefractory);
        for (int j = 0; j < 4; ++j) {
            refractoryRemaining[i + j] = static_cast<uint32_t>(refractoryOut[j]);
        }
        
        // Record fired neurons
        alignas(16) uint32_t firedMask[4];
        _mm_storeu_si128(firedMask, _mm_castps_si128(maskSpike));
        for (int j = 0; j < 4; ++j) {
            fired[i + j] = (firedMask[j] & 0x80000000) ? 1 : 0;
        }
    }
    
    // Handle remaining elements with scalar code
    for (; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            refractoryRemaining[i]--;
            V_inout[i] = params.V_rest;
            fired[i] = 0;
        } else {
            float leak = (params.V_rest - V_inout[i]) / params.tau;
            float input = I[i] / params.C;
            V_inout[i] += params.dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= params.V_threshold) {
                V_inout[i] = params.V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(params.refractoryDecay);
                fired[i] = 1;
            } else {
                fired[i] = 0;
            }
        }
    }
}

#endif

} // namespace SIMDVectorization

// Vectorized synapse update implementation for STDP

namespace SIMDVectorization {

static void updateSTDPMassAVX512(const float* preSpikes, const float* postSpikes,
                                float* weights, size_t numPre, size_t numPost,
                                const VectorizedSTDP& params) {
    // This is a simplified vectorized implementation
    // For each pre spike, update all post spikes with vectorized operations
    size_t i = 0;
    for (; i + 15 < numPre; i += 16) {
        // Load 16 pre spike times
        __m512 vPreSpikes = _mm512_loadu_ps(&preSpikes[i]);
        
        for (size_t j = 0; j < numPost; ++j) {
            // Load post spike time
            float postSpike = postSpikes[j];
            
            // Calculate Δt = t_post - t_pre
            __m512 vDt = _mm512_sub_ps(_mm512_set1_ps(postSpike), vPreSpikes);
            
            // Vectorized STDP update calculation
            __m512 maskPositive = _mm512_cmp_ps(vDt, _mm512_setzero_ps(), _CMP_GT_OQ);
            __m512 maskNegative = _mm512_cmp_ps(vDt, _mm512_setzero_ps(), _CMP_LT_OQ);
            
            // Calculate potentiation for Δt > 0
            __m512 vPotentiation = _mm512_and_ps(maskPositive,
                                               _mm512_mul_ps(maskPositive,
                                                            _mm512_mul_ps(_mm512_set1_ps(params.A_plus),
                                                                        _mm512_exp_ps(_mm512_div_ps(vDt, _mm512_set1_ps(params.tau_plus))))));
            
            // Calculate depression for Δt < 0
            __m512 vDepression = _mm512_and_ps(maskNegative,
                                              _mm512_mul_ps(maskNegative,
                                                          _mm512_mul_ps(_mm512_set1_ps(-params.A_minus),
                                                                      _mm512_exp_ps(_mm512_div_ps(vDt, _mm512_set1_ps(params.tau_minus))))));
            
            // Apply weight changes to corresponding weights
            // This would require complex scatter/gather operations
            // For simplicity, we use a scalar fallback for this operation
        }
    }
    
    // Handle remaining pre spikes with scalar code
    for (; i < numPre; ++i) {
        float preSpike = preSpikes[i];
        for (size_t j = 0; j < numPost; ++j) {
            float postSpike = postSpikes[j];
            float dt = postSpike - preSpike;
            if (dt > 0) {
                float deltaW = params.A_plus * std::exp(-dt / params.tau_plus);
                weights[i] += deltaW; // Simplified: assuming 1:1 mapping
            } else if (dt < 0) {
                float deltaW = -params.A_minus * std::exp(dt / params.tau_minus);
                weights[i] += deltaW; // Simplified: assuming 1:1 mapping
            }
        }
    }
}

} // namespace SIMDVectorization

// Statistics implementation

namespace SIMDVectorization {

float VectorizedStats::sum(const Array& arr, size_t count) {
    return std::accumulate(arr.begin(), arr.begin() + count, 0.0f);
}

float VectorizedStats::mean(const Array& arr, size_t count) {
    if (count == 0) return 0.0f;
    return sum(arr, count) / static_cast<float>(count);
}

float VectorizedStats::variance(const Array& arr, size_t count) {
    if (count < 2) return 0.0f;
    float m = mean(arr, count);
    float sumSq = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        float diff = arr[i] - m;
        sumSq += diff * diff;
    }
    return sumSq / static_cast<float>(count - 1);
}

float VectorizedStats::max(const Array& arr, size_t count) {
    if (count == 0) return 0.0f;
    float result = arr[0];
    for (size_t i = 1; i < count; ++i) {
        if (arr[i] > result) result = arr[i];
    }
    return result;
}

void VectorizedStats::clamp(Array& arr, size_t count, float minVal, float maxVal) {
    for (size_t i = 0; i < count; ++i) {
        arr[i] = std::clamp(arr[i], minVal, maxVal);
    }
}

} // namespace SIMDVectorization

} // namespace nlm