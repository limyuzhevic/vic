#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Reward magnitude history
    float rewardHistory[10];
    size_t rewardHistoryIndex;
    float rewardSum;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(2.0f)
        , rewardHistoryIndex(0)
        , rewardSum(0.0f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f) {
        std::fill(std::begin(rewardHistory), std::end(rewardHistory), 0.0f);
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "REWARD";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward increases plasticity for reinforcement learning
    return 1.0f + 0.7f * pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    // Time-dependent adaptation to maintain reward sensitivity
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.3f * std::tanh(pImpl->adaptationTime * 0.02f - 0.5f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.04f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Reward::computeReward(const Observation& observation) const {
    // Compute reward from observation based on predicted values
    // This is a simplified version - in practice this would use complex logic
    const auto& obsData = observation.getData();
    
    float reward = 0.0f;
    for (size_t i = 0; i < obsData.size(); ++i) {
        // Reward for positive values, punish for negative values
        reward += obsData[i] * 0.1f;
    }
    
    // Normalize reward
    reward = std::clamp(reward, 0.0f, 1.0f);
    
    // Update history
    pImpl->rewardHistory[pImpl->rewardHistoryIndex] = reward;
    pImpl->rewardSum += reward;
    pImpl->rewardHistoryIndex = (pImpl->rewardHistoryIndex + 1) % 10;
    
    if (pImpl->rewardHistoryIndex == 0) {
        pImpl->rewardSum = 0.0f;
        for (size_t i = 0; i < 10; ++i) {
            pImpl->rewardSum += pImpl->rewardHistory[i];
        }
    }
    
    return reward;
}

void Reward::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
    std::fill(std::begin(pImpl->rewardHistory), std::end(pImpl->rewardHistory), 0.0f);
    pImpl->rewardHistoryIndex = 0;
    pImpl->rewardSum = 0.0f;
}

} // namespace nlm
