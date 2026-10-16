#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PHASE 2: Implement predictive coding using internal models and expectations
    // For now, create a simple prediction based on temporal smoothing and noise
    auto predicted = currentState.clone();
    auto& predData = predicted->getData();
    auto& currData = currentState.getData();
    
    if (!currData.empty()) {
        // Apply temporal smoothing (autocorrelation)
        float decayFactor = 0.7f; // How much of previous state to retain
        for (size_t i = 0; i < currData.size(); ++i) {
            // Smooth current state with some noise for prediction
            predData[i] = currData[i] * decayFactor;
            
            // Add small random fluctuations for exploration
            predData[i] += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
        }
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PHASE 2: Implement Bayesian prediction error with adaptive weighting
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error using weighted difference
        float sumSquaredError = 0.0f;
        float sumAbsoluteError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
            sumAbsoluteError += std::abs(diff);
        }
        
        size_t count = predData.size();
        pImpl->predictionError = sumSquaredError / count; // RMSE-style
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error (lower error = higher confidence)
        float maxError = 1.0f; // Normalize error to [0, 1] assuming reasonable sensor ranges
        float normalizedError = std::clamp(pImpl->predictionError / maxError, 0.0f, 1.0f);
        pImpl->confidence = 1.0f - normalizedError; // Inverse relationship
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f); // Keep minimum confidence
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
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

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm
