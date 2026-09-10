#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorWeight;
    float rewardWeight;
    float noveltyWeight;
    float arousalLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionErrorWeight(0.5f), rewardWeight(0.3f), noveltyWeight(0.2f),
             arousalLevel(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Higher dopamine increases learning for successful actions
    return 0.3f + 0.7f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt, float novelty, 
                     float predictionError, float reward) {
    // Real dopamine dynamics based on prediction error and reward
    float delta = 0.0f;
    
    if (predictionError != 0.0f) {
        // Prediction error drives dopamine activity (positive for better-than-expected)
        delta += predictionError * pImpl->predictionErrorWeight;
    }
    
    if (reward != 0.0f) {
        // Immediate reward processing
        delta += reward * pImpl->rewardWeight;
    }
    
    if (novelty != 0.0f) {
        // Novelty signals increase dopamine for exploration
        delta += novelty * pImpl->noveltyWeight;
    }
    
    // Smooth update with decay
    pImpl->level += delta * 0.1f - pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Update arousal level based on overall activity
    pImpl->arousalLevel = std::max(pImpl->arousalLevel, pImpl->level * 0.8f);
}

float Dopamine::getExcitabilityModulator() const {
    // Dopamine increases neural excitability, especially in nucleus accumbens
    return pImpl->level * 0.4f;
}

float Dopamine::getAttentionModulator() const {
    // Dopamine modulates attention via prefrontal cortex
    // High DA can either enhance or impair attention depending on level
    if (pImpl->level < 0.3f) {
        return pImpl->level * 0.5f;  // Low levels enhance focus
    } else {
        return std::max(0.0f, 1.0f - pImpl->level * 0.5f);  // High levels reduce focus
    }
}

float Dopamine::getMemoryModulator() const {
    // Dopamine modulates memory consolidation (systems consolidation)
    return pImpl->level * 0.3f;
}

void Dopamine::signalReward(float reward) {
    // Immediate reward signaling
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::recordPredictionError(float error) {
    // Store prediction error for future reference
    pImpl->predictionErrorWeight = std::abs(error);
}

} // namespace nlm