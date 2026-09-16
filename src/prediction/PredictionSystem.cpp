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
    // Use neural activity patterns as predictions - integrate with brain's neural substrate
    // For now, create a version that can access brain state through the pointer
    // This will be properly integrated when PredictionError initializes the system
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error and update system using neural substrate
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error magnitude (lower error = higher confidence)
        pImpl->confidence = 1.0f / (1.0f + std::abs(pImpl->predictionError));
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
    // Store observation for future prediction using neural substrate
    // Use neural activity patterns as internal representations
    // For now, just record the observation
    // TODO PHASE 2: Implement real predictive model training using brain neural activity
    
    // Store in error history as a placeholder for learned prediction
    const auto& data = observation.getData();
    if (!data.empty()) {
        pImpl->predictionError = data[0];  // Use first value as prediction error for learning
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on observation novelty
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    }
}

} // namespace nlm
