#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal modulation state
    float arousalLevel;
    
    // Working memory enhancement state
    float workingMemoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(0.0f), workingMemoryBoost(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates attention and arousal, affecting plasticity indirectly
    return 0.8f + 0.2f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal and working memory boost
    pImpl->arousalLevel *= (1.0f - dt * 0.03f);
    pImpl->workingMemoryBoost *= (1.0f - dt * 0.02f);
}

void Norepinephrine::increaseArousal(float strength) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + strength * 0.5f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Norepinephrine::enhanceWorkingMemory(float strength) {
    pImpl->workingMemoryBoost = std::min(1.0f, pImpl->workingMemoryBoost + strength);
    // Modulate level for sustained effect
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

} // namespace nlm
