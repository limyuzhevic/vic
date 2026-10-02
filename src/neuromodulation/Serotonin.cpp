#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float moodLevel;
    float impulsivityLevel;
    TimestepDuration lastUpdateTime;
    float socialContext;
    float rewardPredictionError;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.08f), 
             releaseRate(1.8f), learningRate(0.06f), moodLevel(0.0f),
             impulsivityLevel(0.0f), lastUpdateTime(0), socialContext(1.0f),
             rewardPredictionError(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on mood and impulsivity
    // High serotonin reduces impulsivity and stabilizes learning
    float moodFactor = pImpl->moodLevel;
    float impulsivityFactor = pImpl->impulsivityLevel;
    
    // Inverse relationship: high serotonin reduces impulsivity and stabilizes plasticity
    float stabilityFactor = 1.0f - impulsivityFactor * 0.7f;
    float moodEffect = 0.5f + moodFactor * 0.5f;
    
    return stabilityFactor * moodEffect * 0.8f;
}

void Serotonin::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Decay towards baseline
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Social context feedback
    if (pImpl->socialContext < 0.7f) {
        // Poor social context reduces serotonin
        pImpl->level *= (0.8f + pImpl->socialContext * 0.2f);
    }
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalSocialBonding(float bondingStrength) {
    pImpl->socialContext += bondingStrength * pImpl->learningRate;
    pImpl->socialContext = std::clamp(pImpl->socialContext, 0.1f, 1.5f);
    
    if (bondingStrength > 0.2f) {
        // Strong social bonding increases serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + bondingStrength * pImpl->releaseRate * 0.8f);
    }
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward prediction error
    if (reward > 0.0f) {
        pImpl->rewardPredictionError += reward * pImpl->learningRate;
        // Increase level for positive prediction error
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * 0.3f * pImpl->releaseRate);
    } else {
        pImpl->rewardPredictionError += reward * pImpl->learningRate;
        // Decrease level for negative prediction error
        pImpl->level = std::max(0.0f, pImpl->level + reward * 0.2f * pImpl->releaseRate);
    }
}

void Serotonin::setMood(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
    
    // Mood affects serotonin level
    if (mood > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (mood - 0.5f) * pImpl->releaseRate * 1.2f);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + (mood - 0.5f) * pImpl->releaseRate * 0.6f);
    }
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivityLevel = std::clamp(impulsivity, 0.0f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivityLevel;
}

void Serotonin::signalSocialReward(float socialReward) {
    // Strong social rewards significantly increase serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialReward * pImpl->releaseRate * 2.0f);
    pImpl->socialContext = std::min(1.5f, pImpl->socialContext + socialReward * 0.1f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.0f;
    pImpl->impulsivityLevel = 0.0f;
    pImpl->socialContext = 1.0f;
    pImpl->rewardPredictionError = 0.0f;
}

} // namespace nlm