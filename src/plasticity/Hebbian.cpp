#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
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
    // Implement real Hebbian learning based on spike correlation
    // This follows the "neurons that fire together, wire together" principle

    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate Hebbian weight change based on covariance of pre/post activity
    // Δw = η * (⟨pre*post⟩ - ⟨pre⟩⟨post⟩) - simplified as:
    // Δw = η * (coactivity - baseline)
    
    // Count correlated spike pairs within a window
    size_t correlatedPairs = 0;
    const float correlationWindow = 20.0f;  // 20ms window for correlation
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(postTime - preTime);
            if (std::abs(timeDiff) <= correlationWindow) {
                correlatedPairs++;
            }
        }
    }
    
    // Compute baseline (expected coactivity without correlation)
    // This prevents runaway potentiation
    float baseline = pImpl->learningRate * 0.1f;  // Small baseline
    
    // Compute Hebbian weight change
    float delta = pImpl->learningRate * (static_cast<float>(correlatedPairs) - baseline);
    
    // Apply with bounds to prevent instability
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm