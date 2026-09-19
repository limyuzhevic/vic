#include "Dopamine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionGain;
    float memoryConsolidation;
    float decayRate;
    
    Impl() : level(0.0f), attentionGain(1.0f), memoryConsolidation(0.5f), decayRate(0.05f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention and memory encoding
    return pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Signal attention for sensory selection
    pImpl->level = std::min(1.0f, attentionStrength);
    pImpl->attentionGain = 0.5f + 0.5f * pImpl->level;
}

} // namespace nlm
