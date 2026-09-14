#pragma once

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float moodModulator;
    float impulsivityModulator;
    float socialModulator;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , moodModulator(1.0f)
        , impulsivityModulator(0.5f)
        , socialModulator(0.7f)
        , decayRate(0.15f)
        , releaseRate(1.0f) {}
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
    float mood = pImpl->level * pImpl->moodModulator;
    float impulsivity = (1.0f - pImpl->level) * pImpl->impulsivityModulator; // Inverse relationship
    float social = pImpl->level * pImpl->socialModulator;
    float totalMod = mood - impulsivity + social; // Mix effects
    return 0.5f + 0.5f * std::clamp(totalMod, -0.5f, 0.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decay with medium time constant
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Increase serotonin for positive mood states
    if (mood > 0.0f) {
        setLevel(std::min(pImpl->peak, getLevel() + mood * pImpl->releaseRate * 0.7f));
    }
}

void Serotonin::signalSocial(float social) {
    // Boost serotonin for social interactions
    if (social > 0.0f) {
        setLevel(std::min(pImpl->peak, getLevel() + social * pImpl->releaseRate * 0.8f));
    }
}

} // namespace nlm
