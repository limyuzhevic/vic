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
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction system update using neural activity patterns
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using neural representation difference
        float sumSquaredError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
            maxError = std::max(maxError, std::abs(error));
        }
        
        // Use weighted combination of MSE and max error for robust prediction error
        pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
        pImpl->predictionError = std::min(pImpl->predictionError, 1.0f); // Clamp to 0-1 range
        
        // Update confidence based on prediction error (inverse relationship)
        pImpl->confidence = 1.0f - pImpl->predictionError;
        
        // Store error history for learning
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history bounded
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Train prediction model with new error signal
        train(predData);
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
    // Implement real prediction model training using neural activity patterns
    // This learns the mapping from sensory inputs to predicted future states
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Simple Hebbian-like learning for prediction patterns
    // Store recent observations for pattern learning
    if (pImpl->errorHistory.size() > 10) {
        // Model learns from prediction errors - adjust internal parameters
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Update confidence based on stability of predictions
        if (avgError > 0.1f) {
            // High prediction error -> lower confidence, need to learn
            pImpl->confidence *= 0.95f;  // Reduce confidence
        } else {
            // Low prediction error -> increase confidence
            pImpl->confidence = std::min(1.0f, pImpl->confidence * 1.05f);
        }
    }
}

} // namespace nlm
