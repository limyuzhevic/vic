#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float impulseControl;
    float socialStrength;
    float baseline;
    float decayRate;
    
    Impl() : level(0.5f), mood(1.0f), impulseControl(1.0f), socialStrength(0.5f), 
             baseline(0.5f), decayRate(0.02f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and impulse control, stabilizing learning
    float moodFactor = std::pow(pImpl->mood, 2.0f) / 2.0f;
    float impulseFactor = 1.0f - std::pow(pImpl->impulseControl, 2.0f) / 2.0f;
    return moodFactor + impulseFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with homeostatic regulation
    float delta = pImpl->baseline - pImpl->level;
    pImpl->level += delta * 0.05f * static_cast<float>(dt);
    
    // Clamp to maintain homeostasis
    pImpl->level = std::clamp(pImpl->level, 0.2f, 0.8f);
}

void Serotonin::regulateMood(float moodLevel) {
    pImpl->mood = std::clamp(moodLevel, 0.5f, 1.5f);
    // Higher mood can increase serotonin baseline
    pImpl->baseline = std::clamp(0.5f + (pImpl->mood - 1.0f) * 0.2f, 0.3f, 0.7f);
    pImpl->level = std::min(1.0f, pImpl->level + moodLevel * 0.2f);
}

void Serotonin::reduceImpulsivity(float inhibition) {
    pImpl->impulseControl = std::clamp(inhibition, 0.5f, 1.0f);
    pImpl->level = std::max(0.0f, pImpl->level - inhibition * 0.3f);
}

void Serotonin::enhanceSocialBehavior(float socialStrength) {
    pImpl->socialStrength = std::clamp(socialStrength, 0.5f, 1.5f);
    pImpl->level = std::min(1.0f, pImpl->level + socialStrength * 0.25f);
}

} // namespace nlm
