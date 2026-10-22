#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood regulation
    float mood;
    
    // Impulsivity modulation
    float impulsivity;
    
    // Behavioral flexibility
    float flexibility;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    // Prediction error integration
    float predictionErrorHistory[10];
    size_t predictionErrorIndex;
    float predictionErrorSum;
    
    Impl() 
        : level(0.0f)
        , baseline(0.3f)
        , peak(1.5f)
        , decayRate(0.03f)
        , releaseRate(2.5f)
        , mood(0.5f)
        , impulsivity(1.0f)
        , flexibility(1.0f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f)
        , predictionErrorIndex(0)
        , predictionErrorSum(0.0f) {
        std::fill(std::begin(predictionErrorHistory), std::end(predictionErrorHistory), 0.0f);
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getPlasticityFactor() const {
    // 5-HT reduces overall plasticity but increases stability
    float basePlasticity = 1.0f - 0.4f * pImpl->level;
    return std::clamp(basePlasticity, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Time-dependent adaptation for mood stabilization
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.2f * std::cos(pImpl->adaptationTime * 0.02f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.02f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slowly adapt parameters based on mood state
    pImpl->mood = std::clamp(0.5f + 0.3f * pImpl->currentAdaptation, 0.2f, 0.8f);
    pImpl->impulsivity = std::clamp(1.0f - 0.8f * pImpl->mood, 0.2f, 1.5f);
    pImpl->flexibility = std::clamp(1.0f - 0.2f * std::abs(pImpl->mood - 0.5f), 0.7f, 1.5f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
    std::fill(std::begin(pImpl->predictionErrorHistory), std::end(pImpl->predictionErrorHistory), 0.0f);
    pImpl->predictionErrorIndex = 0;
    pImpl->predictionErrorSum = 0.0f;
}

} // namespace nlm
