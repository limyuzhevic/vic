#include "Norepinephrine.hpp"
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
    float signalDetectionGain;
    bool encodingEnhanced;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(0.0f), signalDetectionGain(1.0f), encodingEnhanced(false) {}
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
    // NE enhances plasticity during encoding, suppresses during recall
    if (pImpl->encodingEnhanced) {
        return 1.2f; // Enhanced encoding plasticity
    } else {
        return 0.8f; // Suppressed recall plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal level
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - pImpl->decayRate * 0.5f * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
}

void Norepinephrine::enhanceSignalDetection(float gain) {
    pImpl->signalDetectionGain = std::clamp(gain, 1.0f, 2.0f);
}

void Norepinephrine::modulateEncoding(bool enhance) {
    pImpl->encodingEnhanced = enhance;
}

} // namespace nlm