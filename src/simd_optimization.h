// SIMD-optimized neural computation functions for NLM
// This file provides SIMD-accelerated versions of critical neural computations
// to improve performance in large-scale neural simulations

#include <algorithm>
#include <cmath>
#include <immintrin.h>  // For AVX intrinsics (if available)
#include <smmintrin.h>   // For SSE4.1 intrinsics (if available)

namespace nlm {

// SIMD-optimized spike detection for batches of neurons
// Processes 8 or 16 neurons simultaneously using AVX/SSE instructions
inline void batchSpikeDetectionSIMD(
    const std::vector<float>& membranePotentials,
    const std::vector<float>& thresholds,
    const std::vector<MembranePotential>& adaptationVariables,
    std::vector<bool>& fired,
    std::vector<MembranePotential>& newMembranePotentials,
    Timestamp currentTime,
    float timestep,
    float tau,
    float C,
    float V_rest,
    float V_reset) {
    
    const size_t batchSize = membranePotentials.size();
    if (batchSize == 0) return;
    
    // Determine available SIMD width
    const size_t simdWidth = sizeof(__m128) / sizeof(float);  // 4 floats per 128-bit SIMD
    
    // Process neurons in SIMD batches
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth) {
            // Full SIMD batch processing
            __m128 V_batch = _mm_loadu_ps(&membranePotentials[i]);
            __m128 threshold_batch = _mm_loadu_ps(&thresholds[i]);
            __m128 adaptation_batch = _mm_loadu_ps(&adaptationVariables[i]);
            __m128 V_rest_batch = _mm_set1_ps(V_rest);
            __m128 V_reset_batch = _mm_set1_ps(V_reset);
            
            // Calculate synaptic contribution (simplified - assuming 0 for this example)
            __m128 synapticContribution = _mm_set1_ps(0.0f);
            
            // Calculate leak contribution: (V_rest - V) / tau
            __m128 leakContribution = _mm_div_ps(_mm_sub_ps(V_rest_batch, V_batch), _mm_set1_ps(tau));
            
            // Calculate total contribution: leak + synaptic
            __m128 totalContribution = _mm_add_ps(leakContribution, synapticContribution);
            
            // Scale by timestep and time conversion factor
            __m128 scaledContribution = _mm_mul_ps(totalContribution, _mm_set1_ps(timestep * 1000.0f));
            
            // Update membrane potentials: V_new = V + scaledContribution
            __m128 V_new = _mm_add_ps(V_batch, scaledContribution);
            
            // Apply spike-frequency adaptation
            if (_mm_testz_ps(_mm_set1_ps(0.0f), adaptation_batch)) {
                // Adaptation > 0: V -= adaptation * 0.01
                __m128 adaptationEffect = _mm_mul_ps(adaptation_batch, _mm_set1_ps(0.01f));
                V_new = _mm_sub_ps(V_new, adaptationEffect);
                
                // Decay adaptation: adaptation *= 0.95
                adaptation_batch = _mm_mul_ps(adaptation_batch, _mm_set1_ps(0.95f));
                _mm_storeu_ps(&adaptationVariables[i], adaptation_batch);
            }
            
            // Clamp membrane potentials to prevent instability
            __m128 V_clamped = _mm_max_ps(V_new, _mm_set1_ps(-100.0f));
            V_clamped = _mm_min_ps(V_clamped, _mm_set1_ps(50.0f));
            
            // Store updated membrane potentials
            _mm_storeu_ps(&newMembranePotentials[i], V_clamped);
            
            // Check for spikes: V >= threshold
            __m128 spikeMask = _mm_cmpge_ps(V_clamped, threshold_batch);
            
            if (!_mm_testz_ps(spikeMask, spikeMask)) {
                // Some neurons fired - process them individually for spike handling
                for (size_t j = 0; j < simdWidth; ++j) {
                    if (j < remaining && ((_mm_extract_ps(spikeMask, j) & 0x1) != 0)) {
                        fired[i + j] = true;
                        
                        // Record spike time
                        // (In practice, this would use per-neuron spike history arrays)
                        
                        // Reset membrane potential
                        newMembranePotentials[i + j] = V_reset;
                    }
                }
            }
        } else {
            // Partial SIMD batch - fallback to scalar processing
            for (size_t j = 0; j < remaining; ++j) {
                size_t idx = i + j;
                float& V = newMembranePotentials[idx];
                float V_current = membranePotentials[idx];
                float threshold_val = thresholds[idx];
                float adaptation = adaptationVariables[idx];
                
                // Calculate leak contribution
                float leakContribution = (V_rest - V_current) / tau;
                
                // Calculate total contribution (simplified)
                float totalContribution = leakContribution;
                
                // Update membrane potential
                V = V_current + timestep * 1000.0f * totalContribution;
                
                // Apply spike-frequency adaptation
                if (adaptation > 0.0f) {
                    V -= adaptation * 0.01f;
                    adaptation *= 0.95f;
                    adaptationVariables[idx] = adaptation;
                }
                
                // Clamp membrane potential
                V = std::clamp(V, -100.0f, 50.0f);
                
                // Check for spike
                if (V >= threshold_val) {
                    fired[idx] = true;
                    V = V_reset;
                }
            }
        }
    }
}

