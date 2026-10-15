// Serotonin: Mood, impulsivity, and social behavior
// Implements 5-HT signaling for mood regulation, impulsivity control, and social behavior

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityLevel;
    float socialCohesion;
    float rewardSensitivity;
    
    Impl() 
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.02f), releaseRate(1.2f)
        , moodLevel(0.0f), impulsivityLevel(0.0f), socialCohesion(0.0f), rewardSensitivity(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

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
    // 5-HT modulates plasticity: high levels reduce impulsivity and stabilize learning
    // Low 5-HT increases impulsivity and risk-taking
    float plasticityFactor = 1.0f - 0.4f * pImpl->level;
    
    // Apply mood modulation
    plasticityFactor *= (1.0f + pImpl->moodLevel * 0.1f);
    
    // Apply social cohesion modulation
    plasticityFactor *= (1.0f + pImpl->socialCohesion * 0.15f);
    
    return std::clamp(plasticityFactor, 0.3f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay mood and social effects
    pImpl->moodLevel *= 0.9f;
    pImpl->impulsivityLevel *= 0.95f;
    pImpl->socialCohesion *= 0.92f;
    pImpl->rewardSensitivity *= 0.96f;
}

void Serotonin::signalMood(float mood) {
    // Signal mood state (positive or negative)
    float moodSignal = mood * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal);
    
    // Create mood effect
    pImpl->moodLevel += mood * 0.4f;
    
    NLM_LOG_INFO("Serotonin signaling mood: " + std::to_string(mood) + 
                " (level: " + std::to_string(pImpl->level) + ")");
}

void Serotonin::signalReward(float reward) {
    // Signal reward (increases serotonin for positive outcomes)
    float rewardSignal = reward * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Enhance reward sensitivity
    pImpl->rewardSensitivity += reward * 0.3f;
    
    // Improve social cohesion after reward
    pImpl->socialCohesion += reward * 0.2f;
}

void Serotonin::signalPunishment(float punishment) {
    // Signal punishment or negative outcome
    float punishmentSignal = punishment * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + punishmentSignal);
    
    // Punishment reduces impulsivity
    pImpl->impulsivityLevel += punishment * 0.5f;
}

void Serotonin::signalSocialBonding(float bonding) {
    // Signal social bonding and connection
    float bondingSignal = bonding * pImpl->releaseRate * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingSignal);
    
    // Strong social bonding increases social cohesion
    pImpl->socialCohesion += bonding * 0.6f;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

} // namespace nlm
