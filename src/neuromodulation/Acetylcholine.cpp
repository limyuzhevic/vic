#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionStrength;
    float memoryConsolidationStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), attentionStrength(1.0f), 
             memoryConsolidationStrength(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

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
    // Acetylcholine enhances plasticity for memory consolidation
    return 1.0f + 0.5f * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength);
}

void Acetylcholine::promoteMemoryConsolidation() {
    pImpl->memoryConsolidationStrength = std::min(1.0f, pImpl->memoryConsolidationStrength + 0.2f);
}

} // namespace nlm