// SIMD-optimized synaptic weight updates for batches of synapses
// Processes synaptic plasticity calculations in parallel
inline void batchSynapseUpdateSIMD(
    const std::vector<SynapticWeight>& weights,
    const std::vector<float>& eligibilityTraces,
    const std::vector<float>& plasticityFactors,
    std::vector<SynapticWeight>& newWeights,
    float maxWeight = 1.0f,
    float minWeight = -1.0f) {
    
    const size_t batchSize = weights.size();
    if (batchSize == 0) return;
    
    const size_t simdWidth = sizeof(__m128) / sizeof(float);
    
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth) {
            // Full SIMD batch processing
            __m128 weightBatch = _mm_loadu_ps(&weights[i]);
            __m128 eligibilityBatch = _mm_loadu_ps(&eligibilityTraces[i]);
            __m128 plasticityBatch = _mm_loadu_ps(&plasticityFactors[i]);
            
            // Calculate weight change: delta = eligibility * plasticity
            __m128 delta = _mm_mul_ps(eligibilityBatch, plasticityBatch);
            
            // Apply weight change
            __m128 newWeightBatch = _mm_add_ps(weightBatch, delta);
            
            // Clamp weights to bounds
            __m128 maxWeightBatch = _mm_set1_ps(maxWeight);
            __m128 minWeightBatch = _mm_set1_ps(minWeight);
            
            __m128 clampedWeightBatch = _mm_max_ps(_mm_min_ps(newWeightBatch, maxWeightBatch), minWeightBatch);
            
            // Store updated weights
            _mm_storeu_ps(&newWeights[i], clampedWeightBatch);
        } else {
            // Partial SIMD batch - scalar fallback
            for (size_t j = 0; j < remaining; ++j) {
                size_t idx = i + j;
                float eligibility = eligibilityTraces[idx];
                float plasticity = plasticityFactors[idx];
                float weight = weights[idx];
                
                // Calculate weight change
                float delta = eligibility * plasticity;
                float newWeight = weight + delta;
                
                // Clamp to bounds
                newWeight = std::clamp(newWeight, minWeight, maxWeight);
                newWeights[idx] = newWeight;
            }
        }
    }
}

