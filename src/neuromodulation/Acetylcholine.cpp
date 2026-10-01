// Acetylcholine: Attention and memory consolidation
// TODO PHASE 2

#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryBoost;
    float baseline;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), attentionBoost(1.5f), memoryBoost(2.0f), 
             baseline(0.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // Acetylcholine enhances attention and memory consolidation
    // Higher ACh increases the effectiveness of attention-based learning
    return 1.0f + pImpl->level * (pImpl->attentionBoost - 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float strength) {
    // Boost attention when receiving attentional signals
    pImpl->level = std::min(pImpl->level + strength * pImpl->releaseRate, 1.0f);
}

void Acetylcholine::enhanceMemory(float memoryStrength) {
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->level + memoryStrength * pImpl->memoryBoost * pImpl->releaseRate, 1.0f);
}

} // namespace nlm