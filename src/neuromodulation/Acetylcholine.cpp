#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.5f), decayRate(0.05f), releaseRate(1.0f), attentionBoost(2.0f) {}
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
    // ACh enhances plasticity for attended information
    // Strong attention increases synaptic modification probability
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays with time, modulated by attention demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float focus) {
    // ACh bursts when attention is focused on salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus * pImpl->releaseRate);
}

void Acetylcholine::enhanceWorkingMemory(float currentActivity) {
    // ACh enhances working memory maintenance through theta modulation
    float enhancement = currentActivity * pImpl->attentionBoost;
    pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * 0.1f);
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

} // namespace nlm
