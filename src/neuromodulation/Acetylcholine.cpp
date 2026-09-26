#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Acetylcholine has complex effects on plasticity
    // It enhances attention and working memory encoding
    return 0.8f + pImpl->level * 0.4f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float attention) {
    // TODO PHASE 2: Implement real attention signaling
    // PLACEHOLDER: Burst of acetylcholine on attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.6f);
}

void Acetylcholine::signalMemoryConsolidation(float memoryScore) {
    // TODO PHASE 2: Implement real memory consolidation signaling
    // PLACEHOLDER: Acetylcholine responds to memory consolidation
    pImpl->level = std::max(0.0f, pImpl->level + memoryScore * pImpl->releaseRate * 0.4f);
}

} // namespace nlm
