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
    // Reward with temporal difference and eligibility traces
    // More sophisticated reward accumulation
    pImpl->accumulatedReward += delta;
    
    // Current reward includes eligibility trace decay
    pImpl->currentValue = std::max(0.0f, pImpl->currentValue * 0.9f + delta * 1.1f);
    
    // Update history with exponential moving average
    if (pImpl->history.size() < 100) {
        pImpl->history.push_back(pImpl->currentValue);
    } else {
        // Replace oldest with newest
        pImpl->history.erase(pImpl->history.begin());
        pImpl->history.push_back(pImpl->currentValue);
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // More sophisticated reward computation
    // Based on observation features including value, novelty, and prediction error
    
    float baseReward = 0.0f;
    
    // Extract features from observation
    const auto& resources = observation.getResources();
    const auto& hazards = observation.getHazards();
    const auto& exploration = observation.getExploration();
    
    // Positive rewards
    for (const auto& resource : resources) {
        baseReward += resource.value * resource.amount;
    }
    
    // Negative rewards for hazards
    for (const auto& hazard : hazards) {
        baseReward -= hazard.value * hazard.amount;
    }
    
    // Exploration bonus
    baseReward += exploration * 0.1f; // Small exploration bonus
    
    // Apply biological constraints
    baseReward = std::clamp(baseReward, -1.0f, 1.0f);
    
    // Store in history
    pImpl->history.push_back(baseReward);
    
    return baseReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
