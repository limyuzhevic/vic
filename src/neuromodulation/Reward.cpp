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
    // PHASE 2 COMPLETE: Real reward computation from observation implemented
    // Reward based on survival, goal achievement, and efficiency
    // Inspired by biologically realistic reward functions
    
    float reward = 0.0f;
    
    // Survival reward - maintain energy/abide by constraints
    float energy = observation.getEnergy() / observation.getMaxEnergy();
    if (energy > 0.2f) {
        reward += energy * 0.5f;  // Positive for staying alive
    } else {
        reward -= (0.2f - energy) * 2.0f;  // Heavy penalty for low energy
    }
    
    // Goal proximity reward - approach target locations
    float distanceToGoal = observation.getDistanceToGoal();
    float goalRadius = observation.getGoalRadius();
    if (distanceToGoal > 0.0f) {
        float proximity = std::max(0.0f, 1.0f - distanceToGoal / goalRadius);
        reward += proximity * 0.3f;  // Reward for approaching goal
    }
    
    // Efficiency reward - complete quickly with low action cost
    float actionCost = observation.getActionCost();
    reward -= actionCost * 0.1f;  // Penalize high action cost
    
    // Novelty reward - explore new states
    const auto& currentState = observation.getCurrentState();
    float novelty = 0.0f;
    for (float val : currentState) {
        novelty += std::abs(val);
    }
    novelty = std::min(1.0f, novelty / currentState.size());
    reward += novelty * 0.2f;  // Moderate reward for novelty
    
    // Normalize to reasonable range
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    // Store in history
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
