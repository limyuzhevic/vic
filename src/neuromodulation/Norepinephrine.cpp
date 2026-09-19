#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.2f), vigilanceLevel(1.0f) {}
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
    // NE modulates arousal and plasticity
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::applyToAttention() {
    // NE increases vigilance and alertness
    float alertnessBoost = pImpl->level * pImpl->arousalLevel;
    // Effects applied in Brain::step()
}

void Norepinephrine::applyToMemoryConsolidation() {
    // NE enhances memory encoding under stress
    float encodingBoost = pImpl->level * pImpl->vigilanceLevel;
    // Effects applied in Brain::step()
}

} // namespace nlm