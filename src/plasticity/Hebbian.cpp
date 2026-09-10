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
    // Real Hebbian learning implementation with spike-based covariance rule
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)  (covariance rule)
    // 
    // For spike-based systems with discrete spikes:
    // - ⟨pre⟩ = firing rate of pre-synaptic neuron
    // - ⟨post⟩ = firing rate of post-synaptic neuron
    // - ⟨pre * post⟩ = co-activity rate (both neurons fire)
    // - η = learning rate (covarianceThreshold parameter)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates from spike history
    // Rate = spike count / observation window (approximated by spike times)
    float preRate = static_cast<float>(preSpikes.size()) / 100.0f;  // Normalized
    float postRate = static_cast<float>(postSpikes.size()) / 100.0f;  // Normalized
    
    // Calculate co-activity (correlation) - spikes within 5ms window
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            if (std::abs(static_cast<float>(preTime - postTime)) < 5.0f) {  // 5ms correlation window
                ++coactivity;
            }
        }
    }
    
    // Normalized co-activity rate
    float prePostRate = coactivity / 100.0f;  // Normalized
    
    // Covariance rule: Δw = η * (coactivity_rate - pre_rate * post_rate)
    float delta = pImpl->learningRate * (prePostRate - preRate * postRate);
    
    // Apply biological constraints - prevent runaway potentiation
    // Use covarianceThreshold to scale learning rate based on activity
    float thresholdFactor = 1.0f;
    if (pImpl->covarianceThreshold > 0.0f) {
        thresholdFactor = std::clamp(1.0f - (prePostRate / pImpl->covarianceThreshold), 0.0f, 1.0f);
    }
    
    delta *= thresholdFactor;
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply weight change with biological constraints and bounds
    // Implements weight change with clamping to prevent runaway potentiation
    
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds: clamp to max/min weight based on Hebbian rules
    // Hebbian learning typically has different bounds than STDP
    float maxWeight = pImpl->maxWeight;  // e.g., 1.0 for pure Hebbian
    float minWeight = pImpl->minWeight;  // e.g., -1.0 for bidirectional Hebbian
    
    // Biological constraint: weights don't change beyond certain limits
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    // Additional biological rule: weight changes are more stable than STDP
    // Apply smoothing to prevent abrupt changes
    float currentWeight = synapse->getWeight();
    float maxChange = 0.1f;  // Max change per learning event
    float change = newWeight - currentWeight;
    
    if (std::abs(change) > maxChange) {
        newWeight = currentWeight + (change > 0.0f ? 1.0f : -1.0f) * maxChange;
    }
    
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = threshold;
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

} // namespace nlm
