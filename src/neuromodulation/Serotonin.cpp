// Serotonin: Mood, impulsivity, and social behavior
// TODO PHASE 2

#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float moodEffect;
    float impulseControl;
    float socialBehavior;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), moodEffect(1.3f), impulseControl(0.8f), 
             socialBehavior(1.4f), baseline(0.0f), decayRate(0.06f), releaseRate(1.0f) {}
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
    // Serotonin affects mood and impulse control
    // Higher 5-HT can stabilize mood but reduce impulsivity
    return 1.0f - 0.2f * pImpl->level * (1.0f - pImpl->impulseControl);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Affect mood when experiencing events
    pImpl->level = std::min(pImpl->level + mood * pImpl->moodEffect * pImpl->releaseRate, 1.0f);
}

void Serotonin::modifyImpulsivity(float impulseChange) {
    // Change impulsivity levels
    pImpl->level = std::min(pImpl->level + impulseChange * (1.0f - pImpl->impulseControl), 1.0f);
}

void Serotonin::enhanceSocialBehavior(float socialBoost) {
    // Boost social behavior
    pImpl->level = std::min(pImpl->level + socialBoost * pImpl->socialBehavior * pImpl->releaseRate, 1.0f);
}

} // namespace nlm