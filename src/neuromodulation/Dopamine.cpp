#include "Dopamine.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float rewardHistory[100];  // Rolling buffer of recent rewards
    size_t historyIndex;
    size_t historySize;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), historyIndex(0), historySize(0) {
        std::fill(std::begin(rewardHistory), std::end(rewardHistory), 0.0f);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
    // Plasticity factor decreases with higher dopamine (stabilizes learning)
    pImpl->plasticityFactor = std::max(0.1f, 1.0f - pImpl->level * 0.5f);
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay reward influence over time
    pImpl->level *= (1.0f - dt * 0.01f);
    // Keep plasticity factor bounded
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor, 0.1f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Store reward in rolling buffer
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    if (pImpl->historySize < 100) {
        pImpl->historySize++;
    }
    
    // Update dopamine level based on reward
    // Average recent rewards for stability
    float recentRewardSum = 0.0f;
    for (size_t i = 0; i < pImpl->historySize; ++i) {
        size_t idx = (pImpl->historyIndex + 99 - i) % 100;
        recentRewardSum += pImpl->rewardHistory[idx];
    }
    float avgReward = recentRewardSum / pImpl->historySize;
    
    // Dopamine level is a function of recent reward
    setLevel(avgReward * 2.0f);  // Scale factor for sensitivity
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error modulates learning rate
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    
    float modulation = std::max(-1.0f, std::min(1.0f, error * 0.5f));
    
    // Adjust dopamine level based on prediction error
    float newLevel = pImpl->level + modulation * 0.1f;
    setLevel(newLevel);
    
    // Update plasticity factor based on prediction error
    // Prediction error can destabilize or stabilize learning
    pImpl->plasticityFactor *= (1.0f + modulation * 0.2f);
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor, 0.1f, 2.0f);
}

} // namespace nlm