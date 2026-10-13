// Acetylcholine implementation - Enhanced neuromodulator for attention and memory consolidation
#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidationStrength;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             memoryConsolidationStrength(0.0f), attentionGain(0.0f) {}
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
    // ACh enhances plasticity for memory consolidation, reduces for new learning
    return 0.8f + 0.2f * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay attention gain
    pImpl->attentionGain = std::max(0.0f, pImpl->attentionGain - pImpl->decayRate * 0.5f * static_cast<float>(dt));
}

void Acetylcholine::consolidateMemory(float strength) {
    pImpl->memoryConsolidationStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Acetylcholine::enhanceAttention(float attentionGain) {
    pImpl->attentionGain = std::clamp(attentionGain, 0.0f, 1.0f);
}

} // namespace nlm