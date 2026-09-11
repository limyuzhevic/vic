#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorValue;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), predictionErrorValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error affects plasticity by signaling unexpected outcomes
    return 0.1f + 0.9f * pImpl->level;
}

void PredictionError::update(TimestepDuration dt) {
    // Decay prediction error over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void PredictionError::computeError(float predicted, float actual, float confidence) {
    // Compute prediction error as absolute difference weighted by confidence
    float error = std::abs(predicted - actual) * confidence;
    
    // Level responds to prediction error
    if (error > pImpl->level) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
    }
    
    pImpl->predictionErrorValue = error;
}

float PredictionError::getPredictionErrorValue() const {
    return pImpl->predictionErrorValue;
}

} // namespace nlm