#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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

// Update prediction system with current state and timestep
void PredictionSystem::update(TimestepDuration dt, SimulationStep step) {
    // Update internal prediction state based on timestep
    // In Phase 2, this would update neural prediction networks
    // For now, just store the timestep for history
    if (dt > 0.0f) {
        pImpl->errorHistory.push_back(pImpl->predictionError * dt * step);
    }
}

// Get current prediction strength (0.0 = weak, 1.0 = strong)
float PredictionSystem::getPredictionStrength() const {
    // Calculate prediction strength based on error history
    if (pImpl->errorHistory.empty()) {
        return 0.0f;
    }
    
    float totalError = 0.0f;
    for (float error : pImpl->errorHistory) {
        totalError += error;
    }
    
    float avgError = totalError / pImpl->errorHistory.size();
    
    // Strong prediction = low error = high strength
    float strength = 1.0f - avgError;
    return std::max(0.0f, std::min(strength, 1.0f));
}

// Integrate concepts from concept formation system
void PredictionSystem::integrateConcepts(const std::vector<std::string>& concepts) {
    // Integrate learned concepts into prediction system
    // This would connect concepts to pattern recognition
    // For now, just acknowledge and update confidence
    pImpl->confidence = pImpl->confidence + 0.1f * concepts.size();
    pImpl->confidence = std::min(pImpl->confidence, 1.0f);
}

} // namespace nlm
