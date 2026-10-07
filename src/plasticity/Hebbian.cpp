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
    
    // Optimized spike correlation detection
    size_t correlationCount = 0;
    const float correlationWindow = 100.0f;  // 100ms correlation window
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtTime = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dtTime) < correlationWindow) {
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds and eligibility trace support
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for reward-modulated Hebbian learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta);
    }
}

void Hebbian::updateWithEligibilityTrace(Synapse* synapse,
                                          const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt,
                                          float rewardSignal) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // First, compute Hebbian delta as in regular update
    size_t correlationCount = 0;
    const float correlationWindow = 100.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtTime = static_cast<float>(postTime - preTime);
            if (std::abs(dtTime) < correlationWindow) {
                ++correlationCount;
            }
        }
    }
    
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply reward-modulated Hebbian learning
    if (rewardSignal != 0.0f && std::abs(delta) > 1e-6f) {
        // Reward-modulated Hebbian (R-Hebbian)
        // Δw = Hebbian_delta * reward_signal * eligibility_trace
        float eligibilityTrace = synapse->getEligibilityTrace();
        float rewardModulatedDelta = delta * rewardSignal * eligibilityTrace;
        applyWeightChange(synapse, rewardModulatedDelta);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(0.01f);
    } else if (std::abs(delta) > 1e-6f) {
        // Standard Hebbian update
        applyWeightChange(synapse, delta);
        synapse->setEligibilityTrace(delta);
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
