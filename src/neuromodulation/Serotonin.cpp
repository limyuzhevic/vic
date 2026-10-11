#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and social behavior modulation
    float moodStrength;
    float socialBehaviorStrength;
    float impulsivityReduction;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodStrength(1.0f), socialBehaviorStrength(1.0f), impulsivityReduction(1.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    return 0.4f + 0.6f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float moodStrength) {
    pImpl->moodStrength = moodStrength;
}

void Serotonin::modulateSocialBehavior(float strength) {
    pImpl->socialBehaviorStrength = strength;
}

void Serotonin::reduceImpulsivity(float reduction) {
    pImpl->impulsivityReduction = reduction;
}

} // namespace nlm