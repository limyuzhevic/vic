#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation effect
    float memoryConsolidation;
    
    // Attention modulation
    float attentionGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , memoryConsolidation(0.5f)
        , attentionGain(0.5f) {}
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
    pImpl->level = std::clamp(level, 0. covenants(0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh increases plasticity but with attention gating
    // Higher ACh enhances memory consolidation
    float plasticity = pImpl->memoryConsolidation;
    if (pImpl->level > 0.5f) {
        plasticity *= 1.5f;  // Boost plasticity at high ACh levels
    }
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay memory consolidation effect
    pImpl->memoryConsolidation = std::max(0.1f, pImpl->memoryConsolidation - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Increase ACh level based on attention demands
    // Positive attention signal strengthens encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
    
    // Boost memory consolidation
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + attentionSignal * 0.1f);
}

void Acetylcholine::enhanceMemory(float memoryStrength) {
    // Direct enhancement of memory processes
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + memoryStrength * 0.2f);
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.1f, 2.0f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

} // namespace nlm