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
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher acetylcholine increases working memory consolidation
    // Based on: ACh enhances memory consolidation and attention
    return 0.5f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline, respond to attention/arousal
    // ACh modulates attention and working memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // TODO PHASE 2: Implement real attention signaling
    // PLACEHOLDER: Burst of acetylcholine on attentional demands
    // ACh signals attention demands and arousal level
    float burst = attentionStrength * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Acetylcholine::signalMemoryPriority() {
    // TODO PHASE 2: Implement memory consolidation signaling
    // PLACEHOLDER: Acetylcholine promotes memory consolidation
    // Higher levels enhance memory retention
    // This would signal to other systems that memory consolidation should be prioritized
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
}

void Acetylcholine::reset() {
    // Reset acetylcholine to baseline
    setLevel(0.0f);
}