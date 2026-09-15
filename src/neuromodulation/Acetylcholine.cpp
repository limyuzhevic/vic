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
    
    // Effects on attention
    float attentionGain;
    float attentionThreshold;
    
    // Effects on memory consolidation
    float consolidationBoost;
    float consolidationThreshold;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f),
             attentionGain(1.0f), attentionThreshold(0.5f),
             consolidationBoost(1.0f), consolidationThreshold(0.3f) {}
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
    // TODO PHASE 2: Implement real ACh-modulated plasticity factor
    // ACh typically increases stability and reduces plasticity
    return 1.0f - 0.3f * pImpl->level;  // High ACh = less plasticity
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // TODO PHASE 2: Implement attention signaling
    // ACh increases with focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // TODO PHASE 2: Implement memory consolidation signaling
    // ACh promotes memory consolidation
    if (pImpl->level > pImpl->consolidationThreshold) {
        // Boost consolidation activity
        pImpl->level += 0.1f;
        if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    }
}

} // namespace nlm