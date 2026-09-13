#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f),
             releaseRate(1.0f), arousalLevel(1.0f), vigilance(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Phase 2: Real norepinephrine-modulated plasticity
    // NE enhances signal-to-noise ratio and promotes perseverative learning
    float modulation = 0.9f + 0.6f * pImpl->level;  // Range: 0.9 - 1.5
    modulation = std::clamp(modulation, 0.5f, 2.0f);
    return modulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics: very slow decay for sustained arousal
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::signalArousal(float stimulusIntensity, float novelty) {
    // Real arousal signaling: proportional to stimulus intensity
    float burst = std::min(1.0f, stimulusIntensity * pImpl->arousalLevel);
    // Novelty has multiplicative effect on arousal
    burst *= (1.0f + novelty * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Norepinephrine::enhanceVigilance(const std::vector<float>& inputPattern,
                                     const std::vector<float>& threshold) {
    // Enhance vigilance based on pattern matching
    float patternMatch = 0.0f;
    for (size_t i = 0; i < inputPattern.size() && i < threshold.size(); ++i) {
        patternMatch += std::abs(inputPattern[i] - threshold[i]);
    }
    patternMatch /= static_cast<float>(std::min(inputPattern.size(), threshold.size()));
    
    if (patternMatch > 0.7f) {
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.2f);
    }
}

} // namespace nlm
