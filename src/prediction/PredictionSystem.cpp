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

// Phase 2 enhanced functionality
    
    // Get current prediction error
    float PredictionSystem::getPredictionError() const {
        return pImpl->predictionError;
    }
    
    // Get prediction confidence (0-1, 1 = confident, 0 = uncertain)
    float PredictionSystem::getConfidence() const {
        return pImpl->confidence;
    }
    
    // Get prediction error history
    const std::vector<float>& PredictionSystem::getErrorHistory() const {
        return pImpl->errorHistory;
    }
    
    // Clear prediction history
    void PredictionSystem::clearHistory() {
        pImpl->errorHistory.clear();
    }
    
    // Enhanced prediction training method
    void PredictionSystem::train(const SensoryInput& observation) {
        // TODO PHASE 2: Train prediction model
        // PLACEHOLDER: Just update confidence
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    }
    
    // Get average prediction error over time
    float PredictionSystem::getAverageError() const {
        if (pImpl->errorHistory.empty()) return 0.0f;
        float sum = 0.0f;
        for (float error : pImpl->errorHistory) {
            sum += error;
        }
        return sum / pImpl->errorHistory.size();
    }
    
    // Get maximum prediction error
    float PredictionSystem::getMaxError() const {
        if (pImpl->errorHistory.empty()) return 0.0f;
        float maxError = 0.0f;
        for (float error : pImpl->errorHistory) {
            if (error > maxError) maxError = error;
        }
        return maxError;
    }
    
    // Check if prediction system is properly trained
    bool PredictionSystem::isTrained() const {
        return pImpl->confidence > 0.8f;
    }

} // namespace nlm
