#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

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
        
        // Update confidence based on prediction error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError * 10.0f);
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using observation
    // For now, just store it as the baseline for comparison
    // TODO PHASE 2: Implement proper learning algorithm
    
    NLM_LOG_INFO("PredictionSystem training with observation");
    
    // Store observation for baseline
    const auto& data = observation.getData();
    if (!data.empty()) {
        pImpl->predictionError = 0.0f;  // Reset error after training
        pImpl->confidence = 0.8f;  // Increase confidence after training
    }
}

// Additional helper methods for PredictionSystem
void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
}

float PredictionSystem::getAverageError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    float sum = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += error;
    }
    return sum / pImpl->errorHistory.size();
}

bool PredictionSystem::isStable() const {
    return pImpl->errorHistory.size() >= 5 && 
           getAverageError() < 0.1f &&  // Low error
           pImpl->confidence > 0.7f;    // High confidence
}

} // namespace nlm
