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
 * @file SIMDVectorization.hpp
 * 
 * SIMD vectorization system for NLM
 * 
 * This file provides the SIMD vectorization system for NLM, implementing:
 * - SIMD utilities
 * - Vectorized LIF update
 * - Vectorized synapse update
 * - Vectorized STDP update
 * - Vectorized statistics computation
 * 
 * The SIMD vectorization system in NLM:
 * - **SIMDUtils**: SIMD utilities for portable vectorized operations
 * - **VectorizedLIF**: Vectorized LIF update
 * - **VectorizedSynapse**: Vectorized synapse update
 * - **VectorizedSTDP**: Vectorized STDP update
 * - **VectorizedStats**: Vectorized statistics computation
 * 
 * Key features:
 * - **SIMD utilities**: SIMD utilities for portable vectorized operations
 * - **Vectorized LIF**: Vectorized LIF update
 * - **Vectorized synapse**: Vectorized synapse update
 * - **Vectorized STDP**: Vectorized STDP update
 * - **Vectorized stats**: Vectorized statistics computation
 * 
 * SIMD characteristics in NLM:
 * - **SIMD utilities**: SIMD utilities for portable vectorized operations
 * - **Vectorized LIF**: Vectorized LIF update
 * - **Vectorized synapse**: Vectorized synapse update
 * - **Vectorized STDP**: Vectorized STDP update
 * - **Vectorized stats**: Vectorized statistics computation
 * 
 * @note Critical for performance optimization
 * @note Used extensively in performance systems
 * @note Essential for scaling to large networks
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class SIMDUtils
 * 
 * Neural implementation of SIMD utilities.
 * 
 * This class implements the SIMD utilities, providing
 * SIMD utilities for NLM. It is essential for:
 * - **SIMD utilities**: SIMD utilities for portable vectorized operations
 * - **SIMD width**: SIMD width detection
 * - **SIMD type**: SIMD type detection
 * - **Memory alignment**: Memory alignment detection
 * - **SIMD availability**: SIMD availability detection
 * 
 * SIMD characteristics in NLM:
 * - **SIMD utilities**: SIMD utilities for portable vectorized operations
 * - **SIMD width**: SIMD width detection
 * - **SIMD type**: SIMD type detection
 * - **Memory alignment**: Memory alignment detection
 * - **SIMD availability**: SIMD availability detection
 * 
 * Key components:
 * - **SIMD width**: SIMD width
 * - **SIMD type**: SIMD type
 * - **Memory alignment**: Memory alignment
 * - **SIMD availability**: SIMD availability
 * 
 * @note Critical for performance optimization
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class SIMDUtils {
public:
    /**
     * @brief SIMD width
     * 
     * Returns the SIMD width. This is used for:
     * - SIMD width access
     * - SIMD width management
     * - SIMD width processing
     * - SIMD width control
     */
    static constexpr size_t SIMD_WIDTH = NLM_SIMD_WIDTH;
    
    /**
     * @brief Check if SIMD is available
     * 
     * Checks if SIMD is available. This is used for:
     * - SIMD availability checking
     * - SIMD availability management
     * - SIMD availability processing
     * - SIMD availability control
     */
    static bool isSIMDAvailable() {
#if defined(NLM_SIMD_AVX512) || defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE)
        return true;
#else
        return false;
#endif
    }
    
    /**
     * @brief Get SIMD type name
     * 
     * Returns the SIMD type name. This is used for:
     * - SIMD type access
     * - SIMD type management
     * - SIMD type processing
     * - SIMD type control
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
     * @brief Get optimal memory alignment for SIMD loads/stores
     * 
     * Returns the optimal memory alignment for SIMD loads/stores.
     * This is used for:
     * - Memory alignment access
     * - Memory alignment management
     * - Memory alignment processing
     * - Memory alignment control
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
 * @struct VectorizedLIF
 * 
 * Neural implementation of vectorized LIF (Leaky Integrate-and-Fire) update.
 * 
 * This structure implements the vectorized LIF update, providing
 * vectorized LIF update for NLM. It is essential for:
 * - **Vectorized LIF**: Vectorized LIF update
 * - **Membrane potential**: Membrane potential updates
 * - **Synaptic integration**: Synaptic integration
 * - **Spike detection**: Spike detection
 * - **Refractory management**: Refractory management
 * 
 * Vectorized LIF characteristics in NLM:
 * - **Vectorized LIF**: Vectorized LIF update
 * - **Membrane potential**: Membrane potential updates
 * - **Synaptic integration**: Synaptic integration
 * - **Spike detection**: Spike detection
 * - **Refractory management**: Refractory management
 * 
 * Key components:
 * - **LIF parameters**: LIF parameters (broadcast to all lanes)
 * - **Membrane potential update**: Membrane potential update
 * - **Synaptic integration**: Synaptic integration
 * - **Spike detection**: Spike detection
 * - **Refractory management**: Refractory management
 * 
 * @note Critical for performance optimization
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
struct VectorizedLIF {
    /**
     * Rest potential (mV)
     */
    float V_rest;
    
    /**
     * Reset potential (mV)
     */
    float V_reset;
    
    /**
     * Threshold potential (mV)
     */
    float V_threshold;
    
    /**
     * Time constant (ms)
     */
    float tau;
    
    /**
     * Membrane capacitance (nF)
     */
    float C;
    
    /**
     * Timestep (ms)
     */
    float dt;
    
    /**
     * Refractory period decay
     */
    float refractoryDecay;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default vectorized LIF instance with default parameters.
     * The vectorized LIF is initialized with default values for
     * all LIF parameters.
     */
    VectorizedLIF()
        : V_rest(-70.0f), V_reset(-70.0f), V_threshold(-55.0f)
        , tau(20.0f), C(1.0f), dt(1.0f), refractoryDecay(1.0f) {}
    
    /**
     * @brief Update membrane potentials for a batch of neurons
     * 
     * Updates membrane potentials for a batch of neurons using vectorized operations.
     * This is used for:
     * - Membrane potential update
     * - Synaptic integration
     * - Spike detection
     * - Refractory management
     */
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    size_t update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                   FArray& fired, size_t count);
};

