#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention-related parameters
    float attentionGain;
    
    // Memory-related parameters
    float memoryEncodingStrength;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f),
             attentionGain(1.0f), memoryEncodingStrength(1.0f) {}
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
    // Update attention and memory based on level
    pImpl->attentionGain = 1.0f + pImpl->level * 2.0f;
    pImpl->memoryEncodingStrength = 1.0f + pImpl->level * 1.5f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory formation
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryEncodingStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(float amount) {
    // Increase attention modulation
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryEncoding(float strength) {
    // Enhance memory encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm