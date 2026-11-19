#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionBoost;
    float memoryConsolidationBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             attentionBoost(2.0f), memoryConsolidationBoost(1.5f) {}
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
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    // Boost attentional focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * attentionBoost);
}

void Acetylcholine::modulateMemoryConsolidation(float factor) {
    // Enhance memory consolidation
    pImpl->memoryConsolidationBoost = std::max(1.0f, pImpl->memoryConsolidationBoost + factor * 0.5f);
}

} // namespace nlm
