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
     * Enhanced Real STDP implementation based on spike-timing correlation
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
     * Enhanced with:
     * - Spike rate normalization for stability
     * - Activity-dependent modulation
     * - Eligibility trace integration
     * - Sophisticated biological plausibility
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Convert spike times to float for efficient processing
    std::vector<float> preTimes;
    std::vector<float> postTimes;
    preTimes.reserve(preSpikes.size());
    postTimes.reserve(postSpikes.size());
    
    for (Timestamp pre : preSpikes) {
        preTimes.push_back(static_cast<float>(pre));
    }
    for (Timestamp post : postSpikes) {
        postTimes.push_back(static_cast<float>(post));
    }
    
    // Optimized STDP calculation using sorted spike times and binary search
    std::sort(preTimes.begin(), preTimes.end());
    std::sort(postTimes.begin(), postTimes.end());
    
    // Process each pre spike and find relevant post spikes efficiently
    for (float preTime : preTimes) {
        // Find post spikes that occurred within a reasonable time window (5*tau)
        auto postItLow = std::lower_bound(postTimes.begin(), postTimes.end(), preTime);
        auto postItHigh = std::upper_bound(postTimes.begin(), postTimes.end(), preTime + 5.0f * tau);
        
        // Process all post spikes in the window
        for (auto postIt = postItLow; postIt != postItHigh; ++postIt) {
            float postTime = *postIt;
            float dt = postTime - preTime;  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION (LTP)
                // "Cells that fire together, wire together" - but only if pre fires before post
                // Biological: NMDA receptor activation when postsynaptic depolarization occurs
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < -0.1f * tau) {  // Ignore very small negative dt (simultaneous)
                // Post before pre: DEPRESSION (LTD)
                // "Anti-Hebbian" - connection weakens if post fires without pre
                // Biological: Backpropagating action potentials causing depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
            }
        }
    }
    
    // Calculate activity normalization factor for stability
    float totalActivity = static_cast<float>(preSpikes.size() + postSpikes.size());
    float activityFactor = 1.0f;
    if (totalActivity > 0.0f) {
        activityFactor = std::min(1.0f, totalActivity / 100.0f);  // Cap at 100 spikes
    }
    
    // Apply activity-dependent modulation
    totalDelta *= activityFactor;
    
    // Apply weight change with bounds and eligibility trace integration
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.1f);  // Smaller contribution to trace
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return; // Defensive programming - never dereference null
    }
    
    // Clamp weight to bounds with additional safety checks
    float newWeight = synapse->getWeight() + delta;
    
    // Enhanced bounds checking with stability constraints
    float maxAllowedWeight = pImpl->maxWeight;
    float minAllowedWeight = pImpl->minWeight;
    
    // Additional stability constraints to prevent runaway effects
    if (std::abs(newWeight) > 5.0f) {
        // Cap extreme weights that could cause instability
        maxAllowedWeight = std::min(maxAllowedWeight, 5.0f);
        minAllowedWeight = std::max(minAllowedWeight, -5.0f);
    }
    
    newWeight = std::clamp(newWeight, minAllowedWeight, maxAllowedWeight);
    synapse->setWeight(newWeight);
    
    // Update eligibility trace (if this is part of a larger operation)
    // This would be handled in the main update function
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
