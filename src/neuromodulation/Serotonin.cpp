#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodGain;
    float impulsivityModulator;
    float socialBehaviorGain;
    float moodLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , decayRate(0.04f)
        , moodGain(1.0f)
        , impulsivityModulator(0.5f)
        , socialBehaviorGain(0.8f)
        , moodLevel(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateEffects();
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity and behavioral flexibility
    float mood = getMoodLevel();
    if (mood < 0.3f) {
        return 0.7f;  // Low mood: reduced plasticity, more rigid behavior
    } else if (mood < 0.7f) {
        return 1.0f;  // Normal mood: baseline plasticity
    } else {
        return 1.3f;  // High mood: enhanced plasticity, more flexible behavior
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateEffects();
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityModulator() const {
    return pImpl->impulsivityModulator;
}

float Serotonin::getSocialBehaviorModulator() const {
    return pImpl->socialBehaviorGain;
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Serotonin::setPeak(float peak) {
    pImpl->peak = peak;
}

void Serotonin::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Serotonin::setMoodGain(float gain) {
    pImpl->moodGain = gain;
    updateEffects();
}

void Serotonin::setImpulsivityModulator(float modulator) {
    pImpl->impulsivityModulator = modulator;
}

void Serotonin::setSocialBehaviorGain(float gain) {
    pImpl->socialBehaviorGain = gain;
    updateEffects();
}

void Serotonin::updateEffects() {
    // Calculate neuromodulatory effects based on current serotonin level
    float normalizedLevel = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Mood: Serotonin regulates mood and well-being
    pImpl->moodLevel = 0.5f + 0.5f * normalizedLevel * pImpl->moodGain;
    
    // Impulsivity: Serotonin reduces impulsive behavior
    pImpl->impulsivityModulator = 1.0f - 0.8f * normalizedLevel;
    
    // Social behavior: Serotonin enhances prosocial behavior
    pImpl->socialBehaviorGain = 0.3f + 0.7f * normalizedLevel;
}

} // namespace nlm