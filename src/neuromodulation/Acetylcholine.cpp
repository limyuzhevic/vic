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
    float attentionGain;
    float memoryConsolidationFactor;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , attentionGain(1.0f), memoryConsolidationFactor(1.0f) {}
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
    // ACh enhances synaptic plasticity and memory consolidation
    // Higher ACh increases LTP and strengthens memory traces
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryConsolidationFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience) {
    // Burst of ACh for salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate * pImpl->attentionGain);
}

void Acetylcholine::enhanceMemory() {
    // Enhance memory consolidation
    pImpl->memoryConsolidationFactor = 1.0f + 0.5f * pImpl->level;
}

const float& Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::max(0.1f, std::min(5.0f, gain));
}

} // namespace nlm