// SIMD-optimized vector operations for neural ensemble processing
// Provides optimized operations for arrays of neural values
inline void batchVectorOperationsSIMD(
    const std::vector<float>& values1,
    const std::vector<float>& values2,
    std::vector<float>& result,
    const std::string& operation = "add") {
    
    const size_t batchSize = values1.size();
    if (batchSize == 0) return;
    
    const size_t simdWidth = sizeof(__m128) / sizeof(float);
    
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth && operation == "add") {
            // Full SIMD batch for addition
            __m128 val1 = _mm_loadu_ps(&values1[i]);
            __m128 val2 = _mm_loadu_ps(&values2[i]);
            __m128 result_vec = _mm_add_ps(val1, val2);
            _mm_storeu_ps(&result[i], result_vec);
        } else if (remaining == simdWidth && operation == "subtract") {
            // Full SIMD batch for subtraction
            __m128 val1 = _mm_loadu_ps(&values1[i]);
            __m128 val2 = _mm_loadu_ps(&values2[i]);
            __m128 result_vec = _mm_sub_ps(val1, val2);
            _mm_storeu_ps(&result[i], result_vec);
        } else if (remaining == simdWidth && operation == "multiply") {
            // Full SIMD batch for multiplication
            __m128 val1 = _mm_loadu_ps(&values1[i]);
            __m128 val2 = _mm_loadu_ps(&values2[i]);
            __m128 result_vec = _mm_mul_ps(val1, val2);
            _mm_storeu_ps(&result[i], result_vec);
        } else if (remaining == simdWidth && operation == "dot_product") {
            // Dot product computation
            __m128 val1 = _mm_loadu_ps(&values1[i]);
            __m128 val2 = _mm_loadu_ps(&values2[i]);
            __m128 product = _mm_mul_ps(val1, val2);
            
            // Horizontal add to sum elements
            __m128 shuffled = _mm_shuffle_ps(product, product, _MM_SHUFFLE(2, 3, 0, 1));
            __m128 summed = _mm_add_ps(product, shuffled);
            shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(0, 1, 2, 3));
            __m128 final = _mm_add_ps(summed, shuffled);
            
            float result_val = _mm_cvtss_f32(final);
            result[i] = result_val;
        } else {
            // Scalar fallback for partial batches or different operations
            for (size_t j = 0; j < remaining; ++j) {
                size_t idx = i + j;
                float val1 = values1[idx];
                float val2 = values2[idx];
                float result_val = 0.0f;
                
                if (operation == "add") {
                    result_val = val1 + val2;
                } else if (operation == "subtract") {
                    result_val = val1 - val2;
                } else if (operation == "multiply") {
                    result_val = val1 * val2;
                } else if (operation == "divide") {
                    result_val = val1 / val2;
                } else if (operation == "dot_product") {
                    result_val = val1 * val2;  // Simplified for example
                }
                
                result[idx] = result_val;
            }
        }
    }
}

// SIMD-optimized normalization function for neural activation values
// Applies softmax-like normalization using SIMD
inline void batchNormalizationSIMD(
    const std::vector<float>& input,
    std::vector<float>& output,
    float epsilon = 1e-8f) {
    
    const size_t batchSize = input.size();
    if (batchSize == 0) return;
    
    const size_t simdWidth = sizeof(__m128) / sizeof(float);
    
    // Find maximum value in batch
    float maxVal = -std::numeric_limits<float>::max();
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth) {
            __m128 values = _mm_loadu_ps(&input[i]);
            maxVal = std::max(maxVal, _mm_cvtss_f32(_mm_max_ps(values, _mm_setzero_ps())));
            maxVal = std::max(maxVal, _mm_cvtss_f32(_mm_max_ps(_mm_shuffle_ps(values, values, _MM_SHUFFLE(2, 3, 0, 1)), _mm_shuffle_ps(values, values, _MM_SHUFFLE(0, 1, 2, 3)))));
        } else {
            for (size_t j = 0; j < remaining; ++j) {
                maxVal = std::max(maxVal, input[i + j]);
            }
        }
    }
    
    // Apply softmax normalization: exp(x - max) / sum(exp(x - max))
    float sumExp = 0.0f;
    
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth) {
            __m128 values = _mm_loadu_ps(&input[i]);
            __m128 shifted = _mm_sub_ps(values, _mm_set1_ps(maxVal));
            __m128 expValues = _mm_exp_ps(shifted);
            _mm_storeu_ps(&output[i], expValues);
            
            // Accumulate sum
            __m128 shuffled = _mm_shuffle_ps(expValues, expValues, _MM_SHUFFLE(2, 3, 0, 1));
            __m128 summed = _mm_add_ps(expValues, shuffled);
            shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(0, 1, 2, 3));
            __m128 total = _mm_add_ps(summed, shuffled);
            sumExp += _mm_cvtss_f32(total);
        } else {
            for (size_t j = 0; j < remaining; ++j) {
                output[i + j] = std::exp(input[i + j] - maxVal);
                sumExp += output[i + j];
            }
        }
    }
    
    // Normalize by dividing by sum
    float invSum = 1.0f / (sumExp + epsilon);
    
    for (size_t i = 0; i < batchSize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, batchSize - i);
        
        if (remaining == simdWidth) {
            __m128 values = _mm_loadu_ps(&output[i]);
            __m128 normalized = _mm_mul_ps(values, _mm_set1_ps(invSum));
            _mm_storeu_ps(&output[i], normalized);
        } else {
            for (size_t j = 0; j < remaining; ++j) {
                output[i + j] *= invSum;
            }
        }
    }
}

