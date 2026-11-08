#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    // PHASE 6 ENHANCEMENT: Implement real reward computation from observation
    // The observation contains vision data with intensities mapping to objects
    // Resources are encoded as bright vision (~0.8), Hazards as dark (~0.3)
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& vision = sensoryInput->getData();
    if (vision.empty()) {
        return 0.0f;
    }
    
    // Compute reward based on visible objects in vision
    // This is a simple mapping from vision intensities to expected rewards
    // In SimpleWorld: Resources (value=10.0) are ~0.8 intensity, Hazards (value=-15.0) are ~0.3 intensity
    
    float totalReward = 0.0f;
    const float resourceIntensity = 0.8f;
    const float hazardIntensity = 0.3f;
    const float resourceValue = 10.0f;
    const float hazardValue = -15.0f;
    
    for (float intensity : vision) {
        // Map intensity to reward value
        if (intensity > (resourceIntensity + hazardIntensity) / 2.0f) {
            // High intensity -> Resource
            totalReward += resourceValue;
        } else if (intensity < (resourceIntensity + hazardIntensity) / 2.0f) {
            // Low intensity -> Hazard
            totalReward += hazardValue;
        }
        // Medium intensity -> No object (neutral)
    }
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