/**
 * @struct VectorizedSynapse
 * 
 * Neural implementation of vectorized synapse update.
 * 
 * This structure implements the vectorized synapse update, providing
 * vectorized synapse update for NLM. It is essential for:
 * - **Vectorized synapse**: Vectorized synapse update
 * - **Short-term plasticity**: Short-term plasticity updates
 * - **Facilitation**: Facilitation updates
 * - **Depression**: Depression updates
 * 
 * Vectorized synapse characteristics in NLM:
 * - **Vectorized synapse**: Vectorized synapse update
 * - **Short-term plasticity**: Short-term plasticity updates
 * - **Facilitation**: Facilitation updates
 * - **Depression**: Depression updates
 * 
 * Key components:
 * - **Facilitation time constant**: Facilitation time constant
 * - **Depression time constant**: Depression time constant
 * - **Max utilization**: Max utilization
 * - **STD update**: STD update
 * 
 * @note Critical for performance optimization
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
struct VectorizedSynapse {
    /**
     * Facilitation time constant (ms)
     */
    float tau_facilitation;
    
    /**
     * Depression time constant (ms)
     */
    float tau_depression;
    
    /**
     * Max utilization
     */
    float U_max;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default vectorized synapse instance with default parameters.
     * The vectorized synapse is initialized with default values for
     * all synapse parameters.
     */
    VectorizedSynapse()
        : tau_facilitation(100.0f), tau_depression(200.0f), U_max(1.0f) {}
    
    /**
     * @brief Update short-term plasticity state
     * 
     * Updates short-term plasticity state using vectorized operations.
     * This is used for:
     * - STD update
     * - Facilitation update
     * - Depression update
     */
    template<typename DArray, typename FArray>
    void updateSTD(DArray& depression, FArray& facilitation,
                   const float* lastPreTime, const float* lastPostTime,
                   float currentTime, size_t count);
};

