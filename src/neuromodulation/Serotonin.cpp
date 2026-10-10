#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float moodRate;
    float socialInput;
    
    Impl() : level(0.2f), baseline(0.1f), decayRate(0.05f), moodRate(2.0f), socialInput(1.0f) {}
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
    // PLACEHOLDER: 5-HT modulates mood and impulsivity
    return 1.0f - 0.3f * pImpl->level;  // 5-HT reduces plasticity (inhibits overlearning)
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay with long time constant
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // TODO PHASE 2: Implement serotonin signaling for mood
    // PLACEHOLDER: 5-HT modulates mood and affects learning
    pImpl->level = std::min(1.0f, pImpl->level + mood * pImpl->moodRate);
}

void Serotonin::signalSocialInteraction(float socialStrength) {
    // TODO PHASE 2: Implement serotonin effects on social behavior
    // PLACEHOLDER: 5-HT affects social learning and interactions
    pImpl->level = std::min(1.0f, pImpl->level + socialStrength * pImpl->moodRate * 0.5f);
}

void Serotonin::reduceImpulsivity(float calmFactor) {
    // TODO PHASE 2: Implement serotonin effects on impulse control
    // PLACEHOLDER: 5-HT reduces impulsivity and promotes persistence
    pImpl->level = std::max(0.0f, pImpl->level - calmFactor * pImpl->moodRate * 0.5f);
}

} // namespace nlm