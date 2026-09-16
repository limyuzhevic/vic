#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation state
    float attentionStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , attentionStrength(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay attention strength
    pImpl->attentionStrength *= (1.0f - dt * 0.05f);
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength);
}

void Acetylcholine::consolidateMemory(float strength) {
    // ACh signals during active learning strengthen memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.5f);
}

} // namespace nlm
