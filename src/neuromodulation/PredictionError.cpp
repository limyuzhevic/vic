#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // For advanced prediction error computation
    std::vector<float> gradientHistory;
    float temporalDerivative;
    float predictionConfidence;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f), 
             temporalDerivative(0.0f), predictionConfidence(1.0f) {}
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
    // Store previous values for computing temporal derivatives
    float prevError = pImpl->error;
    float prevPredicted = pImpl->predictedValue;
    
    // Compute new prediction error
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Add to history
    pImpl->history.push_back(pImpl->error);
    
    // Keep history size manageable
    const size_t maxHistorySize = 1000;
    if (pImpl->history.size() > maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Compute temporal derivative (rate of change of prediction error)
    if (std::abs(actual - prevPredicted) > 0.001f) {
        pImpl->temporalDerivative = (actual - prevPredicted) / std::abs(actual - prevPredicted);
    }
    
    // Update prediction confidence based on error magnitude
    // Higher error magnitude suggests less confidence
    pImpl->predictionConfidence = std::exp(-std::abs(pImpl->error) * 2.0f);
    
    // Log if error is significant
    if (std::abs(pImpl->error) > 0.5f) {
        NLM_LOG_INFO("Large prediction error detected: " + std::to_string(pImpl->error));
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Smoothly update prediction with exponential moving average
    pImpl->predictedValue = pImpl->predictedValue * 0.9f + newPrediction * 0.1f;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->gradientHistory.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getTemporalDerivative() const {
    return pImpl->temporalDerivative;
}

float PredictionError::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

} // namespace nlm
