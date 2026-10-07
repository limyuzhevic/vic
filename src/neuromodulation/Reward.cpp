#include "Reward.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    std::vector<float> rewardHistory;
    float lastReward;
    Timestamp lastRewardTime;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , predictionError(0.0f), lastReward(0.0f), lastRewardTime(0.0) {}
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
    // TODO PHASE 2: Implement real reward-modulated plasticity factor
    // PLACEHOLDER: Higher reward increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real reward dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset prediction error if level changes
    if (pImpl->level != pImpl->predictionError) {
        pImpl->predictionError = pImpl->level;
    }
}

void Reward::signal(float reward, Timestamp time) {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Burst of reward on positive outcomes
    float delta = reward - pImpl->lastReward;
    
    if (delta > 0.0f) {
        // Positive reward increases level
        pImpl->level = std::min(pImpl->peak, pImpl->level + delta * pImpl->releaseRate);
    } else if (delta < 0.0f) {
        // Negative reward (punishment) decreases level
        pImpl->level = std::max(pImpl->baseline, pImpl->level + delta * pImpl->releaseRate * 0.5f);
    }
    
    pImpl->lastReward = reward;
    pImpl->lastRewardTime = time;
    pImpl->rewardHistory.push_back(reward);
    
    // Update prediction error based on reward
    pImpl->predictionError = (reward > 0.0f) ? reward : 0.0f;
}

void Reward::computeFromObservations(const std::vector<float>& observations, float baseline) {
    // TODO PHASE 2: Implement real reward computation from observations
    // PLACEHOLDER: Simple average-based reward computation
    if (observations.empty()) {
        pImpl->level = baseline;
        return;
    }
    
    // Compute reward as average of observations
    float sum = 0.0f;
    for (float obs : observations) {
        sum += obs;
    }
    float avg = sum / observations.size();
    
    // Normalize to [0, 1] range
    pImpl->level = std::clamp((avg - baseline) / (1.0f - baseline), 0.0f, 1.0f);
    pImpl->lastReward = pImpl->level;
    pImpl->rewardHistory.push_back(pImpl->level);
}

const std::vector<float>& Reward::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Reward::clearHistory() {
    pImpl->rewardHistory.clear();
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

} // namespace nlm
