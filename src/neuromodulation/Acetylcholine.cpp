#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    float memoryConsolidationStrength;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), attentionStrength(1.0f), memoryConsolidationStrength(0.0f), 
             baseline(0.0f), decayRate(0.05f) {}
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
    // ACh enhances learning and memory consolidation
    return 0.8f + 0.2f * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reduce attention enhancement over time
    pImpl->attentionStrength = std::max(1.0f, pImpl->attentionStrength - 0.01f);
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::clamp(strength, 1.0f, 3.0f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    pImpl->memoryConsolidationStrength = std::clamp(strength, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

} // namespace nlm
