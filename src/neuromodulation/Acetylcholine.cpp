#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

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
    // Acetylcholine enhances plasticity for memory consolidation
    // Higher ACh increases plasticity for salient experiences
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decay towards baseline, burst on attention
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // ACh release during attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalMemory(float memoryStrength) {
    // ACh release during memory encoding and consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm
