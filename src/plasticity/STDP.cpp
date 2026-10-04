#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Precomputed exponential values for performance
    float expLTPCache;    // For dt = 0
    float expLTPStep;     // Step size for exponential interpolation
    float expLTPEpsilon;   // Precision threshold
    
    // Sliding window cache for spike pair counting
    std::unordered_map<Timestamp, size_t> preSpikeCache;
    std::unordered_map<Timestamp, size_t> postSpikeCache;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), expLTPCache(1.0f),
             expLTPStep(0.001f), expLTPEpsilon(0.0001f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

// Optimized STDP update using sliding window and precomputed exponentials
void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized spike pair counting using sliding window
    // Process spikes in chronological order for cache efficiency
    size_t minSize = std::min(preSpikes.size(), postSpikes.size());
    
    if (preSpikes.size() <= postSpikes.size()) {
        // Pre-spikes are fewer - iterate over pre-spikes
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            Timestamp preTime = preSpikes[i];
            
            // Find corresponding post-spikes using binary search for sorted spike times
            auto lowerBound = std::lower_bound(postSpikes.begin(), postSpikes.end(), preTime);
            
            // Calculate contributions efficiently
            for (auto it = lowerBound; it != postSpikes.end(); ++it) {
                Timestamp postTime = *it;
                float delta = postTime - preTime;  // Δt (post - pre)
                
                if (delta > 0) {
                    // Pre before post: POTENTIATION
                    totalDelta += pImpl->ltpWeight * std::exp(-delta / tau);
                } else {
                    // Post before pre: DEPRESSION
                    totalDelta -= pImpl->ltdWeight * std::exp(delta / tau);
                }
                
                // Early exit for very small contributions
                if (std::abs(totalDelta) > 1e-3f * pImpl->ltpWeight) {
                    break;
                }
            }
        }
    } else {
        // Post-spikes are fewer - iterate over post-spikes
        for (size_t i = 0; i < postSpikes.size(); ++i) {
            Timestamp postTime = postSpikes[i];
            
            // Find corresponding pre-spikes using binary search
            auto upperBound = std::upper_bound(preSpikes.begin(), preSpikes.end(), postTime);
            
            // Calculate contributions in reverse order
            for (auto it = upperBound; it != preSpikes.begin(); --it) {
                Timestamp preTime = *(--it);
                float delta = postTime - preTime;  // Δt (post - pre)
                
                if (delta > 0) {
                    // Pre before post: POTENTIATION
                    totalDelta += pImpl->ltpWeight * std::exp(-delta / tau);
                } else {
                    // Post before pre: DEPRESSION
                    totalDelta -= pImpl->ltdWeight * std::exp(delta / tau);
                }
                
                if (std::abs(totalDelta) > 1e-3f * pImpl->ltpWeight) {
                    break;
                }
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
