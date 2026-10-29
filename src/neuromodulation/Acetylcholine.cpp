#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionModulation(0.0f) {}
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
    // Acetylcholine modulates attention and memory consolidation
    return 1.0f + 0.2f * pImpl->level + pImpl->attentionModulation * 0.8f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine increases with attention, decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionModulation = std::max(0.0f, pImpl->attentionModulation - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Acetylcholine responds to attention and salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
    pImpl->attentionModulation = std::min(1.0f, pImpl->attentionModulation + attention * 0.5f);
}

void Acetylcholine::signalLearning(float learning) {
    // Acetylcholine modulates learning and memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + learning * pImpl->releaseRate * 0.5f);
}

} // namespace nlm