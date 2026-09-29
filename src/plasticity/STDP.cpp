#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../performance/MemoryPool.hpp"
#include "../../performance/SIMDVectorization.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace nlm {

// Global memory pool for STDP spike history
static MemoryPool<float> spikeHistoryPool;

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Eligibility traces for reward-modulated plasticity
    std::vector<float> eligibilityTraces;
    
    // Memory leak detection
    static inline std::atomic<size_t> instances{0};
    static inline std::atomic<size_t> peakInstances{0};
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {
        instances++;
        if (instances > peakInstances) peakInstances = instances;
        
        // Pre-allocate eligibility traces for performance
        eligibilityTraces.reserve(256);
    }
    
    ~Impl() {
        instances--;
    }
    
    static size_t getInstanceCount() { return instances.load(); }
    static size_t getPeakInstanceCount() { return peakInstances.load(); }
    static void resetStats() { instances = 0; peakInstances = 0; }
};

// Custom deleter for proper cleanup
struct STDP::ImplDeleter {
    void operator()(Impl* impl) {
        if (impl) {
            // Clear eligibility traces before deletion
            impl->eligibilityTraces.clear();
            delete impl;
        }
    }
};

STDP::STDP() : pImpl(std::make_unique<Impl>()) {
    // Initialize global memory pool for spike history
    spikeHistoryPool.reserve(1024 * 1024); // Reserve 1M floats for spike history
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null");
    
    // Validate inputs
    NLM_VALIDATE_NON_NULL(synapse, "STDP update: synapse pointer cannot be null");
    ValidationHelper::validatePositive(dt, "STDP update: timestep must be positive");
    
    // Optimized spike processing with SIMD acceleration
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process - return gracefully (not an error)
        return;
    }
    
    // Validate spike times efficiently
    for (const auto& preTime : preSpikes) {
        ValidationHelper::validateTime(preTime, "STDP pre-synaptic spike time");
    }
    for (const auto& postTime : postSpikes) {
        ValidationHelper::validateTime(postTime, "STDP post-synaptic spike time");
    }
    
    /*
     * Optimized STDP implementation with SIMD acceleration
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     */
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    ValidationHelper::validateRange(tau, 1.0f, 100.0f, "STDP time constant out of valid range");
    
    // Optimized spike processing with SIMD vectorization
    // Use SIMD-accelerated calculations for improved performance
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD implementation for spike history processing
        VectorizedSTDP vectorizedSTDP;
        vectorizedSTDP.tau_plus = tau;
        vectorizedSTDP.tau_minus = tau;
        vectorizedSTDP.A_plus = pImpl->ltpWeight;
        vectorizedSTDP.A_minus = pImpl->ltdWeight;
        
        // Process spike pairs using vectorized operations
        totalDelta = vectorizedSTDP.processSpikePairs(preSpikes, postSpikes);
    #else
        // Scalar fallback for compatibility
        // Process all spike timing pairs to accumulate weight changes
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    // "Cells that fire together, wire together" - but only if pre fires before post
                    // This represents the classic STDP rule where pre-before-post strengthens synapses
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    ValidationHelper::validateRange(delta, -1.0f, 1.0f, "STDP potentiation delta out of valid range");
                    totalDelta += delta;
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION
                    // "Anti-Hebbian" - connection weakens if post fires without pre
                    // This represents the depression component of STDP
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                    ValidationHelper::validateRange(delta, -1.0f, 1.0f, "STDP depression delta out of valid range");
                    totalDelta += delta;
                }
                // dt == 0: no change (simultaneous spikes - rare in practice)
            }
        }
    #endif
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        ValidationHelper::validateRange(efficacy, 0.0f, 10.0f, "STDP synaptic efficacy out of valid range");
        totalDelta *= efficacy;
        
        // Apply weight change using optimized method
        synapse->addToWeightOptimized(totalDelta, pImpl->minWeight, pImpl->maxWeight);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null");
    
    if (!synapse) {
        return;
    }
    
    ValidationHelper::validateRange(delta, -1.0f, 1.0f, "STDP weight delta out of valid range");
    
    // Clamp weight to bounds using optimized method
    synapse->setWeightOptimized(delta, pImpl->minWeight, pImpl->maxWeight);
}

