#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float baseline;
    float decayRate;
    float learningRate;
    float predictionErrorSignal;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), learningRate(0.1f), predictionErrorSignal(0.0f) {}
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
    // Prediction error directly modulates plasticity
    return 1.0f + pImpl->level * 0.5f;
}

void PredictionError::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void PredictionError::computePredictionError(float reward, float predictedReward) {
    // Prediction error = actual reward - expected reward
    float error = reward - predictedReward;
    pImpl->predictionErrorSignal = error;
    pImpl->level = std::max(0.0f, std::min(1.0f, std::abs(error) * pImpl->learningRate));
}

float PredictionError::getPredictionErrorSignal() const {
    return pImpl->predictionErrorSignal;
}

} // namespace nlm