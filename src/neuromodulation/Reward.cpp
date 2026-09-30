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
    // IMPROVEMENT: Implement real reward computation from observation
    // Calculate reward based on observation properties
    
    float reward = 0.0f;
    
    // Extract observation data
    float energy = observation.getEnergy();
    float health = observation.getHealth();
    const auto& objects = observation.getObjects();
    
    // Energy-based reward (positive for high energy)
    reward += energy * 0.1f;
    
    // Health-based reward (positive for high health)
    reward += health * 0.2f;
    
    // Resource-based reward (positive for resources, negative for hazards)
    for (const auto& obj : objects) {
        if (obj.type == WorldObjectType::Resource) {
            reward += obj.value * 0.5f;
        } else if (obj.type == WorldObjectType::Hazard) {
            reward -= obj.value * 0.3f;
        }
    }
    
    // Add exploration bonus for novel objects
    auto noveltyLevel = observation.getNoveltyLevel();
    reward += noveltyLevel * 0.1f;
    
    // Add survival bonus (positive constant)
    reward += 0.05f;
    
    // Normalize reward to reasonable range
    reward = std::clamp(reward, -10.0f, 10.0f);
    
    // Add to history
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 1000) {
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
