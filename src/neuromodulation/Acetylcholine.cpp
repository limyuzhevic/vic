#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionHistory;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             attentionHistory(0.0f), memoryBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related learning
    return 0.5f + 0.5f * pImpl->level + pImpl->memoryBoost * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay attention history and memory boost
    pImpl->attentionHistory *= 0.95f;
    pImpl->memoryBoost *= 0.9f;
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Strong attention signals boost ACh level for memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
    pImpl->attentionHistory += attentionSignal * 0.5f;
    
    // Boost memory-related plasticity
    pImpl->memoryBoost = std::min(1.0f, pImpl->memoryBoost + attentionSignal * 0.3f);
}

} // namespace nlm
