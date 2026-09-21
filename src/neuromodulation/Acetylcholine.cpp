#include "Neuromodulator.hpp"
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
    // TODO PHASE 2: Implement real acetylcholine-modulated plasticity factor
    // PLACEHOLDER: Higher acetylcholine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float boost) {
    // TODO PHASE 2: Implement attention boost signaling
    // PLACEHOLDER: Burst of acetylcholine on attention demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // TODO PHASE 2: Implement memory consolidation signaling
    // PLACEHOLDER: Acetylcholine supports memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm