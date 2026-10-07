#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f), mood(0.5f), impulsivity(0.5f) {}
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
    // TODO PHASE 2: Implement real serotonin-modulated plasticity factor
    // PLACEHOLDER: Higher serotonin reduces impulsivity and stabilizes plasticity
    return 0.5f + 0.3f * pImpl->level * (1.0f - pImpl->impulsivity);
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline with moderate rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // TODO PHASE 2: Implement real mood signaling
    // PLACEHOLDER: Burst of serotonin influences mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
    pImpl->mood = moodLevel;
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // TODO PHASE 2: Implement real impulsivity signaling
    // PLACEHOLDER: Serotonin modulates impulsivity and decision making
    pImpl->level = std::max(pImpl->level, impulsivityLevel * pImpl->releaseRate * 0.7f);
    pImpl->impulsivity = 1.0f - impulsivityLevel; // Inverse relationship
}

void Serotonin::signalSocial(float socialCohesion) {
    // TODO PHASE 2: Implement real social signaling
    // PLACEHOLDER: Serotonin enhances social behavior and cooperation
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->releaseRate * 0.5f);
}

} // namespace nlm
