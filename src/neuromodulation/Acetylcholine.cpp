#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryEncodingStrength;
    
    Impl() 
        : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , attentionModulation(1.0f), memoryEncodingStrength(1.0f) {}
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
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionModulation = 0.5f + 0.5f * pImpl->level;
    pImpl->memoryEncodingStrength = 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::modulateMemoryEncoding(float attention, float plasticity) {
    pImpl->memoryEncodingStrength = std::clamp(attention * plasticity, 0.0f, 2.0f);
}

void Acetylcholine::modulateSpatialAttention(float focusArea, float peripherySuppression) {
    pImpl->attentionModulation = std::clamp(focusArea - peripherySuppression * 0.5f, 0.0f, 2.0f);
}

} // namespace nlm
