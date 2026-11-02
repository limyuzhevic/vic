#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.2f), releaseRate(1.0f), attentionBoost(2.0f) {}
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
    // PLACEHOLDER: Higher acetylcholine increases plasticity and stabilizes memory
    // ACh enhances LTP and reduces interference, improving learning efficiency
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline, modulated by attention demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float boost) {
    // TODO PHASE 2: Implement real attention signaling
    // PLACEHOLDER: Acetylcholine bursts on attentional focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    // TODO PHASE 2: Implement real memory consolidation signaling
    // PLACEHOLDER: Acetylcholine supports memory consolidation during wakefulness
    pImpl->level = std::max(pImpl->level, memoryStrength * 0.8f);
}

} // namespace nlm
