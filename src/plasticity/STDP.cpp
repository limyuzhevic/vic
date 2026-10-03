#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm;

// Memory pool for STDP spike timing
struct STDPMemoryPool {
    static constexpr size_t MAX_SPIKE_PAIRS = 10000;  // Maximum pairs to process
    static constexpr float MIN_EXP_RESULT = 1e-6f;   // Minimum exp result to avoid division by zero
    
    // Pre-allocated temporary buffer for spike pair processing
    float tempPairs[MAX_SPIKE_PAIRS];
    size_t tempPairsSize;
    
    // Precomputed exponential values for faster STDP calculations
    static constexpr size_t EXP_CACHE_SIZE = 1000;
    float expCache[EXP_CACHE_SIZE];
    
    STDPMemoryPool() : tempPairsSize(0) {
        // Initialize exponential cache
        for (size_t i = 0; i < EXP_CACHE_SIZE; ++i) {
            expCache[i] = std::exp(static_cast<float>(i) / 100.0f);  // Scale for common tau values
        }
    }
};

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Use memory pool for temporary storage
    STDPMemoryPool memoryPool;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

// Fast exponential approximation for STDP calculations
inline float fastExp(float x, float tau) {
    // Use lookup table for common values
    if (tau < 1.0f) return std::exp(-x / tau);
    if (tau > 100.0f) return std::exp(-x / tau);
    
    // For tau in [1, 100], use precomputed values
    int index = static_cast<int>(x * 10.0f / tau) % STDPMemoryPool::EXP_CACHE_SIZE;
    if (index < 0) index += STDPMemoryPool::EXP_CACHE_SIZE;
    return STDPMemoryPool::EXP_CACHE_SIZE > 0 ? STDPMemoryPool::EXP_CACHE_SIZE : std::exp(-x / tau);
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Cache frequently accessed values
    float ltp = pImpl->ltpWeight;
    float ltd = pImpl->ltdWeight;
    
    // Optimized STDP calculation using spatial and temporal locality
    // Sort spikes for better cache performance and faster windowing
    auto sortedPreSpikes = preSpikes;
    auto sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use two-pointer technique for O(n + m) complexity instead of O(n*m)
    size_t preIdx = 0, postIdx = 0;
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        float preTime = static_cast<float>(sortedPreSpikes[preIdx]);
        float postTime = static_cast<float>(sortedPostSpikes[postIdx]);
        float dt = postTime - preTime;
        
        if (dt > 0.0f) {
            // Pre before post: POTENTIATION
            // Only calculate for small dt values (most significant contributions)
            if (dt <= tau * 4.0f) {  // Beyond 4*tau, exp(-4) ≈ 0.018, negligible contribution
                float delta = ltp * std::exp(-dt / tau);
                totalDelta += delta;
            }
            postIdx++;
        } else {
            // Post before pre: DEPRESSION
            // Use absolute value for calculation
            float delta = ltd * std::exp(dt / tau);  // dt is negative
            totalDelta -= delta;  // Subtract for depression
            preIdx++;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
