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

void PredictionSystem::updatePredictionError(float error) {
    // Update prediction error with new measurement
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Adjust confidence based on error magnitude
    // Lower error = higher confidence
    if (error < 0.01f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
    } else if (error < 0.1f) {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f);
    } else {
        pImpl->confidence = std::max(0.01f, pImpl->confidence - 0.1f);
    }
    
    NLM_LOG_INFO("PredictionSystem: Error " + std::to_string(error) + ", Confidence " + std::to_string(pImpl->confidence));
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
