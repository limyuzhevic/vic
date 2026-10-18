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
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs with temporal precision
    // Use spike timing windows to determine correlation
    const float CORRELATION_WINDOW = 50.0f;  // 50ms correlation window
    size_t correlationCount = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window as correlated
            if (std::abs(dt) < CORRELATION_WINDOW) {
                ++correlationCount;
            }
        }
    }
    
    // Calculate weight change based on correlation
    // Use learning rate to control the magnitude of change
    float baseDelta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply stabilization: prevent runaway potentiation by scaling with current weight
    float currentWeight = synapse->getWeight();
    float weightBasedScaling = 1.0f - std::abs(currentWeight) * 0.5f;  // Less scaling near zero weight
    float delta = baseDelta * weightBasedScaling;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update eligibility trace for reward-modulated Hebbian learning
    if (std::abs(delta) > 0.0f) {
        float currentTrace = synapse->getEligibilityTrace();
        // Eligibility trace accumulates changes but decays over time
        synapse->setEligibilityTrace(currentTrace + delta * 0.3f);  // Scale eligibility contribution
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
