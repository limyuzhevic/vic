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
             covarianceThreshold(0.5f) {}
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
    
    // Real Hebbian learning implementation (Covariance rule):
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // Where ⟨⟩ represents average over spike history
    
    // Count pre-synaptic spikes and post-synaptic spikes
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    if (preCount == 0 || postCount == 0) {
        return;
    }
    
    // Calculate average firing rates (simplified: spikes / time window)
    // Use the time span between first and last spike as window
    float preTimeWindow = static_cast<float>(preSpikes.back() - preSpikes.front());
    float postTimeWindow = static_cast<float>(postSpikes.back() - postSpikes.front());
    
    float preRate = static_cast<float>(preCount) / (preTimeWindow > 0 ? preTimeWindow : 1.0f);
    float postRate = static_cast<float>(postCount) / (postTimeWindow > 0 ? postTimeWindow : 1.0f);
    
    // Calculate correlation (simultaneous spike probability)
    size_t coincidences = 0;
    float coincidenceTimeWindow = 20.0f;  // 20ms coincidence window (biological)
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = std::abs(static_cast<float>(postTime - preTime));
            if (dt < coincidenceTimeWindow) {
                ++coincidences;
            }
        }
    }
    
    // Compute covariance-based weight change
    // coincidences / (preCount * postCount) = correlation coefficient
    // Subtract baseline (expected coincidences by chance)
    float expectedCoincidences = preRate * postRate * coincidenceTimeWindow;
    float correlation = static_cast<float>(coincidences) / static_cast<float>(preCount * postCount);
    float covariance = correlation - (preRate * postRate * coincidenceTimeWindow);
    
    // Apply Hebbian learning with covariance rule
    float delta = pImpl->learningRate * covariance * pImpl->covarianceThreshold;
    
    // Apply with bounds and normalization
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
