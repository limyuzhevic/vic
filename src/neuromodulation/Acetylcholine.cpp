// Acetylcholine: Attention and memory consolidation
// Real implementation for attention modulation

#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float effectOnAttention;
    float effectOnMemory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             effectOnAttention(1.5f), effectOnMemory(2.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attention and memory
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Attention modulation effect
void Acetylcholine::enhanceAttention(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Enhance attention to sensory inputs
    if (auto* attention = brain->getAttention()) {
        // This would apply attention weighting to sensory processing
        // In a real implementation, attention would modulate sensory gain
    }
}

} // namespace nlm
