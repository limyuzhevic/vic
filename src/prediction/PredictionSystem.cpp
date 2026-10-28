#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;  // Reference to the brain for integration
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

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

void PredictionSystem::update(float timestep) {
    // Update prediction system state
    // Placeholder implementation - could update confidence based on error history
    if (!pImpl->errorHistory.empty()) {
        // Simple confidence calculation based on prediction error variance
        float sumErrors = 0.0f;
        float sumSquares = 0.0f;
        for (float error : pImpl->errorHistory) {
            sumErrors += error;
            sumSquares += error * error;
        }
        
        float meanError = sumErrors / pImpl->errorHistory.size();
        float variance = sumSquares / pImpl->errorHistory.size() - meanError * meanError;
        
        // Confidence inversely related to prediction error variance
        pImpl->confidence = 1.0f / (1.0f + variance);
        // Clamp confidence between 0.1 and 1.0
        pImpl->confidence = std::max(0.1f, std::min(1.0f, pImpl->confidence));
    }
}
