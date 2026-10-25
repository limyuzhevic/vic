#include "Dopamine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;           // Current dopamine level
    float rewardHistory;   // Cumulative reward received
    float predictionError; // Reward prediction error
    float baseline;        // Baseline dopamine level
    float learningRate;    // Learning rate for reward prediction
    
    Impl() : level(0.0f), rewardHistory(0.0f), predictionError(0.0f), baseline(0.0f), learningRate(0.01f) {}
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
    // Clamp level to reasonable range
    pImpl->level = std::clamp(level, 0.0f, 10.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: higher levels increase learning rate
    // with an inverted U-shaped relationship
    if (pImpl->level <= pImpl->baseline) {
        return 1.0f + pImpl->level * 0.1f;
    } else {
        // High dopamine can interfere with learning
        return 1.0f + 0.1f - (pImpl->level - pImpl->baseline) * 0.05f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine level based on accumulated reward and prediction error
    float predictionErrorEffect = pImpl->predictionError * 0.1f;
    
    // Dopamine level evolves over time toward a target based on prediction error
    if (pImpl->predictionError > 0.0f) {
        // Positive prediction error: increase dopamine
        pImpl->level += predictionErrorEffect * dt * 0.1f;
    } else {
        // Negative prediction error: decrease dopamine
        pImpl->level += predictionErrorEffect * dt * 0.1f;
    }
    
    // Decay toward baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * dt * 0.01f;
}

void Dopamine::signalReward(float reward) {
    // Integrate reward signal
    pImpl->rewardHistory += reward;
    
    // Update dopamine based on reward magnitude
    if (reward > 0.0f) {
        float targetLevel = 1.0f + std::sqrt(reward) * 0.5f;
        pImpl->level = std::min(pImpl->level + targetLevel * 0.1f, 10.0f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Store prediction error for use in update()
    pImpl->predictionError = error;
}

} // namespace nlm