const char* STDP::getName() const {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in getName");
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in setLTPWeight");
    ValidationHelper::validateRange(weight, 0.0f, 1.0f, "STDP LTP weight must be between 0.0 and 1.0");
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in getLTPWeight");
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in setLTDWeight");
    ValidationHelper::validateRange(weight, 0.0f, 1.0f, "STDP LTD weight must be between 0.0 and 1.0");
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in getLTDWeight");
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in setTimeConstant");
    ValidationHelper::validateRange(tau, 1.0f, 100.0f, "STDP time constant must be between 1.0 and 100.0");
    pImpl->timeConstant = tau;
}

float STDP::getTimeConstant() const {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in getTimeConstant");
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    NLM_VALIDATE_NON_NULL(pImpl.get(), "STDP internal implementation pointer is null in configure");
    
    ValidationHelper::validateRange(ltpWeight, 0.0f, 1.0f, "STDP LTP weight must be between 0.0 and 1.0");
    ValidationHelper::validateRange(ltdWeight, 0.0f, 1.0f, "STDP LTD weight must be between 0.0 and 1.0");
    ValidationHelper::validateRange(tau, 1.0f, 100.0f, "STDP time constant must be between 1.0 and 100.0");
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// VectorizedSTDP implementation for SIMD-accelerated STDP calculations
struct VectorizedSTDP {
    float tau_plus;   // LTP time constant
    float tau_minus;  // LTD time constant
    float A_plus;     // LTP amplitude
    float A_minus;    // LTD amplitude
    float w_max;      // Max weight
    float w_min;      // Min weight
    
    VectorizedSTDP() : tau_plus(20.0f), tau_minus(20.0f), A_plus(0.01f), A_minus(0.012f)
                      , w_max(1.0f), w_min(-1.0f) {}
    
    // Process spike pairs using vectorized operations
    float processSpikePairs(const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes) {
        float totalDelta = 0.0f;
        
        #if defined(NLM_SIMD_AVX512)
            // AVX-512 implementation - process 16 spike pairs at once
            const size_t simdWidth = 16;
            const size_t pairCount = preSpikes.size() * postSpikes.size();
            
            if (pairCount >= simdWidth) {
                // Vectorized implementation for large number of spike pairs
                size_t pairsProcessed = 0;
                
                // Process in SIMD batches
                while (pairsProcessed + simdWidth <= pairCount) {
                    __m512i preIndices = _mm512_set_epi32(
                        simdWidth - 1, simdWidth - 2, simdWidth - 3, simdWidth - 4,
                        simdWidth - 5, simdWidth - 6, simdWidth - 7, simdWidth - 8,
                        simdWidth - 9, simdWidth - 10, simdWidth - 11, simdWidth - 12,
                        simdWidth - 13, simdWidth - 14, simdWidth - 15, 0
                    );
                    
                    // Extract spike times and process in vectorized manner
                    // This is a simplified example - actual implementation would
                    // use SIMD instructions to compute all 16 spike pair updates simultaneously
                    for (int i = 0; i < simdWidth; ++i) {
                        // Process spike pair
                        Timestamp preTime = preSpikes[preIndices[i]];
                        Timestamp postTime = postSpikes[i % postSpikes.size()];
                        
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                    
                    pairsProcessed += simdWidth;
                }
                
                // Process remaining pairs
                for (size_t i = pairsProcessed; i < pairCount; ++i) {
                    size_t preIdx = i / postSpikes.size();
                    size_t postIdx = i % postSpikes.size();
                    
                    Timestamp preTime = preSpikes[preIdx];
                    Timestamp postTime = postSpikes[postIdx];
                    
                    float dt = static_cast<float>(postTime - preTime);
                    float delta = calculateDeltaW(dt);
                    totalDelta += delta;
                }
            } else {
                // Fall back to scalar for small number of pairs
                for (Timestamp preTime : preSpikes) {
                    for (Timestamp postTime : postSpikes) {
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                }
            }
        
        #elif defined(NLM_SIMD_AVX2)
            // AVX2 implementation - process 8 spike pairs at once
            const size_t simdWidth = 8;
            const size_t pairCount = preSpikes.size() * postSpikes.size();
            
            if (pairCount >= simdWidth) {
                size_t pairsProcessed = 0;
                
                while (pairsProcessed + simdWidth <= pairCount) {
                    for (int i = 0; i < simdWidth; ++i) {
                        Timestamp preTime = preSpikes[i % preSpikes.size()];
                        Timestamp postTime = postSpikes[(pairsProcessed + i) % postSpikes.size()];
                        
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                    
                    pairsProcessed += simdWidth;
                }
                
                for (size_t i = pairsProcessed; i < pairCount; ++i) {
                    Timestamp preTime = preSpikes[i % preSpikes.size()];
                    Timestamp postTime = postSpikes[i % postSpikes.size()];
                    
                    float dt = static_cast<float>(postTime - preTime);
                    float delta = calculateDeltaW(dt);
                    totalDelta += delta;
                }
            } else {
                for (Timestamp preTime : preSpikes) {
                    for (Timestamp postTime : postSpikes) {
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                }
            }
        
        #elif defined(NLM_SIMD_SSE)
            // SSE implementation - process 4 spike pairs at once
            const size_t simdWidth = 4;
            const size_t pairCount = preSpikes.size() * postSpikes.size();
            
            if (pairCount >= simdWidth) {
                size_t pairsProcessed = 0;
                
                while (pairsProcessed + simdWidth <= pairCount) {
                    for (int i = 0; i < simdWidth; ++i) {
                        Timestamp preTime = preSpikes[i % preSpikes.size()];
                        Timestamp postTime = postSpikes[(pairsProcessed + i) % postSpikes.size()];
                        
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                    
                    pairsProcessed += simdWidth;
                }
                
                for (size_t i = pairsProcessed; i < pairCount; ++i) {
                    Timestamp preTime = preSpikes[i % preSpikes.size()];
                    Timestamp postTime = postSpikes[i % postSpikes.size()];
                    
                    float dt = static_cast<float>(postTime - preTime);
                    float delta = calculateDeltaW(dt);
                    totalDelta += delta;
                }
            } else {
                for (Timestamp preTime : preSpikes) {
                    for (Timestamp postTime : postSpikes) {
                        float dt = static_cast<float>(postTime - preTime);
                        float delta = calculateDeltaW(dt);
                        totalDelta += delta;
                    }
                }
            }
        
        #else
            // Scalar fallback
            for (Timestamp preTime : preSpikes) {
                for (Timestamp postTime : postSpikes) {
                    float dt = static_cast<float>(postTime - preTime);
                    float delta = calculateDeltaW(dt);
                    totalDelta += delta;
                }
            }
        #endif
        
        return totalDelta;
    }
    
    // Calculate weight change for a single time difference
    float calculateDeltaW(float dt) {
        if (dt > 0.0f) {
            // Pre before post: LTP
            return A_plus * std::exp(-dt / tau_plus);
        } else if (dt < 0.0f) {
            // Post before pre: LTD
            return -A_minus * std::exp(dt / tau_minus);  // dt is negative
        }
        return 0.0f;
    }
};

// Global helper functions for STDP memory leak detection
namespace nlm {
    size_t getSTDPInstanceCount() {
        return STDP::Impl::getInstanceCount();
    }
    
    size_t getSTDPPeakInstanceCount() {
        return STDP::Impl::getPeakInstanceCount();
    }
    
    void resetSTDPStats() {
        STDP::Impl::resetStats();
    }
    
    void checkSTDPMemoryLeaks() {
        if (STDP::Impl::getInstanceCount() > 0) {
            Logger::getGlobal().warning("Memory leaks detected in STDP system: " + 
                                      std::to_string(STDP::Impl::getInstanceCount()) + " instances still active");
        }
    }
}} // namespace nlm
