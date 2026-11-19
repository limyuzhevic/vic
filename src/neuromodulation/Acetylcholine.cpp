#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionGain(1.5f), memoryConsolidation(2.0f) {}
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
    // Acetylcholine modulates plasticity for attention and memory consolidation
    // Higher ACh increases plasticity for attended information and memory consolidation
    return 1.0f + pImpl->level * (pImpl->attentionGain - 1.0f) * pImpl->memoryConsolidation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline and resets with learning events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Attention signal increases ACh level
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
}

void Acetylcholine::signalLearningEvent(float learningStrength) {
    // Learning events (successful memory encoding) trigger ACh burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + learningStrength * pImpl->memoryConsolidation * pImpl->releaseRate);
}

} // namespace nlm