// SIMD-optimized memory access pattern for neural arrays
// Implements optimized loading/storing with prefetching
inline void optimizedMemoryAccessSIMD(
    const std::vector<float>& source,
    std::vector<float>& destination,
    size_t sourceOffset = 0,
    size_t destOffset = 0,
    size_t copySize = 0) {
    
    const size_t actualCopySize = copySize > 0 ? 
        std::min({copySize, source.size() - sourceOffset, destination.size() - destOffset}) : 
        std::min(source.size() - sourceOffset, destination.size() - destOffset);
    
    if (actualCopySize == 0) return;
    
    const size_t simdWidth = sizeof(__m128) / sizeof(float);
    
    // Prefetch next cache line for better memory access patterns
    if (actualCopySize >= simdWidth * 4) {
        _mm_prefetch(&source[sourceOffset + simdWidth * 8], _MM_HINT_T0);
        _mm_prefetch(&destination[destOffset + simdWidth * 8], _MM_HINT_T0);
    }
    
    for (size_t i = 0; i < actualCopySize; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, actualCopySize - i);
        
        if (remaining == simdWidth) {
            // Aligned SIMD load/store for best performance
            __m128 values = _mm_load_ps(&source[sourceOffset + i]);
            _mm_store_ps(&destination[destOffset + i], values);
        } else {
            // Unaligned access or partial SIMD - use unaligned variants
            __m128 values = _mm_loadu_ps(&source[sourceOffset + i]);
            _mm_storeu_ps(&destination[destOffset + i], values);
        }
    }
}

// SIMD-optimized statistical operations for neural ensemble analysis
// Provides fast computation of mean, variance, standard deviation
inline void batchStatisticsSIMD(
    const std::vector<float>& values,
    float& mean,
    float& variance,
    float& stddev,
    size_t startIdx = 0,
    size_t endIdx = 0) {
    
    const size_t actualStart = startIdx;
    const size_t actualEnd = endIdx > 0 ? std::min(endIdx, values.size()) : values.size();
    const size_t batchSize = actualEnd - actualStart;
    
    if (batchSize == 0) {
        mean = variance = stddev = 0.0f;
        return;
    }
    
    const size_t simdWidth = sizeof(__m128) / sizeof(float);
    
    // Calculate mean using SIMD
    float sum = 0.0f;
    for (size_t i = actualStart; i < actualEnd; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, actualEnd - i);
        
        if (remaining == simdWidth) {
            __m128 values = _mm_load_ps(&values[i]);
            // Horizontal add to sum elements
            __m128 shuffled = _mm_shuffle_ps(values, values, _MM_SHUFFLE(2, 3, 0, 1));
            __m128 summed = _mm_add_ps(values, shuffled);
            shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(0, 1, 2, 3));
            __m128 total = _mm_add_ps(summed, shuffled);
            sum += _mm_cvtss_f32(total);
        } else {
            for (size_t j = 0; j < remaining; ++j) {
                sum += values[i + j];
            }
        }
    }
    
    mean = sum / batchSize;
    
    // Calculate variance using SIMD
    float sumSqDiff = 0.0f;
    for (size_t i = actualStart; i < actualEnd; i += simdWidth) {
        const size_t remaining = std::min(simdWidth, actualEnd - i);
        
        if (remaining == simdWidth) {
            __m128 values = _mm_load_ps(&values[i]);
            __m128 meanVec = _mm_set1_ps(mean);
            __m128 diff = _mm_sub_ps(values, meanVec);
            __m128 sqDiff = _mm_mul_ps(diff, diff);
            
            // Horizontal add for squared differences
            __m128 shuffled = _mm_shuffle_ps(sqDiff, sqDiff, _MM_SHUFFLE(2, 3, 0, 1));
            __m128 summed = _mm_add_ps(sqDiff, shuffled);
            shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(0, 1, 2, 3));
            __m128 total = _mm_add_ps(summed, shuffled);
            sumSqDiff += _mm_cvtss_f32(total);
        } else {
            for (size_t j = 0; j < remaining; ++j) {
                float diff = values[i + j] - mean;
                sumSqDiff += diff * diff;
            }
        }
    }
    
    variance = sumSqDiff / batchSize;
    stddev = std::sqrt(variance);
}

} // namespace nlm
