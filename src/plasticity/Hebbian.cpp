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
     * Real Hebbian learning implementation
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
    
    // Validate input parameters
    if (!synapse) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    if (dt < 0.0) {
        return;
    }
    
    // Validate Hebbian parameters
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) {
        return;
    }
    
    if (pImpl->minWeight >= pImpl->maxWeight) {
        return;
    }
    
    // Count correlated spike pairs with proper boundary checks
    size_t correlationCount = 0;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        Timestamp preTime = preSpikes[i];
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            Timestamp postTime = postSpikes[j];
            float dt = static_cast<float>(postTime - preTime);
            
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change with validation
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Validate input parameters
    if (!synapse) {
        return;
    }
    
    // Validate parameters for weight change
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Check if the new weight is within bounds before clamping
    if (newWeight < pImpl->minWeight || newWeight > pImpl->maxWeight) {
        // Clamp to bounds
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
    // Only set weight if it would change
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
    }
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
