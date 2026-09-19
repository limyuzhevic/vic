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
    // Real Hebbian learning implementation based on spike covariance
    // 
    // Mathematical formulation (Covariance rule):
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // 
    // For spike-based systems:
    // Δw = η * (coactivity - baseline)
    // 
    // Where:
    //   coactivity = number of correlated pre/post spikes
    //   baseline = learningRate * mean activity
    // 
    // This implements "neurons that fire together, wire together"
    // but with a threshold to prevent runaway potentiation.
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike statistics
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Count correlated spike pairs using precise timing
    size_t correlationCount = 0;
    float totalCorrelation = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_spike = std::abs(postTime - preTime);
            // Count spikes within 20ms as correlated (biological window)
            if (dt_spike < 20.0f) {
                correlationCount++;
                // Weight by inverse distance (closer spikes = stronger correlation)
                totalCorrelation += 1.0f / (1.0f + dt_spike / 10.0f);
            }
        }
    }
    
    // Compute covariance-based weight change
    // Δw = η * (correlation - baseline)
    // where baseline = η * preRate * postRate * timeWindow
    
    float baseline = pImpl->learningRate * preRate * postRate * 0.02f;  // 20ms window
    
    // More sophisticated: use actual correlation strength
    float delta = pImpl->learningRate * totalCorrelation;
    
    // Subtract baseline to get covariance
    delta -= baseline * static_cast<float>(correlationCount);
    
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
