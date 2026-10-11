#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodLevel;
    float impulsivityReduction;
    float socialBehaviorStrength;
    float socialBonding;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), moodLevel(1.0f), 
             impulsivityReduction(0.5f), socialBehaviorStrength(1.0f),
             socialBonding(0.5f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // Serotonin modulates plasticity based on mood and social state
    float moodMod = 1.0f + pImpl->moodLevel * 0.2f;
    float impulseMod = 1.0f + (1.0f - pImpl->impulsivityReduction) * 0.1f;
    float socialMod = 1.0f + pImpl->socialBehaviorStrength * 0.3f;
    return std::min(2.0f, moodMod * impulseMod * socialMod);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
}

void Serotonin::modulateMood(float level) {
    pImpl->moodLevel = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::reduceImpulsivity(float reduction) {
    pImpl->impulsivityReduction = std::clamp(reduction, 0.0f, 1.0f);
}

void Serotonin::enhanceSocialBehavior(float strength) {
    pImpl->socialBehaviorStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Serotonin::setSocialBonding(float bonding) {
    pImpl->socialBonding = std::clamp(bonding, 0.0f, 1.0f);
}

} // namespace nlm