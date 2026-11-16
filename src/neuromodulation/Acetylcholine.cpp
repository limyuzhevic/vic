#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float attention;
    float memoryEnhancement;
    float decayRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), attention(0.0f), memoryEnhancement(0.0f), decayRate(0.05f) {}
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
    // ACh affects attention and memory
    pImpl->attention = level * 0.5f;
    pImpl->memoryEnhancement = level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related learning
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attention = pImpl->level * 0.5f;
    pImpl->memoryEnhancement = pImpl->level * 0.3f;
}

void Acetylcholine::enhanceAttention(float attentionStrength) {
    pImpl->level = std::min(1.0f, pImpl->level + attentionStrength);
    pImpl->attention = std::min(1.0f, pImpl->attention + attentionStrength * 0.5f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attention;
}

} // namespace nlm