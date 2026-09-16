#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    float memoryConsolidationBoost;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), attentionStrength(0.5f), memoryConsolidationBoost(1.2f), 
             baseline(0.0f), decayRate(0.05f) {}
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
    // Acetylcholine enhances memory consolidation
    return 1.0f + pImpl->memoryConsolidationBoost * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Acetylcholine::modulateWorkingMemory(float factor) {
    pImpl->memoryConsolidationBoost = std::max(1.0f, factor);
}

void Acetylcholine::facilitateMemoryConsolidation() {
    // Boost memory consolidation when ACh is high
    pImpl->level = std::min(pImpl->level * 1.1f, 1.0f);
}

} // namespace nlm