#include "PredictionError.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    float currentError;
    float predictedValue;
    float learningRate;
    float decayRate;
    std::vector<float> history;
    static constexpr size_t MAX_HISTORY = 100;
    
    Impl() : currentError(0.0f), predictedValue(0.0f), learningRate(0.01f), decayRate(0.1f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    // Store brain reference if needed for future integration
    // Currently placeholder implementation
}

float PredictionError::getError() const {
    return pImpl->currentError;
}

void PredictionError::computeError(float predicted, float actual) {
    // Compute prediction error as difference between predicted and actual values
    // Sign indicates direction of error (positive = predicted too low, negative = predicted too high)
    
    pImpl->predictedValue = predicted;
    pImpl->currentError = actual - predicted;
    
    // Store in history
    pImpl->history.push_back(pImpl->currentError);
    if (pImpl->history.size() > Impl::MAX_HISTORY) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update predicted value
    pImpl->predictedValue = newPrediction;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentError);
    if (pImpl->history.size() > Impl::MAX_HISTORY) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    // Return absolute value of error for neuromodulation
    return std::abs(pImpl->currentError);
}

} // namespace nlm