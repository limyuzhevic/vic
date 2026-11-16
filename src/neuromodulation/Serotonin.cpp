#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rewardSensitivity;
    
    Impl() : level(0.0f), mood(0.0f), impulsivity(0.0f), socialBehavior(0.0f), rewardSensitivity(0.0f) {}
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
    // Serotonin has complex effects on plasticity
    // Moderate levels enhance some forms of learning, high levels inhibit
    if (pImpl->level < 0.5f) {
        return 1.0f + pImpl->level * 0.3f;  // Low-moderate enhancement
    } else {
        return 1.2f - (pImpl->level - 0.5f) * 0.2f;  // Dim decreasing effect
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Update serotonin based on social interactions and mood
    pImpl->level = 0.1f;  // Baseline level
    
    // Serotonin affects mood and behavior regulation
    pImpl->mood = pImpl->level * 0.9f;
    pImpl->impulsivity = pImpl->level * 0.5f;
    pImpl->socialBehavior = pImpl->level * 0.7f;
    pImpl->rewardSensitivity = pImpl->level * 0.6f;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void Serotonin::signalSocial(float interaction) {
    // Serotonin increases with positive social interaction
    setLevel(getLevel() + interaction * 0.1f);
}

void Serotonin::signalLowMood(float low) {
    // Serotonin decreases with negative mood
    setLevel(getLevel() * 0.9f);
}

} // namespace nlm