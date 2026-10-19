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
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher norepinephrine increases arousal and learning rate
    // Based on: NE enhances attention, memory encoding, and plasticity
    return 0.6f + 0.2f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline, respond to stress/arousal
    // NE modulates arousal, vigilance, and stress response
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // TODO PHASE 2: Implement real arousal signaling
    // PLACEHOLDER: Burst of norepinephrine on stress/arousal
    // NE signals arousal, stress, and vigilance level
    float burst = arousalStrength * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // TODO PHASE 2: Implement vigilance signaling
    // PLACEHOLDER: Norepinephrine enhances vigilance and alertness
    // Higher levels improve attention and cognitive performance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceStrength * 0.3f);
}

} // namespace nlm

void Norepinephrine::reset() {
    // Reset norepinephrine to baseline
    setLevel(0.0f);
}