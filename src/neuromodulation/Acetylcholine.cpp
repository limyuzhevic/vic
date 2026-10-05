#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float attentionGain;
    float memoryConsolidationRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), 
             attentionGain(0.8f), memoryConsolidationRate(0.6f) {}
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
    // ACh increases plasticity for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Signal increased attention demand
    pImpl->level = std::min(1.0f, pImpl->level + attention * 0.5f);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // Enhance memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->memoryConsolidationRate * 0.5f);
}

} // namespace nlm
