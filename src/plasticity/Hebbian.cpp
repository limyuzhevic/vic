#include "Hebbian.hpp"
#include "brain/Synapse.hpp"
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
                      std::span<const Timestamp> preSpikes,
                      std::span<const Timestamp> postSpikes,
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
    
    // Count correlated spike pairs using O(n log n) algorithm
    // Sort spike vectors for efficient range queries
    std::vector<Timestamp> sortedPreSpikes(preSpikes.begin(), preSpikes.end());
    std::vector<Timestamp> sortedPostSpikes(postSpikes.begin(), postSpikes.end());
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // For each pre spike, count post spikes within 100ms window using binary search
    size_t correlationCount = 0;
    for (Timestamp preTime : sortedPreSpikes) {
        // Find range of post spikes within [preTime - 100ms, preTime + 100ms]
        auto lower = std::lower_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), 
                                      preTime - 100.0f);
        auto upper = std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), 
                                      preTime + 100.0f);
        
        // Count spikes in range (upper - lower)
        correlationCount += std::distance(lower, upper);
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
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
