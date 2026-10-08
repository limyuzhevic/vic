#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"

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
    // CURRENT: Use working memory patterns for prediction when available
    
    if (!brain_) return currentState.clone();
    
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return currentState.clone();
    
    // Get working memory patterns
    std::vector<float> memoryPatterns = workingMemory->retrieve();
    
    if (memoryPatterns.empty()) {
        // No working memory content - use current state
        return currentState.clone();
    }
    
    // Create a new InternalSignals with working memory patterns
    auto predictedState = std::make_unique<InternalSignals>();
    
    // Add working memory patterns to the prediction
    for (float pattern : memoryPatterns) {
        predictedState->addSignal(pattern);
    }
    
    // Also add some scaled version of current state
    const auto& currentData = currentState.getData();
    for (float value : currentData) {
        predictedState->addSignal(value * 0.5f);  // Scale down current state
    }
    
    return predictedState;
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
        
        // Update confidence based on prediction accuracy
        // Lower error -> higher confidence
        float errorNormalized = std::min(pImpl->predictionError, 1.0f);
        pImpl->confidence = 0.9f - (errorNormalized * 0.4f);  // Range: 0.5 to 0.9
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

void PredictionSystem::updateConfidence(float newConfidence) {
    pImpl->confidence = std::max(0.1f, std::min(1.0f, newConfidence));
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, just accumulate error history
    pImpl->errorHistory.push_back(0.0f);
    pImpl->predictionError = 0.0f;
    
    // Update confidence based on training
    pImpl->confidence = 0.7f;
}

} // namespace nlm
