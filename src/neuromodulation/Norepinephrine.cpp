#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.3f), releaseRate(1.0f), arousalBoost(1.5f) {}
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
    // TODO PHASE 2: Implement real norepinephrine-modulated plasticity factor
    // PLACEHOLDER: Higher norepinephrine increases arousal, modulating plasticity
    // NE enhances signal-to-noise ratio, prioritizing important information
    return 0.5f + 0.4f * pImpl->level + 0.1f * (pImpl->level * pImpl->level);  // Quadratic effect
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline, modulated by stress/arousal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // TODO PHASE 2: Implement real arousal signaling
    // PLACEHOLDER: Norepinephrine bursts on novel or salient events
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // TODO PHASE 2: Implement real vigilance signaling
    // PLACEHOLDER: NE maintains alertness during sustained attention
    pImpl->level = std::max(pImpl->level, vigilance * 0.9f);
}

void Norepinephrine::signalStress(float stress) {
    // TODO PHASE 2: Implement real stress signaling
    // PLACEHOLDER: NE response to stress affects memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * 0.5f * pImpl->releaseRate);
}

} // namespace nlm
