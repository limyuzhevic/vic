#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryConsolidation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , attentionModulation(0.0f)
        , memoryConsolidation(0.0f) {}
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
    // ACh enhances plasticity for memory consolidation
    // Also modulates attention and working memory
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply attention and memory effects
    float totalFactor = baseFactor * (1.0f + pImpl->attentionModulation + pImpl->memoryConsolidation);
    return std::clamp(totalFactor, 0.1f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate attention and memory consolidation based on level
    pImpl->attentionModulation = pImpl->level * 0.5f;  // Stronger attention with higher ACh
    pImpl->memoryConsolidation = pImpl->level * 0.3f; // Better consolidation with higher ACh
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // ACh increases with attentional demand
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
    NLM_LOG_INFO("Acetylcholine: Attention signal increased level to " + std::to_string(pImpl->level));
}

void Acetylcholine::signalMemoryConsolidation(float consolidationStrength) {
    // ACh peaks during memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->releaseRate * 2.0f);
    NLM_LOG_INFO("Acetylcholine: Memory consolidation signal increased level to " + std::to_string(pImpl->level));
}

} // namespace nlm
