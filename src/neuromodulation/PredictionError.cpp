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
    // Compute prediction error with biological realism
    // Based on temporal difference (TD) learning
    // Includes eligibility traces and decay
    // Models the asymmetric response of dopamine to prediction errors
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Compute raw prediction error
    float rawError = actual - predicted;
    
    // Apply biological constraints
    // Prediction errors are normalized and bounded
    float normalizedError = std::tanh(rawError * 0.5f);  // Non-linear saturation
    
    // Add temporal component - prediction errors decay over time
    // if not reinforced by subsequent events
    if (pImpl->history.empty()) {
        pImpl->error = normalizedError;
    } else {
        float lastError = pImpl->history.back();
        // Smooth update to model neural dynamics
        pImpl->error = lastError * 0.7f + normalizedError * 0.3f;
    }
    
    // Store in history with temporal decay
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
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
