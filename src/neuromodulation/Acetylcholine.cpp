#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation
    float attentionGain;
    
    // Memory consolidation
    float memoryConsolidation;
    
    // Learning rate modulation
    float learningRateMod;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionGain(1.0f)
        , memoryConsolidation(1.0f)
        , learningRateMod(1.2f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances both working memory and attention, increasing overall learning
    return 0.8f + 0.4f * pImpl->level * pImpl->learningRateMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Time-dependent adaptation to maintain optimal attention
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.3f * std::sin(pImpl->adaptationTime * 0.1f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.05f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slowly adapt parameters
    pImpl->attentionGain = std::clamp(1.0f + 0.5f * pImpl->currentAdaptation, 0.5f, 2.0f);
    pImpl->memoryConsolidation = std::clamp(1.0f + 0.3f * pImpl->currentAdaptation, 0.8f, 1.5f);
    pImpl->learningRateMod = std::clamp(1.0f + 0.2f * pImpl->currentAdaptation, 0.9f, 1.5f);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
}

} // namespace nlm
