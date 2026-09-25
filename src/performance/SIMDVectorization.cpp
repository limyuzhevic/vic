// Optimized implementations for neural simulation performance
// Provides SIMD vectorized implementations for critical performance bottlenecks

#include "src/performance/SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

/**
 * VectorizedLIF implementation with SIMD optimizations
 */
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                           FArray& fired, size_t count) {
    size_t numFired = 0;
    
    // Pre-compute constants for efficiency
    float leakCoeff = dt / tau;
    float inputCoeff = dt / C;
    float expLeak = std::exp(-dt / tau);
    
    // SIMD vectorized implementation with multiple optimization levels
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    // AVX-512 implementation (16-wide)
    alignas(64) float vmRestArr[16] = {V_rest};
    alignas(64) float vmResetArr[16] = {V_reset};
    alignas(64) float vmThreshArr[16] = {V_threshold};
    alignas(64) float vLeakCoeffArr[16] = {leakCoeff};
    alignas(64) float vInputCoeffArr[16] = {inputCoeff};
    alignas(64) float vExpLeakArr[16] = {expLeak};
    
    __m512 vmRest = _mm512_load_ps(vmRestArr);
    __m512 vmReset = _mm512_load_ps(vmResetArr);
    __m512 vmThresh = _mm512_load_ps(vmThreshArr);
    __m512 vLeakCoeff = _mm512_load_ps(vLeakCoeffArr);
    __m512 vInputCoeff = _mm512_load_ps(vInputCoeffArr);
    __m512 vExpLeak = _mm512_load_ps(vExpLeakArr);
    
    __m512 vOne = _mm512_set1_ps(1.0f);
    __m512 vZero = _mm512_setzero_ps();
    __m512 vThousand = _mm512_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    // Main SIMD loop for 16-element batches
    for (; i < vectorSize; i += 16) {
        // Load neuron states
        __m512 vV = _mm512_loadu_ps(&V_inout[i]);
        __m512 vI = _mm512_loadu_ps(&I[i]);
        
        // Load refractory counters and convert to float
        alignas(64) uint32_t refractoryInt[16];
        for (int j = 0; j < 16; ++j) {
            refractoryInt[j] = refractoryRemaining[i + j];
        }
        __m512 vRefractory = _mm512_cvtepi32_ps(_mm512_loadu_si512(refractoryInt));
        
        // Create mask for neurons in refractory state
        __m512 maskRefractory = _mm512_cmpgt_ps(vRefractory, vZero);
        
        // Apply exponential leak for neurons not in refractory
        __m512 maskNotRefractory = _mm512_cmpeq_ps(maskRefractory, vZero);
        __m512 vVLeaked = _mm512_add_ps(vV, _mm512_mul_ps(_mm512_mul_ps(vmRest, vExpLeak), maskNotRefractory));
        
        // Calculate synaptic input component
        __m512 vInput = _mm512_mul_ps(vI, vInputCoeff);
        
        // Total membrane potential update
        __m512 vVNew = _mm512_add_ps(vVLeaked, vInput);
        
        // Apply refractory: neurons in refractory stay at rest
        vVNew = _mm512_blendv_ps(vVNew, vmRest, maskRefractory);
        
        // Scale by 1000 for ms conversion
        vVNew = _mm512_mul_ps(vVNew, vThousand);
        
        // Detect spikes
        __m512 maskSpike = _mm512_cmpge_ps(vVNew, vmThresh);
        
        // Reset spiked neurons
        __m512 vVReset = _mm512_blendv_ps(vVNew, vmReset, maskSpike);
        
        // Store updated potentials
        _mm512_storeu_ps(&V_inout[i], vVReset);
        
        // Update refractory counters
        __m512 vRefractoryNew = _mm512_sub_ps(vRefractory, vOne);
        alignas(64) uint32_t refractoryOut[16];
        _mm512_storeu_si512(refractoryOut, _mm512_cvttps_epi32(vRefractoryNew));
        for (int j = 0; j < 16; ++j) {
            refractoryRemaining[i + j] = refractoryOut[j];
        }
        
        // Record spike events and count
        alignas(64) uint32_t spikeMask[16];
        _mm512_storeu_si512(spikeMask, _mm512_castps_si512(maskSpike));
        for (int j = 0; j < 16; ++j) {
            if ((spikeMask[j] & 0x80000000) != 0) {
                fired[i + j] = 1;
                ++numFired;
            } else {
                fired[i + j] = 0;
            }
        }
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    // AVX2 implementation (8-wide)
    alignas(32) float vmRestArr[8] = {V_rest};
    alignas(32) float vmResetArr[8] = {V_reset};
    alignas(32) float vmThreshArr[8] = {V_threshold};
    alignas(32) float vLeakCoeffArr[8] = {leakCoeff};
    alignas(32) float vInputCoeffArr[8] = {inputCoeff};
    alignas(32) float vExpLeakArr[8] = {expLeak};
    
    __m256 vmRest = _mm256_load_ps(vmRestArr);
    __m256 vmReset = _mm256_load_ps(vmResetArr);
    __m256 vmThresh = _mm256_load_ps(vmThreshArr);
    __m256 vLeakCoeff = _mm256_load_ps(vLeakCoeffArr);
    __m256 vInputCoeff = _mm256_load_ps(vInputCoeffArr);
    __m256 vExpLeak = _mm256_load_ps(vExpLeakArr);
    
    __m256 vOne = _mm256_set1_ps(1.0f);
    __m256 vZero = _mm256_setzero_ps();
    __m256 vThousand = _mm256_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 vV = _mm256_loadu_ps(&V_inout[i]);
        __m256 vI = _mm256_loadu_ps(&I[i]);
        
        alignas(32) uint32_t refractoryInt[8];
        for (int j = 0; j < 8; ++j) {
            refractoryInt[j] = refractoryRemaining[i + j];
        }
        __m256 vRefractory = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(refractoryInt)));
        
        __m256 maskRefractory = _mm256_cmpgt_ps(vRefractory, vZero);
        __m256 maskNotRefractory = _mm256_cmpeq_ps(maskRefractory, vZero);
        
        __m256 vVLeaked = _mm256_add_ps(vV, _mm256_mul_ps(_mm256_mul_ps(vmRest, vExpLeak), maskNotRefractory));
        
        __m256 vInput = _mm256_mul_ps(vI, vInputCoeff);
        __m256 vVNew = _mm256_add_ps(vVLeaked, vInput);
        
        vVNew = _mm256_blendv_ps(vVNew, vmRest, maskRefractory);
        vVNew = _mm256_mul_ps(vVNew, vThousand);
        
        __m256 maskSpike = _mm256_cmpge_ps(vVNew, vmThresh);
        __m256 vVReset = _mm256_blendv_ps(vVNew, vmReset, maskSpike);
        
        _mm256_storeu_ps(&V_inout[i], vVReset);
        
        __m256 vRefractoryNew = _mm256_sub_ps(vRefractory, vOne);
        alignas(32) uint32_t refractoryOut[8];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(refractoryOut), _mm256_cvttps_epi32(vRefractoryNew));
        for (int j = 0; j < 8; ++j) {
            refractoryRemaining[i + j] = refractoryOut[j];
        }
        
        alignas(32) uint32_t spikeMask[8];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(spikeMask), _mm256_castps_si256(maskSpike));
        for (int j = 0; j < 8; ++j) {
            if ((spikeMask[j] & 0x80000000) != 0) {
                fired[i + j] = 1;
                ++numFired;
            } else {
                fired[i + j] = 0;
            }
        }
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    // SSE implementation (4-wide)
    alignas(16) float vmRestArr[4] = {V_rest};
    alignas(16) float vmResetArr[4] = {V_reset};
    alignas(16) float vmThreshArr[4] = {V_threshold};
    alignas(16) float vLeakCoeffArr[4] = {leakCoeff};
    alignas(16) float vInputCoeffArr[4] = {inputCoeff};
    alignas(16) float vExpLeakArr[4] = {expLeak};
    
    __m128 vmRest = _mm_load_ps(vmRestArr);
    __m128 vmReset = _mm_load_ps(vmResetArr);
    __m128 vmThresh = _mm_load_ps(vmThreshArr);
    __m128 vLeakCoeff = _mm_load_ps(vLeakCoeffArr);
    __m128 vInputCoeff = _mm_load_ps(vInputCoeffArr);
    __m128 vExpLeak = _mm_load_ps(vExpLeakArr);
    
    __m128 vOne = _mm_set1_ps(1.0f);
    __m128 vZero = _mm_setzero_ps();
    __m128 vThousand = _mm_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 vV = _mm_loadu_ps(&V_inout[i]);
        __m128 vI = _mm_loadu_ps(&I[i]);
        
        alignas(16) uint32_t refractoryInt[4];
        for (int j = 0; j < 4; ++j) {
            refractoryInt[j] = refractoryRemaining[i + j];
        }
        __m128 vRefractory = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const __m128i*>(refractoryInt)));
        
        __m128 maskRefractory = _mm_cmpgt_ps(vRefractory, vZero);
        __m128 maskNotRefractory = _mm_cmpeq_ps(maskRefractory, vZero);
        
        __m128 vVLeaked = _mm_add_ps(vV, _mm_mul_ps(_mm_mul_ps(vmRest, vExpLeak), maskNotRefractory));
        
        __m128 vInput = _mm_mul_ps(vI, vInputCoeff);
        __m128 vVNew = _mm_add_ps(vVLeaked, vInput);
        
        vVNew = _mm_blendv_ps(vVNew, vmRest, maskRefractory);
        vVNew = _mm_mul_ps(vVNew, vThousand);
        
        __m128 maskSpike = _mm_cmpge_ps(vVNew, vmThresh);
        __m128 vVReset = _mm_blendv_ps(vVNew, vmReset, maskSpike);
        
        _mm_storeu_ps(&V_inout[i], vVReset);
        
        __m128 vRefractoryNew = _mm_sub_ps(vRefractory, vOne);
        alignas(16) uint32_t refractoryOut[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(refractoryOut), _mm_cvttps_epi32(vRefractoryNew));
        for (int j = 0; j < 4; ++j) {
            refractoryRemaining[i + j] = refractoryOut[j];
        }
        
        alignas(16) uint32_t spikeMask[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(spikeMask), _mm_castps_si128(maskSpike));
        for (int j = 0; j < 4; ++j) {
            if ((spikeMask[j] & 0x80000000) != 0) {
                fired[i + j] = 1;
                ++numFired;
            } else {
                fired[i + j] = 0;
            }
        }
    }
    
