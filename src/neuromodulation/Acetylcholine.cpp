#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory enhancement state
    float memoryConsolidation;
    float attentionStrength;
    float cholinergicTone;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , memoryConsolidation(0.0f)
        , attentionStrength(0.0f)
        , cholinergicTone(0.0f) {}
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
    // Acetylcholine enhances plasticity for memory formation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update related signals
    pImpl->memoryConsolidation = std::max(0.0f, pImpl->memoryConsolidation - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionStrength = std::max(0.0f, pImpl->attentionStrength - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Enhance memory consolidation
    pImpl->memoryConsolidation = std::min(pImpl->peak, pImpl->memoryConsolidation + 0.2f);
    
    // Also boost attention
    pImpl->attentionStrength = std::min(pImpl->peak, pImpl->attentionStrength + 0.1f);
    
    // Increase overall cholinergic tone
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.15f);
}

} // namespace nlm
