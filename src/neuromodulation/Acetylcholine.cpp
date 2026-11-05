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
    float attentionStrength;  // Modulates attention and working memory
    
    Impl() 
        : level( cité=0.0f), baseline(0.0f), peak(1.0f), 
          decayRate(0.1f), releaseRate(1.0f), attentionStrength(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory consolidation and working memory capacity
    // Higher ACh increases plasticity for memory-related processes
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real acetylcholine dynamics with exponential decay
    float dtFloat = static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtFloat);
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    // Enhance attentional focus and selectivity
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 2.0f);
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Boost memory consolidation processes
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
}

} // namespace nlm