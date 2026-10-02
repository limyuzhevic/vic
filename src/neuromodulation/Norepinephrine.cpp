#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float arousalLevel;
    float vigilanceLevel;
    TimestepDuration lastUpdateTime;
    float stressLevel;
    float rewardSensitivity;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.12f), 
             releaseRate(2.0f), learningRate(0.08f), arousalLevel(0.0f),
             vigilanceLevel(0.0f), lastUpdateTime(0), stressLevel(0.0f),
             rewardSensitivity(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    // Moderate arousal increases plasticity, high stress decreases it
    float arousalFactor = pImpl->arousalLevel;
    float stressFactor = pImpl->stressLevel;
    
    // Inverted U-shaped relationship: moderate NE maximizes plasticity
    if (arousalFactor > 0.6f) {
        return 0.5f * (1.0f - arousalFactor) + 0.1f;  // Decline with high arousal
    }
    return 0.5f * (1.0f + arousalFactor * 0.8f * (1.0f - stressFactor));
}

void Norepinephrine::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Decay towards baseline
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Stress feedback loop
    if (pImpl->stressLevel > 0.5f) {
        pImpl->level *= (1.0f - pImpl->stressLevel * 0.3f);
    }
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    if (stressLevel > 0.3f) {
        // Stress triggers NE release
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 0.5f);
    }
}

void Norepinephrine::signalReward(float reward) {
    pImpl->rewardSensitivity += reward * pImpl->learningRate;
    pImpl->rewardSensitivity = std::clamp(pImpl->rewardSensitivity, 0.5f, 2.0f);
    
    // Positive reward increases NE level (learning signal)
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->rewardSensitivity);
}

void Norepinephrine::signalPunishment(float punishment) {
    if (punishment > 0.0f) {
        // Punishment increases NE level (alerting signal)
        pImpl->level = std::min(pImpl->peak, pImpl->level + punishment * pImpl->releaseRate * 0.3f);
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + punishment * 0.2f);
    }
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
    
    // Update level based on arousal
    if (arousal > 0.3f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate * 0.4f);
    }
}

float Norepinephrine::getArousal() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilanceLevel = std::clamp(vigilance, 0.0f, 1.0f);
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->stressLevel = 0.0f;
    pImpl->rewardSensitivity = 1.0f;
}

} // namespace nlm