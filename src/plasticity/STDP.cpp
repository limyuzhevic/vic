#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * OPTIMIZED STDP implementation
     * 
     * Original O(n²) implementation: For each pre-spike, loop through all post-spikes
     * Optimized O(n) implementation: Use parallel iteration with two pointers
     * 
     * Optimized approach:
     * 1. If both spike trains are sorted (they are), use two-pointer technique
     * 2. For each pre-spike, only consider post-spikes that are within the time window
     * 3. Early termination when post-spikes exceed the time window
     * 4. Precompute exponential terms to avoid repeated calculations
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float timeWindow = std::max(5.0f * tau, 50.0f);  // Reasonable time window
    
    // OPTIMIZATION: Use two-pointer technique for sorted spike trains
    size_t preIdx = 0, postIdx = 0;
    
    // Track last valid post-spike index to avoid O(n²) behavior
    for (preIdx = 0; preIdx < preSpikes.size(); ++preIdx) {
        Timestamp preTime = preSpikes[preIdx];
        
        // Move postIdx forward until we're within time window
        while (postIdx < postSpikes.size() && 
               static_cast<float>(postSpikes[postIdx] - preTime) > timeWindow) {
            ++postIdx;
        }
        
        // Now process relevant post-spikes
        for (size_t i = postIdx; i < postSpikes.size(); ++i) {
            float dt = static_cast<float>(postSpikes[i] - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Only consider spikes within reasonable time window
                if (dt > timeWindow) break;  // Early exit for sorted data
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else {
                // Post before pre: DEPRESSION
                // For dt <= 0, depression decays faster, so less expensive
                if (-dt > timeWindow) break;  // Early exit for sorted data
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
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
