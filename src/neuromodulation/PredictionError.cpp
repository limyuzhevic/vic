#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(4.0f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PRED_ERROR";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error increases plasticity for learning from prediction errors
    return 1.0f + 0.8f * std::abs(pImpl->level);
}

void PredictionError::update(TimestepDuration dt) {
    // Time-dependent adaptation to maintain sensitivity to prediction errors
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.2f * std::cos(pImpl->adaptationTime * 0.05f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.03f;
    
    // Decay towards baseline (errors decay over time)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void PredictionError::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
}

} // namespace nlm
