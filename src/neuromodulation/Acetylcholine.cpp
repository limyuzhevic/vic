#include "Neuromodulator.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // ACh enhances plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Burst of acetylcholine during attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    // ACh responds to novelty, enhancing encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * 0.5f * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryConsolidation(float consolidationStrength) {
    // Enhance memory consolidation during rest
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm