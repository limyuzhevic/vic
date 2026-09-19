#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionLevel(0.0f) {}
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
    // Acetylcholine enhances plasticity for attended stimuli
    // Higher ACh increases memory encoding and attention
    return 1.0f + 0.5f * pImpl->attentionLevel;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Slowly decay attention
    pImpl->attentionLevel = std::max(0.0f, pImpl->attentionLevel - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemory(float strength) {
    // Enhance memory encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
}

void Acetylcholine::regulateAttention(float focus) {
    // Regulate attention level
    pImpl->attentionLevel = std::clamp(focus, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionLevel = 0.0f;
}

} // namespace nlm
