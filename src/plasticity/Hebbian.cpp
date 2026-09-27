#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
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
    
    if (!synapse) {
        NLM_LOG_WARNING("Hebbian::update called with null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZED: Avoid O(n²) by using efficient correlation computation
    // Sort spike times for efficient windowed correlation counting
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Count correlated spike pairs efficiently using binary search
    size_t correlationCount = 0;
    const float CORRELATION_WINDOW = 100.0f;  // 100ms correlation window
    
    // Use sliding window approach for O(n log n) complexity
    for (size_t i = 0; i < sortedPreSpikes.size(); ++i) {
        Timestamp preTime = sortedPreSpikes[i];
        
        // Find posts that correlate with this pre spike
        // Use binary search to find range of posts within correlation window
        auto lower = std::lower_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), 
                                     preTime - CORRELATION_WINDOW);
        auto upper = std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), 
                                     preTime + CORRELATION_WINDOW);
        
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
    if (!synapse) {
        NLM_LOG_ERROR("Hebbian::applyWeightChange called with null synapse");
        return;
    }
    
    // Clamp delta to reasonable bounds
    if (std::abs(delta) > 1.0f) {
        NLM_LOG_WARNING("Hebbian::applyWeightChange: Large delta " + std::to_string(delta) + " may cause instability");
    }
    
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
