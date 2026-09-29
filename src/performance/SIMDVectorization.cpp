#include "SIMDVectorization.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace nlm {

// Vectorized LIF implementation

template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
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
        _mm256_storeu_si256(firedMask, _mm256_castps_si256(maskSpike));
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
        _mm_storeu_si128(firedMask, _mm_castps_si128(maskSpike));
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

// Vectorized Synapse update implementation

template<typename DArray, typename FArray>
void VectorizedSynapse::updateSTD(DArray& depression, FArray& facilitation,
                                 const float* lastPreTime, const float* lastPostTime,
                                 float currentTime, size_t count) {
#if defined(NLM_SIMD_AVX512) || defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
    __m512 vCurrentTime = _mm512_set1_ps(currentTime);
    __m512 vTauFacilitation = _mm512_set1_ps(tau_facilitation);
    __m512 vTauDepression = _mm512_set1_ps(tau_depression);
    __m512 vUMax = _mm512_set1_ps(U_max);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 vLastPre = _mm512_loadu_ps(&lastPreTime[i]);
        __m512 vLastPost = _mm512_loadu_ps(&lastPostTime[i]);
        
        // Calculate time since last pre and post spikes
        __m512 dtPre = _mm512_sub_ps(vCurrentTime, vLastPre);
        __m512 dtPost = _mm512_sub_ps(vCurrentTime, vLastPost);
        
        // Clamp to zero for negative times
        __m512 maskPre = _mm512_cmp_ps(dtPre, _mm512_setzero_ps(), _CMP_GE_OQ);
        __m512 maskPost = _mm512_cmp_ps(dtPost, _mm512_setzero_ps(), _CMP_GE_OQ);
        dtPre = _mm512_blendv_ps(dtPre, _mm512_setzero_ps(), maskPre);
        dtPost = _mm512_blendv_ps(dtPost, _mm512_setzero_ps(), maskPost);
        
        // Update facilitation: F *= exp(-dt/tau_facilitation)
        __m512 vFacilitation = _mm512_loadu_ps(&facilitation[i]);
        __m512 vExpPre = _mm512_exp_ps(_mm512_mul_ps(_mm512_set1_ps(-1.0f), _mm512_div_ps(dtPre, vTauFacilitation)));
        vFacilitation = _mm512_mul_ps(vFacilitation, vExpPre);
        _mm512_storeu_ps(&facilitation[i], vFacilitation);
        
        // Update depression: D += (1 - D) * (1 - exp(-dt/tau_depression))
        __m512 vDepression = _mm512_loadu_ps(&depression[i]);
        __m512 vExpPost = _mm512_exp_ps(_mm512_mul_ps(_mm512_set1_ps(-1.0f), _mm512_div_ps(dtPost, vTauDepression)));
        __m512 vRecovery = _mm512_sub_ps(_mm512_set1_ps(1.0f), vExpPost);
        vDepression = _mm512_add_ps(vDepression, _mm512_mul_ps(_mm512_sub_ps(_mm512_set1_ps(1.0f), vDepression), vRecovery));
        _mm512_storeu_ps(&depression[i], vDepression);
        
        // Clamp depression to [0, U_max]
        vDepression = _mm512_min_ps(vDepression, vUMax);
        vDepression = _mm512_max_ps(vDepression, _mm512_setzero_ps());
        _mm512_storeu_ps(&depression[i], vDepression);
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        float dtPre = std::max(0.0f, currentTime - lastPreTime[i]);
        float dtPost = std::max(0.0f, currentTime - lastPostTime[i]);
        
        // Update facilitation
        float& f = facilitation[i];
        f *= std::exp(-dtPre / tau_facilitation);
        
        // Update depression
        float& d = depression[i];
        d += (1.0f - d) * (1.0f - std::exp(-dtPost / tau_depression));
        
        // Clamp
        d = std::clamp(d, 0.0f, U_max);
    }

