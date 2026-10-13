#include "Acetylcholine.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh effects on attention
    float attentionGain;
    float memoryConsolidationStrength;
    
    // Targeting
    float attentionThreshold;
    float memoryPriority;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , attentionGain(1.0f), memoryConsolidationStrength(0.5f)
           , attentionThreshold(0.3f), memoryPriority(0.7f) {}
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
    // Higher ACh improves attention and memory stability
    return 1.0f + pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate attention threshold based on ACh level
    // Lower ACh = harder to focus (higher threshold)
    // Higher ACh = easier to focus (lower threshold)
    pImpl->attentionThreshold = 0.3f - pImpl->level * 0.2f;
    
    // Modulate memory consolidation priority
    // Higher ACh = prioritize memory consolidation
    pImpl->memoryPriority = 0.5f + pImpl->level * 0.5f;
}

void Acetylcholine::signalAttentionBoost(float boost) {
    // Boost attention based on cholinergic input
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain + boost * 0.5f);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // Enhance memory consolidation
    pImpl->memoryConsolidationStrength = std::min(1.0f, pImpl->memoryConsolidationStrength + strength * 0.3f);
}

float Acetylcholine::getAttentionThreshold() const {
    return pImpl->attentionThreshold;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationStrength;
}

float Acetylcholine::getMemoryPriority() const {
    return pImpl->memoryPriority;
}

} // namespace nlm