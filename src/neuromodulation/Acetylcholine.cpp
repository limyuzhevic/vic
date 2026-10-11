#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionEnhancement;
    float memoryEncoding;
    float memoryReverie;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), attentionEnhancement(1.0f), 
             memoryEncoding(1.0f), memoryReverie(0.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // Acetylcholine enhances plasticity for memory encoding
    float attentionMod = 1.0f + pImpl->attentionEnhancement * 0.5f;
    float memoryMod = 1.0f + pImpl->memoryEncoding * 0.3f;
    return std::min(2.0f, attentionMod * memoryMod);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

void Acetylcholine::enhanceAttention(float enhancement) {
    pImpl->attentionEnhancement = std::clamp(enhancement, 0.0f, 1.0f);
}

void Acetylcholine::enhanceMemoryEncoding(float strength) {
    pImpl->memoryEncoding = std::clamp(strength, 0.0f, 1.0f);
}

void Acetylcholine::setMemoryReverie(bool reverie) {
    pImpl->memoryReverie = reverie ? 1.0f : 0.0f;
}

} // namespace nlm