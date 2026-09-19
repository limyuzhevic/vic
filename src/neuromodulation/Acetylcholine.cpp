#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             attentionGain(1.5f), memoryConsolidationRate(0.3f) {}
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
    // ACh enhances plasticity for memory encoding
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::applyToAttention() {
    // ACh increases attentional focus and signal-to-noise ratio
    float attentionBoost = pImpl->level * pImpl->attentionGain;
    // Effects are applied in Brain::step() when ACh level is high
}

void Acetylcholine::applyToMemoryConsolidation() {
    // ACh enhances memory consolidation during rest phases
    // Consolidation rate is proportional to ACh level
    float consolidationBoost = pImpl->level * pImpl->memoryConsolidationRate;
    // Effects applied in Brain::step()
}

} // namespace nlm