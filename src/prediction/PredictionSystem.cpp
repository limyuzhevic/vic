#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::update(TimestepDuration dt) {
    // Update prediction system state
    // Increase confidence for good predictions, decrease for bad ones
    pImpl->confidence = 0.5f - std::min(0.5f, pImpl->predictionError);
    if (pImpl->confidence < 0.1f) {
        pImpl->confidence = 0.1f;
    }
    
    // Learn from prediction error for next prediction
    // Higher error leads to more plasticity in prediction system
    if (pImpl->predictionError > 0.5f) {
        // Enable learning - weight updates would happen here
        pImpl->confidence *= 0.9f;
    } else {
        // Good prediction - consolidate learning
        pImpl->confidence *= 1.05f;
    }
    
    // Keep confidence bounded
    pImpl->confidence = std::min(1.0f, std::max(0.1f, pImpl->confidence));
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

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm
