#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    std::vector<float> rewardHistory;
    std::vector<float> predictionErrorHistory;
    float maxLevel;
    float decayRate;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), maxLevel(1.0f), decayRate(0.01f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: higher levels enhance learning
    return pImpl->level > 0.0f ? 1.0f + pImpl->level * 0.5f : pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Increase dopamine based on reward
    pImpl->level += reward;
    pImpl->level = std::min(pImpl->level, pImpl->maxLevel);
    pImpl->rewardHistory.push_back(reward);
    NLM_LOG_INFO("Dopamine increased by reward: " + std::to_string(reward) + ", new level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error affects dopamine
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    float delta = 0.5f * error;  // Learning rate for prediction error
    pImpl->level += delta;
    pImpl->level = std::max(0.0f, std::min(pImpl->level, pImpl->maxLevel));
    pImpl->predictionErrorHistory.push_back(error);
    NLM_LOG_INFO("Dopamine changed by prediction error: " + std::to_string(error) + ", new level: " + std::to_string(pImpl->level));
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

const std::vector<float>& Dopamine::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

void Dopamine::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->predictionErrorHistory.clear();
}

} // namespace nlm