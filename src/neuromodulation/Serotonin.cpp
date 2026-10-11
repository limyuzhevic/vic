#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher serotonin affects mood, motivation, and reward processing
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // TODO PHASE 2: Implement mood signaling
    // PLACEHOLDER: Serotonin affects mood and motivation
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
}

void Serotonin::signalMotivation(float motivation) {
    // TODO PHASE 2: Implement motivation signaling
    // PLACEHOLDER: Serotonin influences motivation and goal-directed behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + motivation * pImpl->releaseRate);
}

} // namespace nlm
