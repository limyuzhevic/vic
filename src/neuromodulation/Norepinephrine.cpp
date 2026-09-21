#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.5f), decayRate(0.08f), releaseRate(0.8f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real norepinephrine-modulated plasticity factor
    // Norepinephrine increases arousal and vigilance
    // Higher NE increases alertness and modulates plasticity
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline, release on salient events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // TODO PHASE 2: Implement arousal signaling
    // PLACEHOLDER: Norepinephrine release on arousal events
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // TODO PHASE 2: Implement vigilance signaling
    // PLACEHOLDER: Norepinephrine modulates vigilance and attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

} // namespace nlm