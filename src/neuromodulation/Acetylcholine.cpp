#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // For memory consolidation effects
    float consolidationWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             consolidationWeight(1.0f) {}
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
    // ACh modulates memory consolidation and attention
    // Higher ACh increases memory strength and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay consolidation effects
    pImpl->consolidationWeight = std::max(0.1f, pImpl->consolidationWeight - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Attention signal from prefrontal cortex
    setLevel(attentionSignal);
}

float Acetylcholine::getConsolidationWeight() const {
    return pImpl->consolidationWeight;
}

} // namespace nlm
