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
    // REAL IMPLEMENTATION: Compute reward based on observation content
    // This method analyzes the observation to determine appropriate reward
    
    float totalReward = 0.0f;
    
    // Check for resource objects (positive reward)
    // In SimpleWorld, resources give positive reward when consumed
    // For now, we'll use heuristic detection based on observation content
    for (float signal : observation.getInternalSignals()) {
        if (signal > 0.5f) {  // High signal might indicate resource proximity
            totalReward += 10.0f;  // Substantial reward for resources
        } else if (signal < -0.5f) {  // Negative signal might indicate hazard
            totalReward -= 5.0f;  // Penalty for hazards
        }
    }
    
    // Check for proximity to goal/terminal state
    // This is a simple heuristic - can be refined for specific environments
    if (observation.getEnergy() > 0.8f) {  // High energy indicates good state
        totalReward += 2.0f;
    }
    
    if (observation.getHealth() < 0.3f) {  // Low health is bad
        totalReward -= 8.0f;
    }
    
    // Small step penalty to encourage efficiency
    totalReward -= 0.01f;
    
    // Clamp to reasonable range
    totalReward = std::clamp(totalReward, -20.0f, 20.0f);
    
    // Add to history
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 100) {  // Keep history manageable
        pImpl->history.erase(pImpl->history.begin());
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
