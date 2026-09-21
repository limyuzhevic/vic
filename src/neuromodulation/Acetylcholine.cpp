#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(0.5f) {}
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
    // Acetylcholine modulates memory consolidation and attention
    // Higher ACh increases attention and memory consolidation
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline, release on attention events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // TODO PHASE 2: Implement attention signaling
    // PLACEHOLDER: Acetylcholine release on attention events
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation(float consolidationStrength) {
    // TODO PHASE 2: Implement memory consolidation signaling
    // PLACEHOLDER: Acetylcholine enhances memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

} // namespace nlm