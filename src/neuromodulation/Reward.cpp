#include "Reward.hpp"
#include <cmath>
#include "../world/Observation.hpp"

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
    const auto& data = observation.getData();
    if (data.size() < 4) {
        return 0.0f;
    }
    
    // Extract position information from observation
    // Observation data format: [agentX, agentY, goalX, goalY] (normalized)
    float agentX = data[0];
    float agentY = data[1];
    float goalX = data[2];
    float goalY = data[3];
    
    // Compute distance to goal (Euclidean distance)
    float distance = std::sqrt(std::pow(agentX - goalX, 2) + std::pow(agentY - goalY, 2));
    
    // Base reward: negative step penalty
    float reward = -0.01f;
    
    // Bonus for reaching goal (small goal area)
    const float goalRadius = 0.1f; // Normalized goal radius
    if (distance < goalRadius) {
        reward += 1.0f; // Goal reached
        pImpl->history.push_back(reward);
        return reward;
    }
    
    // Encourage movement toward goal (negative distance based reward)
    reward -= distance * 0.5f;
    
    // Add to history
    pImpl->history.push_back(reward);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
