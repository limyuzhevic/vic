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
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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

// Add updatePrediction method to PredictionSystem
void PredictionSystem::updatePrediction(const std::vector<float>& input) {
    // Simple prediction model: next state is current state with some noise
    // TODO: Replace with more sophisticated predictive model using neural substrate
    
    if (input.empty()) return;
    
    // For now: prediction = current input with some noise
    // This will be replaced by actual neural prediction in Phase 2
    pImpl->predictionError = 0.1f; // Placeholder error value
    
    // Update confidence based on input variability
    float inputVariance = 0.0f;
    float sum = 0.0f;
    for (float val : input) {
        sum += val;
    }
    float mean = sum / input.size();
    for (float val : input) {
        inputVariance += (val - mean) * (val - mean);
    }
    inputVariance /= input.size();
    
    // Higher confidence for consistent input
    pImpl->confidence = std::clamp(1.0f - inputVariance, 0.1f, 1.0f);
    
    // Store error in history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

} // namespace nlm