#else
    // Scalar fallback
    for (size_t i = 0; i < count; ++i) {
        float dtPre = std::max(0.0f, currentTime - lastPreTime[i]);
        float dtPost = std::max(0.0f, currentTime - lastPostTime[i]);
        
        // Update facilitation
        facilitation[i] *= std::exp(-dtPre / tau_facilitation);
        
        // Update depression
        depression[i] += (1.0f - depression[i]) * (1.0f - std::exp(-dtPost / tau_depression));
        
        // Clamp
        depression[i] = std::clamp(depression[i], 0.0f, U_max);
    }
#endif
}

// Vectorized STDP implementation

template<typename WArray, typename PreArray, typename PostArray>
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                  const PostArray& postSpikes, size_t numPre, size_t numPost) {
#if defined(NLM_SIMD_AVX512) || defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
    // For each pre-post pair, calculate weight change
    for (size_t preIdx = 0; preIdx < numPre; ++preIdx) {
        float preTime = preSpikes[preIdx];
        for (size_t postIdx = 0; postIdx < numPost; ++postIdx) {
            float postTime = postSpikes[postIdx];
            float dt = preTime - postTime;
            
            float deltaW = calculateDeltaW(dt);
            weights[preIdx * numPost + postIdx] += deltaW;
            
            // Clamp weight
            weights[preIdx * numPost + postIdx] = std::clamp(weights[preIdx * numPost + postIdx], w_min, w_max);
        }
    }

#else
    // Scalar fallback
    for (size_t preIdx = 0; preIdx < numPre; ++preIdx) {
        float preTime = preSpikes[preIdx];
        for (size_t postIdx = 0; postIdx < numPost; ++postIdx) {
            float postTime = postSpikes[postIdx];
            float dt = preTime - postTime;
            
            float deltaW = calculateDeltaW(dt);
            weights[preIdx * numPost + postIdx] += deltaW;
            
            // Clamp weight
            weights[preIdx * numPost + postIdx] = std::clamp(weights[preIdx * numPost + postIdx], w_min, w_max);
        }
    }
#endif
}

// SIMD statistics implementation

template<typename Array>
float VectorizedStats::sum(const Array& arr, size_t count) {
#if defined(NLM_SIMD_AVX512) || defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
    float result = 0.0f;
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    __m512 vSum = _mm512_setzero_ps();
    
    for (; i < vectorSize; i += 16) {
        __m512 v = _mm512_loadu_ps(&arr[i]);
        vSum = _mm512_add_ps(vSum, v);
    }
    
    // Horizontal sum
    alignas(64) float partial[16];
    _mm512_store_ps(partial, vSum);
    for (int j = 0; j < 16; ++j) {
        result += partial[j];
    }
    
    // Handle remaining
    for (; i < count; ++i) {
        result += arr[i];
    }
    
    return result;

#else
    // Scalar fallback
    float result = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        result += arr[i];
    }
    return result;
#endif
}

// Test SIMD implementation
void testSIMD() {
    std::cout << "SIMD Vectorization Test" << std::endl;
    std::cout << "  SIMD Type: " << SIMDUtils::getSIMDType() << std::endl;
    std::cout << "  SIMD Available: " << (SIMDUtils::isSIMDAvailable() ? "Yes" : "No") << std::endl;
    std::cout << "  SIMD Width: " << SIMDUtils::SIMD_WIDTH << std::endl;
    
    VectorizedLIF lif;
    VectorizedSynapse syn;
    VectorizedSTDP stdp;
    
    std::cout << "  LIF parameters: V_rest=-70.0, V_reset=-70.0, V_threshold=-55.0" << std::endl;
    std::cout << "  Synapse parameters: tau_facilitation=100.0, tau_depression=200.0, U_max=1.0" << std::endl;
    std::cout << "  STDP parameters: tau_plus=20.0, tau_minus=20.0, A_plus=0.01, A_minus=0.012" << std::endl;
}

} // namespace nlm
