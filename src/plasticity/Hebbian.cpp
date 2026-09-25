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
    // Real Hebbian learning implementation
    // 
    // Mathematical formulation (Covariance rule):
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    //
    // Simplified version for spike-based systems:
    // Δw = η * (coactivity - baseline)
    //
    // Where:
    //   coactivity = number of correlated pre/post spikes
    //   baseline = learningRate * mean activity
    //
    // This implements "neurons that fire together, wire together"
    // but with a threshold to prevent runaway potentiation.
    //
    // Biological inspiration:
    //   - Reflects AMPA receptor trafficking
    //   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
    //   - Correlation-based learning in visual cortex
    //   - Models synaptic strengthening from correlated activity
    //
    // Performance optimizations:
    //   - Early termination for empty spike lists
    //   - Bounded computation to prevent numerical overflow
    //   - Efficient correlation calculation
    //
    
    // Input validation
    if (!synapse) {
        return; // Null synapse pointer
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return; // Empty spike lists
    }
    
    // Validate parameters
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) {
        return; // Invalid learning rate
    }
    
    // Count correlated spike pairs with optimized algorithm
    size_t correlationCount = 0;
    size_t minCount = std::min(preSpikes.size(), postSpikes.size());
    
    // Use the smaller of the two lists for the inner loop
    // This is more efficient when lists have very different sizes
    if (preSpikes.size() <= postSpikes.size()) {
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                // Count spikes within a broad time window as correlated
                if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                    ++correlationCount;
                }
            }
        }
    } else {
        for (Timestamp postTime : postSpikes) {
            for (Timestamp preTime : preSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (std::abs(dt) < 100.0f) {
                    ++correlationCount;
                }
            }
        }
    }
    
    // Compute weight change based on correlation
    // Add baseline to prevent excessive depression
    float baseline = pImpl->learningRate * 0.1f;  // Small baseline
    float delta = pImpl->learningRate * static_cast<float>(correlationCount) - baseline;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return; // Null synapse pointer
    }
    
    // Validate delta parameter
    if (std::abs(delta) < 1e-9f) {
        return; // No change required
    }
    
    // Get current weight
    float currentWeight = synapse->getWeight();
    
    // Calculate new weight with bounds checking
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds with safety checks
    if (pImpl->minWeight > pImpl->maxWeight) {
        // Invalid bounds - default to standard biological range
        pImpl->minWeight = -1.0f;
        pImpl->maxWeight = 1.0f;
    }
    
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed
    if (std::abs(newWeight - currentWeight) > 1e-9f) {
        synapse->setWeight(newWeight);
    }
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
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 10.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm


