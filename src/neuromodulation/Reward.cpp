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
    return 0.0f;
}

float Reward::computeReward(const Observation& observation, const class AgentBody& agent) const {
    // Real reward computation based on observation and agent state
    float reward = 0.0f;
    
    // Extract observation features
    const auto& observationData = observation.getData();
    
    // Extract agent features
    float agentX = agent.x;
    float agentY = agent.y;
    float agentEnergy = agent.energy;
    float agentHealth = agent.health;
    
    // Compute reward based on proximity to desirable states
    // Positive reward for higher energy and health
    reward += agentEnergy * 0.5f;
    reward += agentHealth * 0.5f;
    
    // Positive reward for being away from boundaries
    float boundaryReward = std::min(agentX, agentY) * 0.1f;
    reward += boundaryReward;
    
    // Check for desirable objects in observation
    // In a real implementation, this would parse the observation for resource locations
    // For now, we assume observation data contains resource information
    if (!observationData.empty()) {
        // Check if agent is near a resource (simplified: based on observation values)
        float resourcePresence = observationData[0];  // Simplified assumption
        reward += resourcePresence * 2.0f;  // Strong positive reward for resources
    }
    
    // Small negative reward for low energy (motivation to seek resources)
    if (agentEnergy < 0.3f) {
        reward -= 0.5f;
    }
    
    // Small negative reward for low health
    if (agentHealth < 0.3f) {
        reward -= 0.5f;
    }
    
    // Ensure reward is reasonable
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
