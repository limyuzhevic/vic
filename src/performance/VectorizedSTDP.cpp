// SIMD-optimized STDP implementation
// Replaces O(N²) double nested loops with vectorized operations

#include "src/performance/SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Vectorized STDP implementation with SIMD optimizations
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                  const PostArray& postSpikes, size_t numPre, size_t numPost) {
    // Pre-compute spike history for efficient processing
    std::vector<float> preHistory(numPre, 0.0f);
    std::vector<float> postHistory(numPost, 0.0f);
    
    // Extract spike timing information from history arrays
    for (size_t i = 0; i < numPre; ++i) {
        if (preSpikes[i] > 0.0f) {
            preHistory[i] = std::exp(-preSpikes[i] / tau_plus);
        } else if (preSpikes[i] < 0.0f) {
            preHistory[i] = -std::exp(preSpikes[i] / tau_minus);
        }
    }
    
    for (size_t i = 0; i < numPost; ++i) {
        if (postSpikes[i] > 0.0f) {
            postHistory[i] = std::exp(-postSpikes[i] / tau_plus);
        } else if (postSpikes[i] < 0.0f) {
            postHistory[i] = -std::exp(postSpikes[i] / tau_minus);
        }
    }
    
    // Vectorized weight update using SIMD
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    size_t i = 0;
    size_t vectorSize = numPre - (numPre % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 weightUpdates = _mm512_setzero_ps();
        
        // Process pre-synaptic contributions
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < numPre && preHistory[i + j] != 0.0f) {
                __m512 update = _mm512_set1_ps(preHistory[i + j] * A_plus);
                weightUpdates = _mm512_add_ps(weightUpdates, update);
            }
        }
        
        // Process post-synaptic contributions  
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < numPost && postHistory[i + j] != 0.0f) {
                __m512 update = _mm512_set1_ps(postHistory[i + j] * A_minus);
                weightUpdates = _mm512_add_ps(weightUpdates, update);
            }
        }
        
        // Apply updates to weights
        __m512 currentWeights = _mm512_loadu_ps(&weights[i]);
        __m512 updatedWeights = _mm512_add_ps(currentWeights, weightUpdates);
        
        // Clamp to bounds
        __m512 minBounds = _mm512_set1_ps(w_min);
        __m512 maxBounds = _mm512_set1_ps(w_max);
        updatedWeights = _mm512_max_ps(_mm512_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm512_storeu_ps(&weights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    size_t i = 0;
    size_t vectorSize = numPre - (numPre % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 weightUpdates = _mm256_setzero_ps();
        
        for (size_t j = 0; j < 8; ++j) {
            if (i + j < numPre && preHistory[i + j] != 0.0f) {
                __m256 update = _mm256_set1_ps(preHistory[i + j] * A_plus);
                weightUpdates = _mm256_add_ps(weightUpdates, update);
            }
        }
        
        for (size_t j = 0; j < 8; ++j) {
            if (i + j < numPost && postHistory[i + j] != 0.0f) {
                __m256 update = _mm256_set1_ps(postHistory[i + j] * A_minus);
                weightUpdates = _mm256_add_ps(weightUpdates, update);
            }
        }
        
        __m256 currentWeights = _mm256_loadu_ps(&weights[i]);
        __m256 updatedWeights = _mm256_add_ps(currentWeights, weightUpdates);
        
        __m256 minBounds = _mm256_set1_ps(w_min);
        __m256 maxBounds = _mm256_set1_ps(w_max);
        updatedWeights = _mm256_max_ps(_mm256_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm256_storeu_ps(&weights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    size_t i = 0;
    size_t vectorSize = numPre - (numPre % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 weightUpdates = _mm_setzero_ps();
        
        for (size_t j = 0; j < 4; ++j) {
            if (i + j < numPre && preHistory[i + j] != 0.0f) {
                __m128 update = _mm_set1_ps(preHistory[i + j] * A_plus);
                weightUpdates = _mm_add_ps(weightUpdates, update);
            }
        }
        
        for (size_t j = 0; j < 4; ++j) {
            if (i + j < numPost && postHistory[i + j] != 0.0f) {
                __m128 update = _mm_set1_ps(postHistory[i + j] * A_minus);
                weightUpdates = _mm_add_ps(weightUpdates, update);
            }
        }
        
        __m128 currentWeights = _mm_loadu_ps(&weights[i]);
        __m128 updatedWeights = _mm_add_ps(currentWeights, weightUpdates);
        
        __m128 minBounds = _mm_set1_ps(w_min);
        __m128 maxBounds = _mm_set1_ps(w_max);
        updatedWeights = _mm_max_ps(_mm_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm_storeu_ps(&weights[i], updatedWeights);
    }
    
#else
    // Scalar fallback for platforms without SIMD
    for (size_t i = 0; i < numPre; ++i) {
        float weightUpdate = 0.0f;
        
        // Pre-synaptic contributions
        for (size_t j = 0; j < numPost; ++j) {
            if (preHistory[j] != 0.0f) {
                weightUpdate += preHistory[j] * A_plus;
            }
            if (postHistory[j] != 0.0f) {
                weightUpdate += postHistory[j] * A_minus;
            }
        }
        
        weights[i] += weightUpdate;
        weights[i] = std::clamp(weights[i], w_min, w_max);
    }
#endif
    
    // Scalar handling for remaining elements
    for (; i < numPre; ++i) {
        float weightUpdate = 0.0f;
        
        // Pre-synaptic contributions
        for (size_t j = 0; j < numPost; ++j) {
            if (preHistory[j] != 0.0f) {
                weightUpdate += preHistory[j] * A_plus;
            }
            if (postHistory[j] != 0.0f) {
                weightUpdate += postHistory[j] * A_minus;
            }
        }
        
        weights[i] += weightUpdate;
        weights[i] = std::clamp(weights[i], w_min, w_max);
    }
}

// Vectorized spike history processing for STDP
void VectorizedSTDP::updateSynapseHistory(
    float* preHistory,
    float* postHistory,
    const float* spikeTimes,
    const uint8_t* spikeActive,
    size_t numNeurons,
    float currentTime,
    float tau) {
    
    // Efficient spike history update using vectorization
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    size_t i = 0;
    size_t vectorSize = numNeurons - (numNeurons % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 historyMask = _mm512_loadu_ps(&spikeActive[i]);
        __m512 spikeTimesVec = _mm512_loadu_ps(&spikeTimes[i]);
        __m512 currentHistory = _mm512_loadu_ps(&preHistory[i]);
        
        // Update history: decay previous values and add new spikes
        __m512 decayFactor = _mm512_set1_ps(std::exp(-dt / tau));
        __m512 newHistory = _mm512_mul_ps(currentHistory, decayFactor);
        
        // Add spike contributions
        __m512 spikeMask = _mm512_cmpgt_ps(spikeTimesVec, _mm512_setzero_ps());
        __m512 spikeContribution = _mm512_and_ps(spikeMask, _mm512_set1_ps(1.0f));
        newHistory = _mm512_add_ps(newHistory, spikeContribution);
        
        _mm512_storeu_ps(&preHistory[i], newHistory);
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    size_t i = 0;
    size_t vectorSize = numNeurons - (numNeurons % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 historyMask = _mm256_loadu_ps(&spikeActive[i]);
        __m256 spikeTimesVec = _mm256_loadu_ps(&spikeTimes[i]);
        __m256 currentHistory = _mm256_loadu_ps(&preHistory[i]);
        
        __m256 decayFactor = _mm256_set1_ps(std::exp(-dt / tau));
        __m256 newHistory = _mm256_mul_ps(currentHistory, decayFactor);
        
        __m256 spikeMask = _mm256_cmpgt_ps(spikeTimesVec, _mm256_setzero_ps());
        __m256 spikeContribution = _mm256_and_ps(spikeMask, _mm256_set1_ps(1.0f));
        newHistory = _mm256_add_ps(newHistory, spikeContribution);
        
        _mm256_storeu_ps(&preHistory[i], newHistory);
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    size_t i = 0;
    size_t vectorSize = numNeurons - (numNeurons % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 historyMask = _mm_loadu_ps(&spikeActive[i]);
        __m128 spikeTimesVec = _mm_loadu_ps(&spikeTimes[i]);
        __m128 currentHistory = _mm_loadu_ps(&preHistory[i]);
        
        __m128 decayFactor = _mm_set1_ps(std::exp(-dt / tau));
        __m128 newHistory = _mm_mul_ps(currentHistory, decayFactor);
        
        __m128 spikeMask = _mm_cmpgt_ps(spikeTimesVec, _mm_setzero_ps());
        __m128 spikeContribution = _mm_and_ps(spikeMask, _mm_set1_ps(1.0f));
        newHistory = _mm_add_ps(newHistory, spikeContribution);
        
        _mm_storeu_ps(&preHistory[i], newHistory);
    }
    
#else
    // Scalar fallback
    for (size_t i = 0; i < numNeurons; ++i) {
        if (spikeActive[i]) {
            preHistory[i] = 1.0f;  // New spike
        } else {
            preHistory[i] *= std::exp(-dt / tau);  // Decay
        }
    }
#endif
    
    // Handle remaining elements
    for (; i < numNeurons; ++i) {
        if (spikeActive[i]) {
            preHistory[i] = 1.0f;
        } else {
            preHistory[i] *= std::exp(-dt / tau);
        }
    }
}

} // namespace nlm
