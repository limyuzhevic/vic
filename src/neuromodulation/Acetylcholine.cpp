#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float memoryBoostRate;
    
    Impl() : level(0.0f), baseline(0.1f), decayRate(0.05f), memoryBoostRate(2.0f) {}
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
    // TODO PHASE 2: Implement real acetylcholine-modulated plasticity factor
    // PLACEHOLDER: ACh enhances memory consolidation and attention
    return 1.0f + pImpl->level * 0.5f;  // ACh boosts plasticity for memory encoding
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay with short time constant
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // TODO PHASE 2: Implement acetylcholine signaling for attention
    // PLACEHOLDER: ACh bursts with attentional demand
    pImpl->level = std::min(1.0f, pImpl->level + attentionStrength * pImpl->memoryBoostRate);
}

void Acetylcholine::boostMemoryConsolidation(float memoryStrength) {
    // TODO PHASE 2: Implement acetylcholine effects on memory consolidation
    // PLACEHOLDER: ACh enhances memory consolidation during sleep/rest
    pImpl->level = std::min(1.0f, pImpl->level + memoryStrength * pImpl->memoryBoostRate * 0.5f);
}

} // namespace nlm