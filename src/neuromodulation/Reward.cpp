#include "Reward.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentLevel;
    float totalRewardEarned;
    float maxPossibleReward;
    int rewardCooldown;
    
    Impl() : currentLevel(0.0f), totalRewardEarned(0.0f), 
             maxPossibleReward(1.0f), rewardCooldown(0) {}
};

Reward::Reward() : pImpl(std::make_unique<Impl>()), totalReward(0.0f), stepsSinceLastReward(0) {}

Reward::~Reward() = default;

float Reward::getLevel() const {
    return pImpl->currentLevel;
}

void Reward::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward modulates plasticity based on magnitude and recency
    float recencyFactor = std::exp(-static_cast<float>(pImpl->rewardCooldown) * 0.1f);
    return 1.0f + pImpl->currentLevel * 0.5f * recencyFactor;
}

void Reward::update(TimestepDuration dt) {
    // Decay reward signal over time
    pImpl->rewardCooldown++;
    pImpl->currentLevel = std::max(0.0f, pImpl->currentLevel - 0.05f * static_cast<float>(dt));
}

void Reward::computeReward(const Environment& environment, 
                          const nlm::Observation& observation) {
    // Compute reward based on environment state and observation
    // This is a placeholder for goal-directed reward computation
    float goalProgress = 0.0f; // Would compute based on specific goals
    
    // Clamp and update reward signal
    float reward = std::clamp(goalProgress, 0.0f, 1.0f);
    setLevel(reward);
    totalReward += reward;
    stepsSinceLastReward = 0;
}

} // namespace nlm