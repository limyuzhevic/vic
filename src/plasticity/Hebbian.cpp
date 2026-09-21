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
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Real Hebbian learning implementation using covariance rule:
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    // Calculate statistics for covariance computation
    float sumPre = 0.0f;
    float sumPost = 0.0f;
    float sumPrePost = 0.0f;
    float count = static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Compute spike statistics over the observation window
    for (Timestamp preTime : preSpikes) {
        sumPre += static_cast<float>(preTime);
        for (Timestamp postTime : postSpikes) {
            sumPost += static_cast<float>(postTime);
            sumPrePost += static_cast<float>(preTime) * static_cast<float>(postTime);
        }
    }
    
    // Normalize statistics
    float meanPre = sumPre / static_cast<float>(preSpikes.size());
    float meanPost = sumPost / static_cast<float>(postSpikes.size());
    float meanPrePost = sumPrePost / count;
    
    // Compute covariance
    float covariance = meanPrePost - (meanPre * meanPost);
    
    // Apply covariance rule with adaptive learning rate
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds and regularization
    if (std::abs(delta) > 1e-6f) {
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for future reward modulation
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta * 0.5f);
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