/**
 * @struct VectorizedSTDP
 * 
 * Neural implementation of vectorized STDP update.
 * 
 * This structure implements the vectorized STDP update, providing
 * vectorized STDP update for NLM. It is essential for:
 * - **Vectorized STDP**: Vectorized STDP update
 * - **STDP calculation**: STDP calculation
 * - **Weight update**: Weight update
 * - **LTP**: Long-term potentiation
 * - **LTD**: Long-term depression
 * 
 * Vectorized STDP characteristics in NLM:
 * - **Vectorized STDP**: Vectorized STDP update
 * - **STDP calculation**: STDP calculation
 * - **Weight update**: Weight update
 * - **LTP**: Long-term potentiation
 * - **LTD**: Long-term depression
 * 
 * Key components:
 * - **LTP time constant**: LTP time constant
 * - **LTD time constant**: LTD time constant
 * - **LTP amplitude**: LTP amplitude
 * - **LTD amplitude**: LTD amplitude
 * - **Max weight**: Max weight
 * - **Min weight**: Min weight
 * 
 * @note Critical for performance optimization
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
struct VectorizedSTDP {
    /**
     * LTP time constant (ms)
     */
    float tau_plus;
    
    /**
     * LTD time constant (ms)
     */
    float tau_minus;
    
    /**
     * LTP amplitude
     */
    float A_plus;
    
    /**
     * LTD amplitude
     */
    float A_minus;
    
    /**
     * Max weight
     */
    float w_max;
    
    /**
     * Min weight
     */
    float w_min;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default vectorized STDP instance with default parameters.
     * The vectorized STDP is initialized with default values for
     * all STDP parameters.
     */
    VectorizedSTDP()
        : tau_plus(20.0f), tau_minus(20.0f), A_plus(0.01f), A_minus(0.012f)
        , w_max(1.0f), w_min(-1.0f) {}
    
    /**
     * @brief Calculate weight update for a pair of pre/post spike times
     * 
     * Calculates weight update for a pair of pre/post spike times.
     * This is used for:
     * - STDP calculation
     * - Weight update
     * - LTP/LTD calculation
     */
    float calculateDeltaW(float dt);
    
    /**
     * @brief Vectorized weight update
     * 
     * Performs vectorized weight update.
     * This is used for:
     * - Weight update
     * - STDP update
     * - Vectorized STDP
     */
    template<typename WArray, typename PreArray, typename PostArray>
    void updateWeights(WArray& weights, const PreArray& preSpikes,
                       const PostArray& postSpikes, size_t numPre, size_t numPost);
};

/**
 * @struct VectorizedStats
 * 
 * Neural implementation of vectorized statistics computation.
 * 
 * This structure implements the vectorized statistics computation, providing
 * vectorized statistics computation for NLM. It is essential for:
 * - **Vectorized stats**: Vectorized statistics computation
 * - **Sum calculation**: Sum calculation
 * - **Mean calculation**: Mean calculation
 * - **Variance calculation**: Variance calculation
 * - **Max calculation**: Max calculation
 * - **Clamp operation**: Clamp operation
 * 
 * Vectorized stats characteristics in NLM:
 * - **Vectorized stats**: Vectorized statistics computation
 * - **Sum calculation**: Sum calculation
 * - **Mean calculation**: Mean calculation
 * - **Variance calculation**: Variance calculation
 * - **Max calculation**: Max calculation
 * - **Clamp operation**: Clamp operation
 * 
 * Key components:
 * - **Sum calculation**: Sum calculation
 * - **Mean calculation**: Mean calculation
 * - **Variance calculation**: Variance calculation
 * - **Max calculation**: Max calculation
 * - **Clamp operation**: Clamp operation
 * 
 * @note Critical for performance optimization
 * @see SIMDVectorization.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
struct VectorizedStats {
    /**
     * @brief Compute sum of array elements
     * 
     * Computes sum of array elements.
     * This is used for:
     * - Sum calculation
     * - Array summation
     * - Vectorized summation
     */
    template<typename Array>
    static float sum(const Array& arr, size_t count);
    
    /**
     * @brief Compute mean of array elements
     * 
     * Computes mean of array elements.
     * This is used for:
     * - Mean calculation
     * - Array mean
     * - Vectorized mean
     */
    template<typename Array>
    static float mean(const Array& arr, size_t count);
    
    /**
     * @brief Compute variance of array elements
     * 
     * Computes variance of array elements.
     * This is used for:
     * - Variance calculation
     * - Array variance
     * - Vectorized variance
     */
    template<typename Array>
    static float variance(const Array& arr, size_t count);
    
    /**
     * @brief Compute max of array elements
     * 
     * Computes max of array elements.
     * This is used for:
     * - Max calculation
     * - Array max
     * - Vectorized max
     */
    template<typename Array>
    static float max(const Array& arr, size_t count);
    
    /**
     * @brief Clamp array values to range
     * 
     * Clamps array values to range.
     * This is used for:
     * - Clamp operation
     * - Range clamping
     * - Vectorized clamp
     */
    template<typename Array>
    static void clamp(Array& arr, size_t count, float minVal, float maxVal);
};

