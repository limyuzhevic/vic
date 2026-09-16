#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood modulation state
    float moodLevel;
    
    // Social behavior modulation state
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodLevel(0.0f), socialBehavior(0.0f) {}
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
    // Serotonin affects mood and social behavior, modulating plasticity
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay mood and social behavior
    pImpl->moodLevel *= (1.0f - dt * 0.04f);
    pImpl->socialBehavior *= (1.0f - dt * 0.04f);
}

void Serotonin::modulateMood(float strength) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + strength * 0.5f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Serotonin::modulateSocialBehavior(float strength) {
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + strength);
    // Modulate level for sustained effect
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

} // namespace nlm
