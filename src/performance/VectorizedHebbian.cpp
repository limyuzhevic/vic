// SIMD-optimized Hebbian learning implementation
// Replaces O(N²) correlation counting with vectorized operations

#include "src/performance/SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Optimized Hebbian learning using vectorized correlation computation
void VectorizedHebbian::updateWeights(
    float* synapseWeights,
    const float* preSpikeHistory,
    const float* postSpikeHistory,
    const uint32_t* preSpikeTimes,
    const uint32_t* postSpikeTimes,
    const uint8_t* plasticityFlags,
    size_t synapseCount,
    size_t maxHistorySize,
    float learningRate,
    float correlationWindow) {
    
    // Pre-compute spike correlations using efficient algorithms
    std::vector<float> correlations(synapseCount, 0.0f);
    
    // Vectorized correlation computation
    size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    size_t vectorSize = synapseCount - (synapseCount % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 preSum = _mm512_setzero_ps();
        __m512 postSum = _mm512_setzero_ps();
        __m512 productSum = _mm512_setzero_ps();
        
        // Extract spike histories for this batch
        alignas(64) float preHistory[16];
        alignas(64) float postHistory[16];
        for (size_t j = 0; j < 16; ++j) {
            preHistory[j] = (i + j < maxHistorySize) ? preSpikeHistory[i + j] : 0.0f;
            postHistory[j] = (i + j < maxHistorySize) ? postSpikeHistory[i + j] : 0.0f;
        }
        
        // Compute correlation terms using SIMD
        for (size_t j = 0; j < 16; ++j) {
            if (plasticityFlags[i + j]) {
                float preCorr = preHistory[j];
                float postCorr = postHistory[j];
                
                // Simplified covariance calculation: ⟨pre·post⟩ - ⟨pre⟩⟨post⟩
                // Using efficient vector operations
                __m512 preVal = _mm512_set1_ps(preCorr);
                __m512 postVal = _mm512_set1_ps(postCorr);
                
                // Accumulate correlations for this synapse
                preSum = _mm512_add_ps(preSum, preVal);
                postSum = _mm512_add_ps(postSum, postVal);
                
                // Pairwise product for coactivity
                __m512 product = _mm512_mul_ps(preVal, postVal);
                productSum = _mm512_add_ps(productSum, product);
            }
        }
        
        // Calculate final correlation: learningRate * (⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
        __m512 correlation = _mm512_mul_ps(learningRate, 
            _mm512_sub_ps(productSum, _mm512_mul_ps(preSum, postSum)));
        
        // Apply correlation to synapse weights
        __m512 currentWeights = _mm512_loadu_ps(&synapseWeights[i]);
        __m512 updatedWeights = _mm512_add_ps(currentWeights, correlation);
        
        // Clamp to bounds
        __m512 minBounds = _mm512_set1_ps(w_min);
        __m512 maxBounds = _mm512_set1_ps(w_max);
        updatedWeights = _mm512_max_ps(_mm512_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm512_storeu_ps(&synapseWeights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    size_t vectorSize = synapseCount - (synapseCount % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 preSum = _mm256_setzero_ps();
        __m256 postSum = _mm256_setzero_ps();
        __m256 productSum = _mm256_setzero_ps();
        
        alignas(32) float preHistory[8];
        alignas(32) float postHistory[8];
        for (size_t j = 0; j < 8; ++j) {
            preHistory[j] = (i + j < maxHistorySize) ? preSpikeHistory[i + j] : 0.0f;
            postHistory[j] = (i + j < maxHistorySize) ? postSpikeHistory[i + j] : 0.0f;
        }
        
        for (size_t j = 0; j < 8; ++j) {
            if (plasticityFlags[i + j]) {
                float preCorr = preHistory[j];
                float postCorr = postHistory[j];
                
                __m256 preVal = _mm256_set1_ps(preCorr);
                __m256 postVal = _mm256_set1_ps(postCorr);
                
                preSum = _mm256_add_ps(preSum, preVal);
                postSum = _mm256_add_ps(postSum, postVal);
                
                __m256 product = _mm256_mul_ps(preVal, postVal);
                productSum = _mm256_add_ps(productSum, product);
            }
        }
        
        __m256 correlation = _mm256_mul_ps(learningRate,
            _mm256_sub_ps(productSum, _mm256_mul_ps(preSum, postSum)));
        
        __m256 currentWeights = _mm256_loadu_ps(&synapseWeights[i]);
        __m256 updatedWeights = _mm256_add_ps(currentWeights, correlation);
        
        __m256 minBounds = _mm256_set1_ps(w_min);
        __m256 maxBounds = _mm256_set1_ps(w_max);
        updatedWeights = _mm256_max_ps(_mm256_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm256_storeu_ps(&synapseWeights[i], updatedWeights);
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    size_t vectorSize = synapseCount - (synapseCount % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 preSum = _mm_setzero_ps();
        __m128 postSum = _mm_setzero_ps();
        __m128 productSum = _mm_setzero_ps();
        
        alignas(16) float preHistory[4];
        alignas(16) float postHistory[4];
        for (size_t j = 0; j < 4; ++j) {
            preHistory[j] = (i + j < maxHistorySize) ? preSpikeHistory[i + j] : 0.0f;
            postHistory[j] = (i + j < maxHistorySize) ? postSpikeHistory[i + j] : 0.0f;
        }
        
        for (size_t j = 0; j < 4; ++j) {
            if (plasticityFlags[i + j]) {
                float preCorr = preHistory[j];
                float postCorr = postHistory[j];
                
                __m128 preVal = _mm_set1_ps(preCorr);
                __m128 postVal = _mm_set1_ps(postCorr);
                
                preSum = _mm_add_ps(preSum, preVal);
                postSum = _mm_add_ps(postSum, postVal);
                
                __m128 product = _mm_mul_ps(preVal, postVal);
                productSum = _mm_add_ps(productSum, product);
            }
        }
        
        __m128 correlation = _mm_mul_ps(learningRate,
            _mm_sub_ps(productSum, _mm_mul_ps(preSum, postSum)));
        
        __m128 currentWeights = _mm_loadu_ps(&synapseWeights[i]);
        __m128 updatedWeights = _mm_add_ps(currentWeights, correlation);
        
        __m128 minBounds = _mm_set1_ps(w_min);
        __m128 maxBounds = _mm_set1_ps(w_max);
        updatedWeights = _mm_max_ps(_mm_min_ps(updatedWeights, maxBounds), minBounds);
        
        _mm_storeu_ps(&synapseWeights[i], updatedWeights);
    }
    
#else
    // Scalar fallback implementation
    for (; i < synapseCount; ++i) {
        if (!plasticityFlags[i]) continue;
        
        float correlation = 0.0f;
        float preSum = 0.0f;
        float postSum = 0.0f;
        
        // Compute covariance for this synapse
        for (size_t j = 0; j < maxHistorySize; ++j) {
            float preCorr = (i + j < maxHistorySize) ? preSpikeHistory[i + j] : 0.0f;
            float postCorr = (i + j < maxHistorySize) ? postSpikeHistory[i + j] : 0.0f;
            
            preSum += preCorr;
            postSum += postCorr;
            correlation += preCorr * postCorr;
        }
        
        // Apply Hebbian rule
        float delta = learningRate * (correlation - preSum * postSum);
        synapseWeights[i] += delta;
        synapseWeights[i] = std::clamp(synapseWeights[i], w_min, w_max);
    }
#endif
    
    // Handle remaining elements with scalar code
    for (; i < synapseCount; ++i) {
        if (!plasticityFlags[i]) continue;
        
        float correlation = 0.0f;
        float preSum = 0.0f;
        float postSum = 0.0f;
        
        for (size_t j = 0; j < maxHistorySize; ++j) {
            float preCorr = (i + j < maxHistorySize) ? preSpikeHistory[i + j] : 0.0f;
            float postCorr = (i + j < maxHistorySize) ? postSpikeHistory[i + j] : 0.0f;
            
            preSum += preCorr;
            postSum += postCorr;
            correlation += preCorr * postCorr;
        }
        
        float delta = learningRate * (correlation - preSum * postSum);
        synapseWeights[i] += delta;
        synapseWeights[i] = std::clamp(synapseWeights[i], w_min, w_max);
    }
}

// Optimized spike history computation for Hebbian learning
void VectorizedHebbian::computeSpikeHistory(
    float* spikeHistory,
    const float* spikeTimes,
    const uint8_t* spikeActive,
    size_t numNeurons,
    float timeWindow,
    float decayRate) {
    
    // Efficient spike history computation using vectorization
    size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
    size_t vectorSize = numNeurons - (numNeurons % 16);
    
    for (; i < vectorSize; i += 16) {
        __m512 history = _mm512_loadu_ps(&spikeHistory[i]);
        __m512 times = _mm512_loadu_ps(&spikeTimes[i]);
        __m512 active = _mm512_loadu_ps(&spikeActive[i]);
        
        // Apply exponential decay
        __m512 decayFactor = _mm512_set1_ps(std::exp(-decayRate));
        history = _mm512_mul_ps(history, decayFactor);
        
        // Add new spike contributions
        __m512 spikeMask = _mm512_cmpgt_ps(times, _mm512_setzero_ps());
        __m512 spikeAdd = _mm512_and_ps(spikeMask, _mm512_set1_ps(1.0f));
        history = _mm512_add_ps(history, spikeAdd);
        
        _mm512_storeu_ps(&spikeHistory[i], history);
    }
    
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
    size_t vectorSize = numNeurons - (numNeurons % 8);
    
    for (; i < vectorSize; i += 8) {
        __m256 history = _mm256_loadu_ps(&spikeHistory[i]);
        __m256 times = _mm256_loadu_ps(&spikeTimes[i]);
        __m256 active = _mm256_loadu_ps(&spikeActive[i]);
        
        __m256 decayFactor = _mm256_set1_ps(std::exp(-decayRate));
        history = _mm256_mul_ps(history, decayFactor);
        
        __m256 spikeMask = _mm256_cmpgt_ps(times, _mm256_setzero_ps());
        __m256 spikeAdd = _mm256_and_ps(spikeMask, _mm256_set1_ps(1.0f));
        history = _mm256_add_ps(history, spikeAdd);
        
        _mm256_storeu_ps(&spikeHistory[i], history);
    }
    
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
    size_t vectorSize = numNeurons - (numNeurons % 4);
    
    for (; i < vectorSize; i += 4) {
        __m128 history = _mm_loadu_ps(&spikeHistory[i]);
        __m128 times = _mm_loadu_ps(&spikeTimes[i]);
        __m128 active = _mm_loadu_ps(&spikeActive[i]);
        
        __m128 decayFactor = _mm_set1_ps(std::exp(-decayRate));
        history = _mm_mul_ps(history, decayFactor);
        
        __m128 spikeMask = _mm_cmpgt_ps(times, _mm_setzero_ps());
        __m128 spikeAdd = _mm_and_ps(spikeMask, _mm_set1_ps(1.0f));
        history = _mm_add_ps(history, spikeAdd);
        
        _mm_storeu_ps(&spikeHistory[i], history);
    }
    
#else
    // Scalar fallback
    for (; i < numNeurons; ++i) {
        if (spikeActive[i]) {
            spikeHistory[i] = 1.0f;  // New spike
        } else {
            spikeHistory[i] *= std::exp(-decayRate);  // Decay
        }
    }
#endif
    
    // Handle remaining elements
    for (; i < numNeurons; ++i) {
        if (spikeActive[i]) {
            spikeHistory[i] = 1.0f;
        } else {
            spikeHistory[i] *= std::exp(-decayRate);
        }
    }
}

} // namespace nlm
