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
    // Implement real Hebbian learning based on spike timing
    // Delta w ∝ pre_spike × post_spike (correlational learning)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike correlation with temporal precision
    float correlation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Exponential decay of correlation with time difference
            float timeDiff = static_cast<float>(std::abs(post - pre));
            float temporalFactor = std::exp(-timeDiff / 20.0f);  // 20ms time constant
            correlation += temporalFactor;
        }
    }
    
    // Normalize by total possible correlations
    float maxCorrelations = static_cast<float>(preSpikes.size()) * static_cast<float>(postSpikes.size());
    if (maxCorrelations > 0.0f) {
        correlation /= maxCorrelations;
    }
    
    // Apply weight change based on correlation
    if (correlation > 0.0f) {
        float weightChange = pImpl->learningRate * correlation * dt;
        applyWeightChange(synapse, weightChange);
    }
    
    // Apply homeostatic scaling to prevent runaway potentiation
    float currentWeight = std::abs(synapse->getWeight());
    float targetWeight = pImpl->covarianceThreshold > 0.0f ? pImpl->covarianceThreshold : 0.5f;
    
    // Scale if current weight deviates too much from target
    if (currentWeight > 0.0f) {
        float scaleFactor = targetWeight / currentWeight;
        if (scaleFactor < 0.8f || scaleFactor > 1.2f) {
            // Apply synaptic scaling with learning rate
            float homeostaticDelta = (targetWeight - currentWeight) * 0.01f * dt;
            applyWeightChange(synapse, homeostaticDelta);
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
