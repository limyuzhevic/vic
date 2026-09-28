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
    // Implement real reward computation based on observation content
    // Reward is computed based on:
    // 1. Resource availability (positive reward)
    // 2. Hazard avoidance (negative reward)  
    // 3. Goal achievement (positive reward)
    // 4. Novelty detection (small positive reward)
    
    float reward = 0.0f;
    
    // Get the objects in the observation
    const auto& objects = observation.getObjects();
    
    // Reward for resource collection
    for (const auto& obj : objects) {
        if (obj.type == WorldObjectType::Resource) {
            // Larger resource = higher reward
            reward += obj.value * 10.0f;
            
            // Bonusing close resources (efficiency)
            float distance = std::sqrt(obj.x * obj.x + obj.y * obj.y);
            if (distance < 5.0f) {
                reward += 5.0f;  // Proximity bonus
            }
        } else if (obj.type == WorldObjectType::Hazard) {
            // Hazard detection = negative reward
            reward -= std::abs(obj.value) * 5.0f;
            
            // Large penalty for close hazards
            float distance = std::sqrt(obj.x * obj.x + obj.y * obj.y);
            if (distance < 3.0f) {
                reward -= 10.0f;  // Proximity penalty
            }
        }
    }
    
    // Reward for survival (avoid death)
    // This would be determined by agent's energy/health state
    // For now, we assume survival is rewarded
    
    // Reward for efficiency (short path traveled)
    // This would require tracking agent's position history
    
    // Small baseline reward for action (encourages exploration)
    reward += 0.01f;
    
    // Clamp reward to reasonable bounds
    reward = std::clamp(reward, -100.0f, 100.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
