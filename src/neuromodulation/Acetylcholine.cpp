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
    float attentionModifier;  // How ACh affects attention
    float memoryConsolidationBoost;  // How ACh affects memory consolidation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), attentionModifier(2.0f), memoryConsolidationBoost(1.5f) {}
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
    // Acetylcholine enhances plasticity for attention and memory consolidation
    float attentionEffect = pImpl->level * pImpl->attentionModifier;
    return 0.5f + attentionEffect;  // Boosts plasticity
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline but can be released by attention-demanding tasks
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionDemand) {
    // ACh is released during attention-demanding tasks
    setLevel(std::min(pImpl->peak, pImpl->level + attentionDemand * pImpl->releaseRate));
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    // ACh boosts memory consolidation
    float newLevel = getLevel() + memoryStrength * pImpl->memoryConsolidationBoost;
    setLevel(newLevel);
}

float Acetylcholine::getAttentionEffect() const {
    return pImpl->attentionModifier * getLevel();
}

float Acetylcholine::getMemoryConsolidationEffect() const {
    return pImpl->memoryConsolidationBoost * getLevel();
}

} // namespace nlm
