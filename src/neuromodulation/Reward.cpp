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
    // Real reward computation based on environmental state
    // This implementation computes reward based on:
    // - Proximity to positive objectives
    // - Avoidance of hazards
    // - Resource availability
    // - Energy levels
    
    float reward = 0.0f;
    
    // Check for resource collection
    if (observation.hasObject(WorldObjectType::Resource)) {
        // Extract resource value
        auto resources = observation.getObjects(WorldObjectType::Resource);
        for (const auto& resource : resources) {
            reward += resource.value;
        }
    }
    
    // Check for hazard avoidance (negative reward)
    if (observation.hasObject(WorldObjectType::Hazard)) {
        // Penalty for being near hazards
        auto hazards = observation.getObjects(WorldObjectType::Hazard);
        for (const auto& hazard : hazards) {
            float distance = std::sqrt(std::pow(hazard.x - observation.getAgentX(), 2) + 
                                     std::pow(hazard.y - observation.getAgentY(), 2));
            reward -= std::max(0.0f, 10.0f - distance);  // Max penalty at 10 units
        }
    }
    
    // Check energy level
    float energy = observation.getEnergy();
    if (energy < 10.0f) {
        // Low energy penalty
        reward -= 1.0f;
    } else if (energy > 50.0f) {
        // Good energy bonus
        reward += 0.5f;
    }
    
    // Add to history
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
