#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> lastPrediction;
    std::vector<float> lastSensoryState;
    
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
        // Update confidence based on error (lower error = higher confidence)
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
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

// New methods for improved integration
void PredictionSystem::updateWithState(const std::vector<float>& sensoryState, float reward) {
    // Update prediction based on sensory state and reward
    pImpl->lastSensoryState = sensoryState;
    
    // Simple prediction: predict next state based on current state with some noise
    pImpl->lastPrediction.resize(sensoryState.size());
    float noise = 0.1f;  // Add some prediction noise
    for (size_t i = 0; i < sensoryState.size(); ++i) {
        // Simple random walk prediction
        float delta = ((rand() % 200 - 100) / 100.0f) * noise;
        pImpl->lastPrediction[i] = sensoryState[i] + delta;
    }
    
    // Calculate prediction error
    float sumError = 0.0f;
    for (size_t i = 0; i < sensoryState.size(); ++i) {
        float diff = pImpl->lastPrediction[i] - sensoryState[i];
        sumError += diff * diff;
    }
    pImpl->predictionError = sumError / sensoryState.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
}

void PredictionSystem::updateWithWorkingMemory(const std::vector<float>& workingMemoryPattern) {
    // Update prediction based on working memory content
    pImpl->lastSensoryState = workingMemoryPattern;
    
    // Predict based on working memory pattern
    pImpl->lastPrediction.resize(workingMemoryPattern.size());
    for (size_t i = 0; i < workingMemoryPattern.size(); ++i) {
        // Enhance prediction based on working memory strength
        float memoryStrength = workingMemoryPattern[i];
        float prediction = workingMemoryPattern[i] * (1.0f + memoryStrength * 0.5f);
        // Add some noise
        prediction += ((rand() % 200 - 100) / 100.0f) * 0.1f;
        pImpl->lastPrediction[i] = prediction;
    }
    
    // Calculate prediction error (assuming current state is close to working memory)
    pImpl->predictionError = 0.0f;  // Low error when working memory is present
    for (size_t i = 0; i < workingMemoryPattern.size(); ++i) {
        float diff = pImpl->lastPrediction[i] - workingMemoryPattern[i];
        pImpl->predictionError += diff * diff;
    }
    pImpl->predictionError /= workingMemoryPattern.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError * 10.0f);  // Higher confidence for working memory
}

} // namespace nlm
