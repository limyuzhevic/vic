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
    // Phase 6: Real reward computation from observation
    // The agent gets rewards for:
    // 1. Resource consumption (energy gain)
    // 2. Hazard avoidance (damage prevention)
    // 3. Exploration (novelty detection)
    // 4. Efficient navigation (movement cost savings)
    
    float reward = 0.0f;
    
    // Extract relevant information from observation
    // Observation contains: energy, health, isMoving, isTurning, 
    // vision, touch, internal signals, proprioception, audio
    
    // Resource reward: energy is a valuable resource that should be maintained
    float energyLevel = 0.0f;
    if (!observation.internal.empty()) {
        energyLevel = observation.internal[0];  // Energy level is first internal signal
    }
    
    // Reward for high energy (resources are valuable)
    if (energyLevel > 0.7f) {
        reward += 0.5f;
    } else if (energyLevel > 0.4f) {
        reward += 0.2f;
    }
    
    // Health reward: agent should avoid taking damage
    float healthLevel = 0.0f;
    if (!observation.internal.empty() && observation.internal.size() > 1) {
        healthLevel = observation.internal[1];  // Health is second internal signal
    }
    
    // Small negative reward for low health (prevents damage)
    if (healthLevel < 0.3f) {
        reward -= 1.0f;
    }
    
    // Novelty reward: exploration bonus for new experiences
    // Vision contains novelty information - low values indicate novelty
    float noveltyScore = 1.0f;
    if (!observation.vision.empty()) {
        for (float v : observation.vision) {
            if (v < 0.2f) noveltyScore += 0.1f;  // Dark pixels indicate novel objects
        }
    }
    reward += noveltyScore * 0.5f;
    
    // Efficiency reward: small positive reward for movement (energy expenditure)
    // but penalize excessive stillness to encourage exploration
    bool isMoving = false;
    if (!observation.internal.empty() && observation.internal.size() > 2) {
        isMoving = observation.internal[2] > 0.5f;
    }
    
    if (isMoving) {
        reward -= 0.01f;  // Small cost for movement (energy expenditure)
    } else {
        reward += 0.02f;  // Bonus for efficiency (don't waste time)
    }
    
    // Bounded reward to prevent runaway values
    reward = std::clamp(reward, -10.0f, 10.0f);
    
    // Store in history for analysis
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
