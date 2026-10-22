#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Signal detection enhancement
    float signalDetection;
    
    // Arousal and vigilance
    float arousal;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(2.0f)
        , decayRate(0.02f)
        , releaseRate(3.0f)
        , signalDetection(1.0f)
        , arousal(0.5f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getPlasticityFactor() const {
    // NE increases signal detection and arousal, enhancing focused attention
    float basePlasticity = 0.9f + 0.6f * pImpl->level;
    return std::clamp(basePlasticity, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Time-dependent adaptation to maintain optimal arousal
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.4f * std::tanh(pImpl->adaptationTime * 0.05f - 1.0f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.03f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slowly adapt parameters
    pImpl->signalDetection = std::clamp(1.0f + 0.8f * pImpl->currentAdaptation, 0.5f, 2.0f);
    pImpl->arousal = std::clamp(0.5f + 0.5f * pImpl->currentAdaptation, 0.2f, 1.0f);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
}

} // namespace nlm
