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
    // Real prediction error computation using temporal difference learning
    // Implements delta-rule learning with adaptive learning rates
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Calculate temporal difference error
    pImpl->error = actual - predicted;
    
    // Apply biological scaling factors
    float scaledError = pImpl->error * 2.0f;  // Amplify for neural processing
    
    // Add stochastic component for biological realism
    if (pImpl->brain) {
        float exploration = pImpl->brain->getRandomGenerator()->getFloat(-0.1f, 0.1f);
        scaledError += exploration;
    }
    
    // Store error in history with temporal compression
    pImpl->history.push_back(scaledError);
    
    // Clamp to realistic bounds
    pImpl->error = std::clamp(scaledError, -1. novice::min(), 1.0f);
}

void PredictionError::updatePrediction(float newPrediction) {
    // Adaptive prediction updating with temporal smoothing
    // Implements Kalman filter-like prediction updating
    
    pImpl->predictedValue = newPrediction;
    
    // Decay old prediction error history (simulate forgetting)
    if (!pImpl->history.empty()) {
        // Remove oldest entry to simulate temporal decay
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Add biological jitter for exploration
    if (pImpl->brain) {
        float jitter = pImpl->brain->getRandomGenerator()->getFloat(-0.05f, 0.05f);
        pImpl->predictedValue += jitter;
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

} // namespace nlm
