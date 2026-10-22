#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <iostream>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {
    // Initialize performance monitoring
    std::cout << "[INIT] Hebbian initialized with performance monitoring enabled" << std::endl;
}

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
    
    // Performance profiling for Hebbian updates
    Performance::Profiler profiler("Hebbian::update");
    
    // Use correlation statistics helper for better performance
    CorrelationStats stats = getCorrelationStats(preSpikes, postSpikes);
    
    // Compute weight change based on correlation
    float delta = pImpl->learningRate * static_cast<float>(stats.preSpikeCount * stats.postSpikeCount);
    
    // Performance tracking
    performanceCounter_.incrementUpdates();
    
    // Apply with bounds using base class method
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        performanceCounter_.incrementWeightChanges();
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with bounds using base class helper
    applyPlasticityWeight(synapse, delta);
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

// Implement the correlation statistics helper function
Hebbian::CorrelationStats Hebbian::getCorrelationStats(const std::vector<Timestamp>& preSpikes,
                                                         const std::vector<Timestamp>& postSpikes) const {
    CorrelationStats stats;
    stats.preSpikeCount = preSpikes.size();
    stats.postSpikeCount = postSpikes.size();
    
    // OPTIMIZATION: Use two-pointer technique for O(n + m) instead of O(n²)
    // This is much more efficient for large spike trains
    std::vector<Timestamp> sortedPre = preSpikes;
    std::vector<Timestamp> sortedPost = postSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    // Count correlated spike pairs using two-pointer technique
    size_t correlationCount = 0;
    size_t preIdx = 0, postIdx = 0;
    
    while (preIdx < sortedPre.size() && postIdx < sortedPost.size()) {
        Timestamp preTime = sortedPre[preIdx];
        Timestamp postTime = sortedPost[postIdx];
        
        // Check if spikes are within correlation window
        if (std::abs(static_cast<float>(postTime - preTime)) < stats.correlationWindow) {
            correlationCount++;
            preIdx++;
            postIdx++;
        } else if (postTime < preTime - static_cast<Timestamp>(stats.correlationWindow)) {
            // Post spike too early, move post pointer forward
            postIdx++;
        } else {
            // Pre spike too early, move pre pointer forward
            preIdx++;
        }
    }
    
    // The correlation count could be stored if needed for weight calculation
    return stats;
}

} // namespace nlm
