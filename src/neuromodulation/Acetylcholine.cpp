#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float consolidationFactor;
    float attentionFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), consolidationFactor(0.5f), attentionFactor(0.3f) {}
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
    // ACH enhances attention and working memory consolidation
    // Higher levels improve memory stability and attention focus
    float attentionEffect = std::pow(pImpl->level, 0.5f);
    float consolidationEffect = std::min(1.0f, pImpl->level * pImpl->consolidationFactor);
    return 1.0f + attentionEffect * pImpl->attentionFactor + consolidationEffect * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACH decays toward baseline but has sustained effects on memory
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Increase ACh level based on attention demand
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->attentionFactor);
}

void Acetylcholine::enhanceMemoryConsolidation(float memoryStrength) {
    // ACH boosts memory consolidation for important experiences
    float consolidationBoost = std::min(1.0f, pImpl->level * pImpl->consolidationFactor);
    // This would be used by episodic memory system
    // For now, just store the effect
    // pImpl->lastConsolidationStrength = memoryStrength * consolidationBoost;
}

const float Acetylcholine::getAttentionFactor() const {
    return pImpl->attentionFactor;
}

void Acetylcholine::setAttentionFactor(float factor) {
    pImpl->attentionFactor = factor;
}

} // namespace nlm