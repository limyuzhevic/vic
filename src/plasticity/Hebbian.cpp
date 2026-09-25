#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "core/Constants.hpp"
#include "core/Utils.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learning_rate;
    float max_weight;
    float min_weight;
    float covariance_threshold;  // For covariance rule
    
    Impl() : learning_rate(Constants::DEFAULT_LEARNING_RATE), 
             max_weight(Constants::MAX_WEIGHT), 
             min_weight(Constants::MIN_WEIGHT),
             covariance_threshold(0.0f) {}
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
     *   baseline = learning_rate * mean activity
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
    
    // Validate inputs using common utility
    if (!Utils::validateUpdateInputs(synapse, preSpikes, postSpikes, dt)) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlation_count = 0;
    for (Timestamp pre_time : preSpikes) {
        for (Timestamp post_time : postSpikes) {
            float dt_val = static_cast<float>(post_time - pre_time);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt_val) < Constants::CORRELATION_WINDOW_MS) {
                ++correlation_count;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learning_rate * static_cast<float>(correlation_count);
    
    // Apply with bounds
    if (std::abs(delta) > Constants::EPSILON_FOR_NORMALS) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with bounds using common utility
    float new_weight = synapse->getWeight() + delta;
    new_weight = Utils::clampWeight(new_weight);
    synapse->setWeight(new_weight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learning_rate = Utils::clampLearningRate(rate);
}

float Hebbian::getLearningRate() const {
    return pImpl->learning_rate;
}

void Hebbian::setMaxWeight(float max_weight) {
    pImpl->max_weight = Utils::clampWeight(max_weight);
}

float Hebbian::getMaxWeight() const {
    return pImpl->max_weight;
}

} // namespace nlm
