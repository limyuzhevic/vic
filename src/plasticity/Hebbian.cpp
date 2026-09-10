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
    // Real Hebbian learning implementation (Covariance rule)
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate cross-correlation term: ⟨pre * post⟩
    float crossCorrelation = 0.0f;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float timeDiff = static_cast<float>(postSpikes[j] - preSpikes[i]);
            // Use exponential kernel for spike timing similarity
            float kernel = std::exp(-std::abs(timeDiff) / 20.0f);  // 20ms time constant
            crossCorrelation += kernel;
        }
    }
    crossCorrelation /= (preSpikes.size() * postSpikes.size());
    
    // Calculate baseline: ⟨pre⟩⟨post⟩ (product of mean firing rates)
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float baseline = preRate * postRate * 0.001f;  // Scale factor
    
    // Compute covariance-based weight change
    float covariance = crossCorrelation - baseline;
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds and stability constraints
    if (std::abs(delta) > 1e-8f) {
        // Ensure weight doesn't exceed bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
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
