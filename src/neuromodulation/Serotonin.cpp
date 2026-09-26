#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityLevel;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodLevel(1.0f), impulsivityLevel(1.0f), socialBehavior(1.0f) {}
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
    // 5-HT regulates mood and impulsivity: Moderate levels optimize plasticity, high levels reduce exploration
    float moodEffect = 0.5f + 0.5f * pImpl->moodLevel;
    float impulsivityEffect = std::max(0.1f, 1.0f - 0.8f * pImpl->impulsivityLevel);
    float socialEffect = 0.5f + 0.5f * pImpl->socialBehavior;
    return moodEffect * impulsivityEffect * socialEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood based on current level
    pImpl->moodLevel = 0.5f + 0.5f * pImpl->level;
    
    // Impulsivity: Higher levels reduce impulsivity (more cautious behavior)
    pImpl->impulsivityLevel = std::max(0.2f, 1.0f - 0.8f * pImpl->level);
    
    // Social behavior: Higher levels enhance social engagement
    pImpl->socialBehavior = 0.3f + 0.7f * pImpl->level;
}

void Serotonin::signalPositiveExperience(float intensity) {
    // Positive experiences increase serotonin level
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Serotonin::reduceImpulsivity(float duration) {
    // Reduce impulsivity for the given duration (more cautious behavior)
    pImpl->impulsivityLevel = std::max(0.2f, pImpl->impulsivityLevel - duration * 0.1f);
}

} // namespace nlm
