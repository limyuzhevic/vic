// Implementation of Acetylcholine neuromodulator
#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                     // Current ACh concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float attentionStrength;         // Effect on attention
    float memoryConsolidationRate;   // Effect on memory consolidation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             attentionStrength(1.0f), memoryConsolidationRate(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

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
    // ACh increases plasticity for attention-grabbing stimuli
    // Higher ACh -> more selective attention -> focused plasticity
    float attentionFactor = 0.5f + 0.5f * pImpl->level;
    float memoryFactor = 0.3f + 0.7f * pImpl->memoryConsolidationRate;
    return attentionFactor * memoryFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays over time and naturally fluctuates
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float strength) {
    // Stimuli grab attention -> increase ACh
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryEvent(float importance) {
    // Important events boost memory consolidation via ACh
    pImpl->level = std::min(pImpl->peak, pImpl->level + importance * pImpl->memoryConsolidationRate * 0.5f);
}

} // namespace nlm