// Vectorized STDP update implementation

// Vectorized STDP weight update implementation
template<typename WArray, typename PreArray, typename PostArray>
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                  const PostArray& postSpikes, size_t numPre, size_t numPost) {
    size_t i = 0;
    size_t vectorSize = 0;
    
    // Determine vector size based on SIMD width
    if constexpr (NLM_SIMD_WIDTH == 16) {
        vectorSize = (numPre > numPost) ? numPre : numPost;
        vectorSize -= (vectorSize % 16);
    } else if constexpr (NLM_SIMD_WIDTH == 8) {
        vectorSize = (numPre > numPost) ? numPre : numPost;
        vectorSize -= (vectorSize % 8);
    } else if constexpr (NLM_SIMD_WIDTH == 4) {
        vectorSize = (numPre > numPost) ? numPre : numPost;
        vectorSize -= (vectorSize % 4);
    }
    
    // Vectorized STDP update for pre-to-post spikes (LTP)
    for (size_t p = 0; p < numPre; ++p) {
        if (preSpikes[p] <= 0.0f) continue;
        
        for (size_t q = 0; q < numPost; ++q) {
            if (postSpikes[q] <= 0.0f) continue;
            
            float dt = postSpikes[q] - preSpikes[p];
            if (dt > 0.0f) {
                float deltaW = A_plus * std::exp(-dt / tau_plus);
                
                // Apply weight update
                size_t idx = p * numPost + q;
                if (idx < weights.size()) {
                    weights[idx] += deltaW;
                    weights[idx] = std::clamp(weights[idx], w_min, w_max);
                }
            }
        }
    }
    
    // Vectorized STDP update for post-to-pre spikes (LTD)
    for (size_t q = 0; q < numPost; ++q) {
        if (postSpikes[q] <= 0.0f) continue;
        
        for (size_t p = 0; p < numPre; ++p) {
            if (preSpikes[p] <= 0.0f) continue;
            
            float dt = preSpikes[p] - postSpikes[q];
            if (dt > 0.0f) {
                float deltaW = -A_minus * std::exp(-dt / tau_minus);
                
                // Apply weight update
                size_t idx = p * numPost + q;
                if (idx < weights.size()) {
                    weights[idx] += deltaW;
                    weights[idx] = std::clamp(weights[idx], w_min, w_max);
                }
            }
        }
    }
}

// Vectorized STDP update implementation for compatible arrays
inline void VectorizedSTDP::updateWeights(float* weights, const float* preSpikes,
                                         const float* postSpikes, size_t numPre, size_t numPost) {
    updateWeights(std::vector<float>(weights, weights + numPre * numPost),
                  std::vector<float>(preSpikes, preSpikes + numPre),
                  std::vector<float>(postSpikes, postSpikes + numPost),
                  numPre, numPost);
}

// VectorizedSTDP implementation (inline functions already in header)

// VectorizedStats implementation (inline functions already in header)

// Vectorized STDP implementation (continued)

// STDP update with decay for vector arrays
void VectorizedSTDP::updateWithDecay(std::vector<float>& weights,
                                    const std::vector<float>& preSpikes,
                                    const std::vector<float>& postSpikes,
                                    const std::vector<float>& eligibilityTraces,
                                    float timeStep) {
    size_t numPre = preSpikes.size();
    size_t numPost = postSpikes.size();
    size_t numWeights = weights.size();
    
    // Process each pre-post pair
    for (size_t p = 0; p < numPre; ++p) {
        if (preSpikes[p] <= 0.0f) continue;
        
        for (size_t q = 0; q < numPost; ++q) {
            if (postSpikes[q] <= 0.0f) continue;
            
            size_t idx = p * numPost + q;
            if (idx >= numWeights) continue;
            
            float dt = postSpikes[q] - preSpikes[p];
            if (dt > 0.0f) {
                // LTP
                float deltaW = A_plus * std::exp(-dt / tau_plus);
                weights[idx] += deltaW * eligibilityTraces[p];
            } else {
                // LTD
                float deltaW = -A_minus * std::exp(dt / tau_minus);
                weights[idx] += deltaW * eligibilityTraces[q];
            }
            
            // Apply bounds
            weights[idx] = std::clamp(weights[idx], w_min, w_max);
        }
    }
}

// Vectorized STDP implementation (continued)

