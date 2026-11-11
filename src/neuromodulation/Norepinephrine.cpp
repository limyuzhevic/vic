#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float adaptationRate;
    float arousalFactor;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), adaptationRate(0.1f), arousalFactor(0.8f), vigilance(1.0f) {}
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
    // TODO PHASE 2: Implement NE-modulated plasticity factor
    // PLACEHOLDER: NE increases plasticity for salient stimuli
    return 1.0f + pImpl->level * 0.4f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: NE modulates arousal and vigilance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->adaptationRate * static_cast<float>(dt));
}

void Norepinephrine::applyToExcitability(float &leakConductance, float &threshold) {
    // TODO PHASE 2: Implement neural excitability effects
    // PLACEHOLDER: NE reduces threshold and increases excitability
    threshold -= pImpl->level * 5.0f;
    leakConductance += pImpl->level * 2.0f;
}

void Norepinephrine::applyToSensoryProcessing(float &receptorSensitivity, float &noiseLevel) {
    // TODO PHASE 2: Implement sensory modulation
    // PLACEHOLDER: NE increases sensory gain
    receptorSensitivity += pImpl->level * 0.5f;
    noiseLevel *= (1.0f - pImpl->level * 0.2f);
}

} // namespace nlm