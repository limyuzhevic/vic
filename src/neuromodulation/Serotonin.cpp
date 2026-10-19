#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher serotonin reduces impulsivity and improves mood
    // Based on: 5-HT modulates mood, impulsivity, and social behavior
    return 0.4f + 0.3f * pImpl->level + 0.3f * moodStabilization;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline, respond to social/behavioral states
    // 5-HT modulates mood, impulsivity, and social behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodStrength) {
    // TODO PHASE 2: Implement mood signaling
    // PLACEHOLDER: Burst of serotonin on mood changes
    // 5-HT signals mood and behavioral state
    float burst = moodStrength * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // TODO PHASE 2: Implement social behavior signaling
    // PLACEHOLDER: Serotonin modulates social approach and interaction
    // Higher levels promote social behavior and reduce aggression
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * 0.4f);
}

void Serotonin::reduceImpulsivity(float reductionStrength) {
    // TODO PHASE 2: Implement impulsivity control
    // PLACEHOLDER: Serotonin reduces impulsive actions
    // This helps stabilize behavior and improve decision-making
    pImpl->level = std::min(pImpl->peak, pImpl->level + reductionStrength * 0.5f);
}

} // namespace nlm

void Serotonin::reset() {
    // Reset serotonin to baseline
    setLevel(0.0f);
}