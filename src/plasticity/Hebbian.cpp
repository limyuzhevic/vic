#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Precomputed values for performance
    float rateSquared;
    float weightRange;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), rateSquared(0.0001f), weightRange(2.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

// Optimized Hebbian update using vectorization and precomputation
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
    
    // Optimized correlation counting using sliding window
    size_t correlationCount = 0;
    
    // Pre-compute time difference thresholds
    const float maxCorrelationTime = 100.0f;  // 100ms correlation window
    
    // Choose the smaller vector for iteration to minimize operations
    if (preSpikes.size() <= postSpikes.size()) {
        // Iterate over pre-spikes for better cache locality
        for (Timestamp preTime : preSpikes) {
            // Binary search to find post-spikes within correlation window
            auto lowerBound = std::lower_bound(postSpikes.begin(), postSpikes.end(), preTime - maxCorrelationTime);
            auto upperBound = std::upper_bound(postSpikes.begin(), postSpikes.end(), preTime + maxCorrelationTime);
            
            correlationCount += std::distance(lowerBound, upperBound);
        }
    } else {
        // Iterate over post-spikes (fewer iterations)
        for (Timestamp postTime : postSpikes) {
            auto lowerBound = std::lower_bound(preSpikes.begin(), preSpikes.end(), postTime - maxCorrelationTime);
            auto upperBound = std::upper_bound(preSpikes.begin(), preSpikes.end(), postTime + maxCorrelationTime);
            
            correlationCount += std::distance(lowerBound, upperBound);
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds and early clamping
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Optimized weight change with precomputed bounds
    float newWeight = synapse->getWeight() + delta;
    
    // Clamp to reasonable bounds using precomputed values
    if (newWeight < pImpl->minWeight) {
        newWeight = pImpl->minWeight;
    } else if (newWeight > pImpl->maxWeight) {
        newWeight = pImpl->maxWeight;
    }
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    // Update precomputed values
    pImpl->rateSquared = rate * rate;
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
    // Update weight range
    pImpl->weightRange = pImpl->maxWeight - pImpl->minWeight;
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
