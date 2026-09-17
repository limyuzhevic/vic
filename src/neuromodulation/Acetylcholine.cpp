#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

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
    // ACh modulates attention and memory consolidation
    return 1.0f + pImpl->level * 0.5f;  // Enhances plasticity for attended information
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slower decay, release on novel stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // ACh bursts when attention is focused
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::signalNovelty(float novelty) {
    // ACh responds to novel stimuli (before learning)
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.5f);
}

} // namespace nlm