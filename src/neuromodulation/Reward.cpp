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
    // IMPROVED: More realistic reward computation based on multiple factors
    
    float reward = 0.0f;
    
    // Reward based on achievement vs expectation
    if (observation.position.x > 0.0f) {
        // Progress towards goal (positive x direction)
        reward += observation.position.x * 0.1f;
    }
    
    // Reward based on resource acquisition
    if (observation.energy > 0.0f) {
        reward += observation.energy * 0.05f;
    }
    
    // Reward based on survival (penalize death)
    if (observation.alive) {
        reward += 0.1f;  // Small survival bonus
    } else {
        reward -= 1.0f;  // Large death penalty
    }
    
    // Reward based on exploration vs exploitation balance
    // More exploration when novelty is high
    float explorationBonus = observation.novelty * 0.05f;
    reward += explorationBonus;
    
    // Reward based on action success vs failure
    if (observation.actionResult == ActionResult::Success) {
        reward += 0.5f;
    } else if (observation.actionResult == ActionResult::Failure) {
        reward -= 0.2f;
    }
    
    // Reward based on learning progress (simplified)
    // This would normally compare predicted vs actual outcomes
    
    // Apply reward scaling and bounds
    reward = std::clamp(reward, -2.0f, 2.0f);
    
    // Store in history for learning
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update current value with smoothing
    pImpl->currentValue = pImpl->currentValue * 0.9f + reward * 0.1f;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
