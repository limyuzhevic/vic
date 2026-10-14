#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float learningModulation;
    
    // Memory consolidation state
    float memoryConsolidationRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), 
             learningModulation(1.0f), memoryConsolidationRate(0.0f), attentionGain(1.0f) {}
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
    // ACh modulates attention and memory consolidation
    return pImpl->level * 0.5f + 1.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate attention and memory
    pImpl->attentionGain = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryConsolidationRate = pImpl->level * 0.3f;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    setLevel(std::min(pImpl->peak, getLevel() + attentionStrength * 0.5f));
}

void Acetylcholine::signalMemoryConsolidation() {
    // ACh burst signals memory consolidation
    setLevel(std::min(pImpl->peak, getLevel() * 1.5f));
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

} // namespace nlm