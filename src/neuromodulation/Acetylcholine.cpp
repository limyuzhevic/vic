#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionBoost(1.0f), memoryBoost(1.0f) {}
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
    // ACh modulates attention and working memory
    // Higher ACh increases attention focus, enhances working memory
    float attentionMod = 1.0f + pImpl->level * 0.5f;
    
    // Reset to reasonable bounds
    return std::clamp(attentionMod, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slower decay than dopamine
    pImpl->attentionBoost = pImpl->attentionBoost * 0.9f + 1.0f * 0.1f;
    pImpl->memoryBoost = pImpl->memoryBoost * 0.9f + 1.0f * 0.1f;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Signal increased attention due to novel stimuli or goals
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate * 1.5f);
}

void Acetylcholine::enhanceMemory(float memoryPriority) {
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryPriority * pImpl->releaseRate);
}

} // namespace nlm