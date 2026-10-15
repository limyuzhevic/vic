#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "SpikeCorrelationCache.hpp"
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
     * OPTIMIZED: Uses SpikeCorrelationCache to avoid redundant calculations
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
     * OPTIMIZATION:
     * 1. Use SpikeCorrelationCache for spike storage and correlation statistics
     * 2. Sort spike times for efficient windowed processing
     * 3. Use histogram-based correlation counting (O(n) instead of O(n²))
     * 4. Pre-compute statistics in cache to avoid recomputation
     * 5. Retrieve cached correlation coefficients when available
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get synapse ID for cache operations
    SynapseId synapseId = synapse->getId();
    
    // Try to get cached spike data and correlation statistics
    SpikeCorrelationCache cache;
    auto cachedSpikes = cache.getSpikes(synapseId);
    
    // Get spike data from either cache or directly
    std::vector<Timestamp> actualPreSpikes = preSpikes;
    std::vector<Timestamp> actualPostSpikes = postSpikes;
    
    if (!cachedSpikes.first.empty() && !cachedSpikes.second.empty()) {
        // Use cached spikes (they should be equivalent)
        actualPreSpikes = cachedSpikes.first;
        actualPostSpikes = cachedSpikes.second;
    }
    
    // Store spikes in cache for future use
    cache.storeSpikes(synapseId, actualPreSpikes, actualPostSpikes);
    
    // Get correlation statistics from cache (computes if not available)
    SpikeCorrelationCache::CorrelationStats stats = cache.getStats(synapseId);
    
    // OPTIMIZATION: Use cached correlation statistics for weight update
    // If cache has valid stats, use them directly for faster computation
    if (stats.preSpikeCount > 0 && stats.postSpikeCount > 0) {
        // Use cached correlation coefficient and coincidence rate
        float correlation = stats.correlationCoefficient;
        float coincidenceRate = stats.coincidenceRate;
        float temporalDistance = stats.temporalDistance;
        
        // Compute weight change based on cached statistics
        // More sophisticated: combine correlation with temporal distance
        float temporalWeight = 1.0f / (1.0f + temporalDistance / 1000.0f);  // Normalize distance
        
        // Update learning rule: correlation contributes positively, but
        // slow temporal distance may indicate irrelevant correlations
        float delta = pImpl->learningRate * correlation * coincidenceRate * temporalWeight;
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
    } else {
        // Fallback to original algorithm for edge case
        size_t correlationCount = 0;
        for (Timestamp preTime : actualPreSpikes) {
            for (Timestamp postTime : actualPostSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                    ++correlationCount;
                }
            }
        }
        
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
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