// STDP eligibility trace management
void VectorizedSTDP::updateEligibilityTraces(std::vector<float>& traces,
                                             const std::vector<float>& preSpikes,
                                             const std::vector<float>& postSpikes,
                                             float timeStep, float decayRate) {
    for (size_t i = 0; i < traces.size(); ++i) {
        // Decay traces
        traces[i] *= (1.0f - decayRate);
        
        // Add traces for pre-synaptic spikes
        if (i < preSpikes.size() && preSpikes[i] > 0.0f) {
            traces[i] += 1.0f;
        }
        
        // Add traces for post-synaptic spikes
        if (i < postSpikes.size() && postSpikes[i] > 0.0f) {
            traces[i] += 1.0f;
        }
    }
}

// Vectorized STDP implementation (continued)

// Enhanced STDP with dopaminergic modulation
void VectorizedSTDP::updateWithDopamine(std::vector<float>& weights,
                                       const std::vector<float>& preSpikes,
                                       const std::vector<float>& postSpikes,
                                       float dopamineLevel,
                                       float timeStep) {
    size_t numPre = preSpikes.size();
    size_t numPost = postSpikes.size();
    size_t numWeights = weights.size();
    
    // Apply dopaminergic modulation
    float dopamineFactor = 0.5f + 0.5f * std::clamp(dopamineLevel, -1.0f, 1.0f);
    
    for (size_t p = 0; p < numPre; ++p) {
        if (preSpikes[p] <= 0.0f) continue;
        
        for (size_t q = 0; q < numPost; ++q) {
            if (postSpikes[q] <= 0.0f) continue;
            
            size_t idx = p * numPost + q;
            if (idx >= numWeights) continue;
            
            float dt = postSpikes[q] - preSpikes[p];
            if (dt > 0.0f) {
                // LTP with dopamine modulation
                float deltaW = A_plus * std::exp(-dt / tau_plus) * dopamineFactor;
                weights[idx] += deltaW;
            } else {
                // LTD with dopamine modulation (typically reduces LTD under positive dopamine)
                float deltaW = -A_minus * std::exp(dt / tau_minus) * (1.0f - 0.5f * dopamineFactor);
                weights[idx] += deltaW;
            }
            
            // Apply bounds
            weights[idx] = std::clamp(weights[idx], w_min, w_max);
        }
    }
}

// Vectorized STDP implementation (continued)

// STDP for batch processing with multiple time steps
void VectorizedSTDP::batchUpdate(std::vector<std::vector<float>>& weightHistory,
                                const std::vector<float>& preSpikes,
                                const std::vector<float>& postSpikes,
                                size_t numTimeSteps, float timeStep) {
    size_t numPre = preSpikes.size();
    size_t numPost = postSpikes.size();
    
    // Initialize weight history if needed
    if (weightHistory.empty()) {
        weightHistory.resize(numTimeSteps, std::vector<float>(numPre * numPost, 0.0f));
    }
    
    // Process each time step
    for (size_t t = 0; t < numTimeSteps; ++t) {
        // Calculate spike times for this step
        std::vector<float> preSpikeTimes;
        std::vector<float> postSpikeTimes;
        
        // Simplified spike detection (in real implementation would use actual spike times)
        for (size_t p = 0; p < numPre; ++p) {
            if (preSpikes[p] > 0.0f) {
                preSpikeTimes.push_back(static_cast<float>(t) * timeStep + preSpikes[p]);
            }
        }
        
        for (size_t q = 0; q < numPost; ++q) {
            if (postSpikes[q] > 0.0f) {
                postSpikeTimes.push_back(static_cast<float>(t) * timeStep + postSpikes[q]);
            }
        }
        
        // Update weights for this time step
        updateWeights(weightHistory[t], preSpikeTimes, postSpikeTimes, preSpikeTimes.size(), postSpikeTimes.size());
    }
}

// Vectorized STDP implementation (continued)

// Helper function for STDP compatibility
void VectorizedSTDP::legacyUpdate(float* weights, const float* preSpikes,
                                 const float* postSpikes, size_t numPre, size_t numPost) {
    std::vector<float> weightVec(weights, weights + numPre * numPost);
    std::vector<float> preVec(preSpikes, preSpikes + numPre);
    std::vector<float> postVec(postSpikes, postSpikes + numPost);
    
    updateWeights(weightVec, preVec, postVec, numPre, numPost);
    
    std::copy(weightVec.begin(), weightVec.end(), weights);
}size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, 
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