#else
    // Scalar fallback implementation
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

/**
 * Vectorized STDP weight update implementation
 */
template<typename WArray, typename PreArray, typename PostArray>
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                  const PostArray& postSpikes, size_t numPre, size_t numPost) {
    // Pre-compute exponential terms
    alignas(64) float preExpTerms[16];
    alignas(64) float postExpTerms[16];
    
    size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    size_t vectorSize = numPre - (numPre % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 preSum = _mm512_setzero_ps();
        __m512 postSum = _mm512_setzero_ps();
        
        for (size_t j = 0; j < 16 && i + j < numPost; ++j) {
            float dt = postSpikes[i + j];
            if (dt > 0.0f) {
                preExpTerms[j] = A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                postExpTerms[j] = -A_minus * std::exp(dt / tau_minus);
            } else {
                preExpTerms[j] = postExpTerms[j] = 0.0f;
            }
        }
        
        // Accumulate contributions
        for (size_t j = 0; j < 16 && i + j < numPre; ++j) {
            float dt = preSpikes[i + j];
            if (dt > 0.0f) {
                __m512 weightUpdate = _mm512_set1_ps(preExpTerms[j]);
                preSum = _mm512_add_ps(preSum, weightUpdate);
            } else if (dt < 0.0f) {
                __m512 weightUpdate = _mm512_set1_ps(postExpTerms[j]);
                postSum = _mm512_add_ps(postSum, weightUpdate);
            }
        }
        
        // Apply weight updates
        __m512 currentWeights = _mm512_loadu_ps(&weights[i]);
        __m512 updatedWeights = _mm512_add_ps(currentWeights, _mm512_add_ps(preSum, postSum));
        
        // Clamp to bounds
        __m512 minWeights = _mm512_set1_ps(w_min);
        __m512 maxWeights = _mm512_set1_ps(w_max);
        updatedWeights = _mm512_max_ps(_mm512_min_ps(updatedWeights, maxWeights), minWeights);
        
        _mm512_storeu_ps(&weights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    size_t vectorSize = numPre - (numPre % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 preSum = _mm256_setzero_ps();
        __m256 postSum = _mm256_setzero_ps();
        
        for (size_t j = 0; j < 8 && i + j < numPost; ++j) {
            float dt = postSpikes[i + j];
            if (dt > 0.0f) {
                preExpTerms[j] = A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                postExpTerms[j] = -A_minus * std::exp(dt / tau_minus);
            } else {
                preExpTerms[j] = postExpTerms[j] = 0.0f;
            }
        }
        
        for (size_t j = 0; j < 8 && i + j < numPre; ++j) {
            float dt = preSpikes[i + j];
            if (dt > 0.0f) {
                __m256 weightUpdate = _mm256_set1_ps(preExpTerms[j]);
                preSum = _mm256_add_ps(preSum, weightUpdate);
            } else if (dt < 0.0f) {
                __m256 weightUpdate = _mm256_set1_ps(postExpTerms[j]);
                postSum = _mm256_add_ps(postSum, weightUpdate);
            }
        }
        
        __m256 currentWeights = _mm256_loadu_ps(&weights[i]);
        __m256 updatedWeights = _mm256_add_ps(currentWeights, _mm256_add_ps(preSum, postSum));
        
        __m256 minWeights = _mm256_set1_ps(w_min);
        __m256 maxWeights = _mm256_set1_ps(w_max);
        updatedWeights = _mm256_max_ps(_mm256_min_ps(updatedWeights, maxWeights), minWeights);
        
        _mm256_storeu_ps(&weights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    size_t vectorSize = numPre - (numPre % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 preSum = _mm_setzero_ps();
        __m128 postSum = _mm_setzero_ps();
        
        for (size_t j = 0; j < 4 && i + j < numPost; ++j) {
            float dt = postSpikes[i + j];
            if (dt > 0.0f) {
                preExpTerms[j] = A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                postExpTerms[j] = -A_minus * std::exp(dt / tau_minus);
            } else {
                preExpTerms[j] = postExpTerms[j] = 0.0f;
            }
        }
        
        for (size_t j = 0; j < 4 && i + j < numPre; ++j) {
            float dt = preSpikes[i + j];
            if (dt > 0.0f) {
                __m128 weightUpdate = _mm_set1_ps(preExpTerms[j]);
                preSum = _mm_add_ps(preSum, weightUpdate);
            } else if (dt < 0.0f) {
                __m128 weightUpdate = _mm_set1_ps(postExpTerms[j]);
                postSum = _mm_add_ps(postSum, weightUpdate);
            }
        }
        
        __m128 currentWeights = _mm_loadu_ps(&weights[i]);
        __m128 updatedWeights = _mm_add_ps(currentWeights, _mm_add_ps(preSum, postSum));
        
        __m128 minWeights = _mm_set1_ps(w_min);
        __m128 maxWeights = _mm_set1_ps(w_max);
        updatedWeights = _mm_max_ps(_mm_min_ps(updatedWeights, maxWeights), minWeights);
        
        _mm_storeu_ps(&weights[i], updatedWeights);
    }
    
#else
    // Scalar fallback
    for (size_t i = 0; i < numPre; ++i) {
        float preSum = 0.0f;
        float postSum = 0.0f;
        
        for (size_t j = 0; j < numPost; ++j) {
            float dt = postSpikes[j];
            if (dt > 0.0f) {
                preSum += A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                postSum += -A_minus * std::exp(dt / tau_minus);
            }
        }
        
        float updatedWeight = weights[i] + preSum + postSum;
        weights[i] = std::clamp(updatedWeight, w_min, w_max);
    }
#endif
    
    // Handle remaining elements with scalar code
    for (; i < numPre; ++i) {
        float preSum = 0.0f;
        float postSum = 0.0f;
        
        for (size_t j = 0; j < numPost; ++j) {
            float dt = postSpikes[j];
            if (dt > 0.0f) {
                preSum += A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                postSum += -A_minus * std::exp(dt / tau_minus);
            }
        }
        
        float updatedWeight = weights[i] + preSum + postSum;
        weights[i] = std::clamp(updatedWeight, w_min, w_max);
    }
}

} // namespace nlm
