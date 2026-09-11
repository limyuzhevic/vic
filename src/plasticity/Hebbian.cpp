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
    
    // OPTIMIZATION: Sort spikes and use time-window correlation
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use time-based correlation instead of O(n²) pairwise comparison
    // This is more biologically realistic - correlations happen within specific time windows
    const float correlationWindow = 50.0f;  // 50ms window for correlation
    
    // For each pre-synaptic spike, find post-synaptic spikes within correlation window
    float totalCorrelation = 0.0f;
    for (size_t i = 0; i < sortedPreSpikes.size(); ++i) {
        Timestamp preTime = sortedPreSpikes[i];
        
        // Find post spikes in time window [preTime, preTime + correlationWindow]
        auto postStartIt = std::lower_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime);
        auto postEndIt = std::upper_bound(postStartIt, sortedPostSpikes.end(), preTime + correlationWindow);
        
        // Count correlated post spikes
        size_t correlatedPostCount = std::distance(postStartIt, postEndIt);
        totalCorrelation += static_cast<float>(correlatedPostCount);
    }
    
    // Also count reverse correlation (post before pre)
    float totalReverseCorrelation = 0.0f;
    for (size_t i = 0; i < sortedPostSpikes.size(); ++i) {
        Timestamp postTime = sortedPostSpikes[i];
        
        // Find pre spikes in time window [postTime, postTime + correlationWindow]
        auto preStartIt = std::lower_bound(sortedPreSpikes.begin(), sortedPreSpikes.end(), postTime);
        auto preEndIt = std::upper_bound(preStartIt, sortedPreSpikes.end(), postTime + correlationWindow);
        
        // Count correlated pre spikes
        size_t correlatedPreCount = std::distance(preStartIt, preEndIt);
        totalReverseCorrelation += static_cast<float>(correlatedPreCount);
    }
    
    // Apply covariance-based Hebbian learning
    // Δw = η * (coactivity - baseline) where coactivity = correlated spikes
    float coactivity = totalCorrelation;
    float baseline = pImpl->learningRate * (totalCorrelation + totalReverseCorrelation) * 0.5f;
    float delta = pImpl->learningRate * (coactivity - baseline);
    
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
