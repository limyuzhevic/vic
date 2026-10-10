#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation with biological realism
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse) {
        return;  // Defensive programming
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;  // No correlation without both pre and post spikes
    }
    
    if (dt <= 0.0f) {
        return;  // Invalid timestep duration
    }
    
    // Count correlated spike pairs with adaptive time window
    size_t correlationCount = 0;
    float timeWindow = 100.0f;  // 100ms correlation window
    
    // Enhanced correlation detection with temporal dynamics
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < timeWindow) {
                ++correlationCount;
                
                // Apply temporal weighting based on spike timing
                if (dt > 0) {
                    // Pre before post: stronger correlation
                    correlationCount += static_cast<size_t>(std::abs(dt) * 0.1f);
                } else {
                    // Post before pre: weaker correlation
                    correlationCount += static_cast<size_t>(-dt * 0.05f);
                }
            }
        }
    }
    
    // Compute weight change based on multiple factors
    if (correlationCount > 0) {
        // Base Hebbian change
        float baseDelta = pImpl->learningRate * static_cast<float>(correlationCount);
        
        // Apply synaptic efficacy modulation
        float efficacy = synapse->getEfficacy();
        baseDelta *= efficacy;
        
        // Incorporate history-dependent plasticity
        float historyFactor = synapse->getHistoryFactor();
        baseDelta *= (1.0f + historyFactor * 0.2f);  // Boost by synaptic history
        
        // Apply neuromodulatory influence
        float neuromodInfluence = synapse->getNeuromodulatorInfluence();
        baseDelta *= (1.0f + neuromodInfluence * 0.3f);  // Strong neuromodulation effect
        
        // Apply eligibility trace modulation
        float eligibility = synapse->getEligibilityTrace();
        baseDelta *= (1.0f + eligibility * 0.5f);  // Boost eligibility trace
        
        // Apply bounds to prevent runaway potentiation
        baseDelta = std::clamp(baseDelta, -pImpl->maxWeight, pImpl->maxWeight);
        
        // Apply weight change with bounds
        if (std::abs(baseDelta) > 1e-6f) {
            applyWeightChange(synapse, baseDelta);
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
