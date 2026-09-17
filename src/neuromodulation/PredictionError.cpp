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
    // Compute prediction error using temporal difference formula
    // δ = r + γV(s') - V(s) ≈ actual - predicted
    // 
    // Biological basis:
    // - Dopamine encodes reward prediction error (RPE)
    // - Prediction error drives learning in both dopamine and curiosity systems
    // - Error magnitude influences plasticity
    
    // Compute raw prediction error
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Apply learning rate for temporal difference
    float tdError = pImpl->error * pImpl->learningRate;
    
    // Update predicted value (TD update rule)
    pImpl->predictedValue += tdError;
    
    // Store in history
    pImpl->history.push_back(pImpl->error);
    
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction using temporal difference learning
    float delta = newPrediction - pImpl->predictedValue;
    pImpl->predictedValue = newPrediction;
    
    // Prediction error drives neuromodulatory signals
    pImpl->error = delta;
    
    // Store in history
    pImpl->history.push_back(pImpl->error);
    
    if (pImpl->history.size() > 100) {
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
    return std::abs(pImpl->error);
}

} // namespace nlm
