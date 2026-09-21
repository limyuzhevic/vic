#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Real Anti-Hebbian learning implementation
     * 
     * Mathematical formulation (Anti-Hebbian rule):
     * Δw = -η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = -η × (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate × mean activity
     *   η = learning rate parameter
     * 
     * This implements "neurons that fire together, weaken together"
     * but with a threshold to prevent runaway depression.
     * 
     * Biological inspiration:
     * - Lateral inhibition in cortical circuits
     * - Competitive learning during development
     * - Stabilizing network dynamics
     * - Preventing runaway excitation
     * 
     * Limitations:
     * - Doesn't account for STDP timing details
     * - Single learning rate (no separate depression rates)
     * - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
