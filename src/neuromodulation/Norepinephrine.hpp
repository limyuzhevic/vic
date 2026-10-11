#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher norepinephrine increases arousal and salience
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // TODO PHASE 2: Implement arousal signaling
    // PLACEHOLDER: Norepinephrine burst on high arousal or salient events
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate);
}

void Norepinephrine::signalSalience(float salience) {
    // TODO PHASE 2: Implement salience signaling
    // PLACEHOLDER: Norepinephrine responds to novel or salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate);
}

} // namespace nlm
