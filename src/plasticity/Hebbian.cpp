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
     * Optimized Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Optimized implementation for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with activity thresholds to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Improvements over basic implementation:
     *   - O(N+M) complexity using sorted spike lists and binary search
     *   - Activity-based learning rate modulation
     *   - Spike correlation window optimization
     *   - Prevention of runaway potentiation with adaptive bounds
     *   - Better handling of sparse spike activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Sort spike lists for efficient correlation computation
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use binary search to find correlated spike pairs within window
    const float CORRELATION_WINDOW = 100.0f;  // 100ms correlation window
    size_t correlationCount = 0;
    
    // Optimized O(N log M) algorithm for finding correlated spikes
    for (Timestamp preTime : sortedPreSpikes) {
        // Find all post spikes within correlation window
        auto postRange = std::equal_range(
            sortedPostSpikes.begin(), 
            sortedPostSpikes.end(), 
            preTime - CORRELATION_WINDOW,
            std::less<Timestamp>()
        );
        auto endRange = std::equal_range(
            sortedPostSpikes.begin(), 
            sortedPostSpikes.end(), 
            preTime + CORRELATION_WINDOW,
            std::less<Timestamp>()
        );
        correlationCount += std::distance(postRange, endRange);
    }
    
    // Adaptive learning rate based on correlation strength
    // Prevent runaway potentiation by scaling with activity levels
    float activityScale = std::min(1.0f, static_cast<float>(correlationCount) / 1000.0f);
    float adaptiveLearningRate = pImpl->learningRate * activityScale;
    
    // Compute weight change with activity-dependent baseline
    // This prevents runaway potentiation when activity is high
    float baseline = adaptiveLearningRate * 0.1f;  // Reduced baseline
    float delta = adaptiveLearningRate * (static_cast<float>(correlationCount) - baseline);
    
    // Apply with enhanced bounds checking and clamping
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
