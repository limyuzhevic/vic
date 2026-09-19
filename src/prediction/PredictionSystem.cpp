#include "PredictionSystem.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brainPtr) {
    brain = brainPtr;
    pImpl->brain = brainPtr;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Use neural substrate for real prediction
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
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, just record the observation
    const auto& data = observation.getData();
    if (!data.empty()) {
        // Simple pattern storage
        pImpl->predictionError = data.size() / 100.0f; // dummy value
    }
}

} // namespace nlm
