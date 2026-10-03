#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rewardSensitivity;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(1.0f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f)
        , rewardSensitivity(1.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.03f) {}
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
    // Serotonin affects mood, impulsivity, and social behavior
    float moodEffect = 1.0f + (pImpl->level * 0.3f * pImpl->mood);
    return moodEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: slower response, affects long-term mood states
    float target = pImpl->baseline + getLevel() * 0.05f;
    
    // Slower integration for serotonergic system
    pImpl->level += (target - pImpl->level) * 0.03f;
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and behavioral effects
    pImpl->mood = 1.0f + getLevel() * 0.5f;
    pImpl->socialBehavior = 1.0f + getLevel() * 0.4f;
    pImpl->rewardSensitivity = 1.0f + getLevel() * 0.3f;
}

void Serotonin::signalSocialInteraction(float socialQuality) {
    // Serotonin responds to social interactions and rank
    float socialBoost = socialQuality * 0.7f;
    setLevel(std::min(pImpl->peak, getLevel() + socialBoost * 1.5f));
}

void Serotonin::modifyImpulsivity(float change) {
    // Serotonin modulates impulsivity
    pImpl->impulsivity = std::clamp(pImpl->impulsivity + change, 0.2f, 2.0f);
}

void Serotonin::updateRewardSensitivity() {
    // Serotonin adjusts reward sensitivity based on current level
    pImpl->rewardSensitivity = 1.0f + getLevel() * 0.5f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->impulsivity = 1.0f;
    pImpl->socialBehavior = 1.0f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->mood = 1.0f;
}

} // namespace nlm
