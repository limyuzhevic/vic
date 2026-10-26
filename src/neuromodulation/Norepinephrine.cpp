#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousal;
    float alertness;
    float tonic;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), arousal(1.0f), alertness(0.5f), tonic(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and attention
    // High NE promotes synaptic strengthening for salient events
    float arousalEffect = pImpl->arousal * pImpl->level;
    float alertnessEffect = (1.0f - pImpl->alertness) * 0.5f * pImpl->level;
    return 1.0f + arousalEffect + alertnessEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: rapid decay but sensitive to novel events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Increase arousal and alertness
void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousal = std::min(1.0f, pImpl->arousal + amount * 0.1f);
}

void Norepinephrine::increaseAlertness(float amount) {
    pImpl->alertness = std::min(1.0f, pImpl->alertness + amount * 0.1f);
}

// Get combined arousal and alertness measure
float Norepinephrine::getArousalAlertness() const {
    return pImpl->arousal * pImpl->alertness;
}

} // namespace nlm
