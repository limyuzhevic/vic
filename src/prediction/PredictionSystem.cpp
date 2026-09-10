#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> expectedPattern;  // Expected sensory pattern for next timestep
    
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

void PredictionSystem::updateCurrentPattern(const std::vector<float>& pattern) {
    // Update prediction model with current sensory pattern
    // This is a placeholder implementation - in a real system this would update
    // an internal prediction model based on the current sensory input
    
    // Store pattern as the "expected" pattern for next step
    pImpl->expectedPattern = pattern;
    
    // Clear previous prediction error history
    pImpl->errorHistory.clear();
    
    // Initialize confidence based on pattern variability
    if (!pattern.empty()) {
        float sum = 0.0f, sumSq = 0.0f;
        for (float val : pattern) {
            sum += val;
            sumSq += val * val;
        }
        float mean = sum / pattern.size();
        pImpl->confidence = 1.0f / (1.0f + std::sqrt(sumSq / pattern.size() - mean * mean));
    }
}

} // namespace nlm
