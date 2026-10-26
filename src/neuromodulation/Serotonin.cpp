#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and social state
    float moodLevel;
    float socialStatus;
    float impulsivityLevel;
    float rewardSensitivity;
    std::vector<float> socialMemory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodLevel(0.5f), socialStatus(0.5f), impulsivityLevel(0.5f), rewardSensitivity(0.5f) {}
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
    // Serotonin modulates plasticity - complex effects
    // Low serotonin increases impulsivity, high serotonin promotes stability
    // For now, just scale plasticity factor
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update internal state
    updateInternalState();
}

void Serotonin::signalMood(float moodLevel) {
    // Mood changes affect social behavior and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate * 0.8f);
    pImpl->moodLevel = moodLevel;
    
    // Update social processing
    updateSocialProcessing(moodLevel);
}

void Serotonin::signalSocialStatus(float socialStatus) {
    // Social status changes affect serotonin levels
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStatus * pImpl->releaseRate * 1.0f);
    pImpl->socialStatus = socialStatus;
    
    // Update reward sensitivity
    updateRewardSensitivity(socialStatus);
}

void Serotonin::updateInternalState() {
    // Decay internal state
    pImpl->moodLevel *= 0.95f;
    pImpl->socialStatus *= 0.95f;
    pImpl->impulsivityLevel *= 0.9f;
    pImpl->rewardSensitivity *= 0.95f;
}

void Serotonin::updateSocialProcessing(float moodLevel) {
    // Modulate social behavior based on mood
    // Positive mood increases social engagement, negative mood decreases it
    for (auto& memory : pImpl->socialMemory) {
        if (moodLevel > 0.5f) {
            // Positive mood enhances positive social memories
            memory = std::min(1.0f, memory + 0.1f);
        } else {
            // Negative mood suppresses social memories
            memory *= 0.95f;
        }
    }
}

void Serotonin::updateRewardSensitivity(float socialStatus) {
    // Social status affects reward sensitivity
    // Higher social status increases reward sensitivity
    pImpl->rewardSensitivity = std::min(1.0f, socialStatus * 1.2f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getSocialStatus() const {
    return pImpl->socialStatus;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

} // namespace nlm