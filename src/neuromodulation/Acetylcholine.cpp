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
    float memoryEnhance;
    float attentionFocus;
    float synapticModulation;
    
    Impl() : level(0.0f), baseline(0.03f), peak(1.0f),
             decayRate(0.06f), releaseRate(2.5f),
             memoryEnhance(1.0f), attentionFocus(1.0f),
             synapticModulation(1.0f) {}
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
    // Acetylcholine modulates memory consolidation and attention
    // Higher ACh enhances memory encoding and attention selectivity
    float memoryMod = 1.0f + pImpl->memoryEnhance * pImpl->level * 0.8f;
    float attentionMod = 1.0f + pImpl->attentionFocus * pImpl->level * 0.5f;
    float synapticMod = 1.0f + pImpl->synapticModulation * pImpl->level * 0.3f;
    
    float factor = memoryMod * attentionMod * synapticMod;
    return std::clamp(factor, 0.8f, 2.5f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update associated modulators
    pImpl->memoryEnhance = std::clamp(1.5f + pImpl->level * 1.2f, 1.2f, 3.0f);
    pImpl->attentionFocus = std::clamp(1.2f + pImpl->level * 0.8f, 1.0f, 2.5f);
    pImpl->synapticModulation = std::clamp(1.0f + pImpl->level * 0.5f, 1.0f, 2.0f);
}

void Acetylcholine::signalAttention(float attentionDelta) {
    // ACh responds to salient stimuli and rewards
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionDelta * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryEncoding(float baseEncoding) {
    // ACh enhances memory encoding based on current level
    float encodingEnhancement = baseEncoding * pImpl->memoryEnhance;
}

void Acetylcholine::focusAttention(float baseFocus, float distractionLevel) {
    // ACh increases focus and reduces distraction effects
    float focusMod = baseFocus * pImpl->attentionFocus;
    float distractionMod = distractionLevel * (1.0f - pImpl->level * 0.7f);
}

} // namespace nlm