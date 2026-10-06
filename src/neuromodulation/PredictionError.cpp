#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Enhanced prediction error calculation with robustness
    if (std::isnan(predicted) || std::isinf(predicted) ||
        std::isnan(actual) || std::isinf(actual)) {
        NLM_LOG_WARNING("Invalid prediction values: predicted=" + std::to_string(predicted) +
                       " actual=" + std::to_string(actual));
        predicted = actual = 0.0f;
    }
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Store with magnitude and sign tracking
    pImpl->history.push_back(pImpl->error);
    
    // Maintain rolling window of recent errors
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Log significant prediction errors
    float magnitude = std::abs(pImpl->error);
    if (magnitude > 0.5f) {
        NLM_LOG_INFO("Large prediction error detected: " + std::to_string(pImpl->error) +
                    " (predicted=" + std::to_string(predicted) +
                    " actual=" + std::to_string(actual) + ")");
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Enhanced prediction update with temporal smoothing
    if (std::isnan(newPrediction) || std::isinf(newPrediction)) {
        NLM_LOG_WARNING("Invalid new prediction: " + std::to_string(newPrediction));
        return;
    }
    
    // Exponential smoothing for prediction updates
    float alpha = 0.3f; // Smoothing factor
    pImpl->predictedValue = pImpl->predictedValue * (1.0f - alpha) + newPrediction * alpha;
    
    // Reduce current error as prediction improves
    pImpl->error = pImpl->actualValue - pImpl->predictedValue;
}

void PredictionError::clearHistory() {
    // More robust history clearing
    pImpl->history.clear();
    
    NLM_LOG_DEBUG("Prediction error history cleared");
}

float PredictionError::getMagnitude() const {
    // Enhanced magnitude calculation with confidence weighting
    float magnitude = std::abs(pImpl->error);
    
    // Scale magnitude based on prediction confidence (if implemented)
    // For now, return raw absolute error
    return magnitude;
}

} // namespace nlm
