#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float currentError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : currentError(0.0f), confidence(1.0f), errorHistory() {}
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<class PredictionError> PredictionSystem::predictNextState(const class SensoryInput& currentState) {
    // Simple prediction: anticipate similar sensory patterns based on current state
    // TODO: Implement proper predictive coding
    
    auto error = std::make_unique<PredictionError>();
    
    // Simple heuristic: predict that patterns will persist with some noise
    const auto& data = currentState.getData();
    if (!data.empty()) {
        float predictedValue = data[0];  // Simple single-value prediction
        error->setPredicted(predictedValue);
        error->setActual(data[0]);  // Same for now (no change)
        error->setError(std::abs(predictedValue - data[0]));
    }
    
    pImpl->currentError = error->getError();
    pImpl->confidence = std::max(0.0f, 1.0f - std::abs(pImpl->currentError));
    
    return error;
}

void PredictionSystem::updatePredictions(const class SensoryInput& predicted, const class SensoryInput& actual) {
    // Update prediction confidence based on prediction error
    // TODO: Implement real prediction error calculation
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (!predData.empty() && !actualData.empty()) {
        float predValue = predData[0];
        float actualValue = actualData[0];
        
        float error = std::abs(predValue - actualValue);
        pImpl->currentError = error;
        pImpl->confidence = std::max(0.0f, 1.0f - error);
        
        pImpl->errorHistory.push_back(error);
        
        // Keep history limited
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->currentError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const class SensoryInput& observation) {
    // Update prediction model based on observation
    // TODO: Implement real training with neural networks
    
    // Simple update: reduce error with each training example
    const auto& data = observation.getData();
    if (!data.empty()) {
        float currentError = pImpl->currentError;
        float learningRate = 0.01f;
        
        // Reduce error based on observation
        pImpl->currentError = currentError * (1.0f - learningRate);
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->currentError);
    }
}

} // namespace nlm
