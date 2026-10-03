#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;      // η - learning rate for Hebbian plasticity
    float maxWeight;         // Maximum synaptic weight
    float minWeight;         // Minimum synaptic weight
    float covarianceThreshold;  // Threshold for covariance rule
    float baselineWeight;    // Baseline weight for normalization
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), baselineWeight(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Hebbian Plasticity Implementation
     * 
     * Biological inspiration:
     *   - Implements "neurons that fire together, wire together" principle
     *   - Reflects AMPA receptor trafficking at synapses
     *   - Models early developmental synaptic strengthening
     *   - Based on classical Hebbian learning in hippocampus
     *   
     * Mathematical formulation (covariance rule):
     *   Δw = η * (⟨x*t⟩ - ⟨x⟩⟨t⟩)
     *   
     * Simplified version for spike-based systems:
     *   Δw = η * (correlation - baseline)
     *   
     * Where:
     *   correlation = number of spike coincidences
     *   baseline = learningRate * average activity
     *   η = learningRate
     *   
     * This is a correlation-based Hebbian rule that prevents runaway
     * potentiation through normalization and baseline subtraction.
     * 
     * Limitations:
     *   - Doesn't account for spike timing details (unlike STDP)
     *   - Single learning rate (no separate LTP/LTD rates)
     *   - Assumes stationary statistics over learning window
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate correlation between pre- and post-synaptic activity
    // Count spikes within a broad time window as correlated
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Consider spikes within 50ms as correlated
            if (std::abs(dt) < 50.0f) {
                ++correlationCount;
            }
        }
    }
    
    // Calculate baseline activity (expected coincidences by chance)
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float expectedCoincidences = preRate * postRate * 100.0f;  // 100ms window
    
    // Compute weight change using covariance rule
    float correlation = static_cast<float>(correlationCount);
    float baseline = expectedCoincidences;
    float delta = pImpl->learningRate * (correlation - baseline);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds with saturation
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Add weight normalization to prevent runaway potentiation
    // Implement homeostatic scaling
    if (newWeight > 0.9f * pImpl->maxWeight) {
        // Scale down if approaching maximum
        newWeight *= 0.95f;
    } else if (newWeight < 0.9f * pImpl->minWeight) {
        // Scale up if approaching minimum
        newWeight *= 1.05f;
    }
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.5f, 5.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm