#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention and memory modulation
    float attentionStrength;
    float memoryConsolidationStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionStrength(1.0f), memoryConsolidationStrength(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    pImpl->attentionStrength = attentionStrength;
}

void Acetylcholine::modulateMemoryConsolidation(float strength) {
    pImpl->memoryConsolidationStrength = strength;
}

} // namespace nlm