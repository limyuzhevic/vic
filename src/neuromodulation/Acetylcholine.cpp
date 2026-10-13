// Acetylcholine.cpp
// Implementation of Acetylcholine neuromodulator for attention and memory

#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionGain;
    float memoryConsolidationRate;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), attentionGain(1.0f), memoryConsolidationRate(1.0f),
             baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
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
    // Acetylcholine modulates both plasticity rates and memory consolidation
    // Higher ACh increases attention and memory consolidation
    return 1.0f + pImpl->level * 2.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline (slow decay)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(boost::string_view stimulus) {
    // Signal new attention-grabbing stimulus
    // ACh bursts are triggered by novel or salient stimuli
    float salience = computeSalience(stimulus);
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * 2.0f);
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Temporarily boost memory consolidation rate
    // This is used during learning events
    // In a full implementation, this would affect synaptic tagging
    // For now, just increment the level to maintain consolidation state
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::computeSalience(boost::string_view stimulus) {
    // Simple salience computation based on stimulus properties
    // In a full implementation, this would use sensory features
    // For now, return a fixed value based on stimulus length
    return std::min(1.0f, static_cast<float>(stimulus.size()) / 10.0f);
}

} // namespace nlm
