#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<SensoryInput> lastPrediction;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::update(Brain* brain, TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update prediction confidence based on error history
    if (!pImpl->errorHistory.empty()) {
        float recentErrors = 0.0f;
        size_t recentCount = std::min(size_t(10), pImpl->errorHistory.size());
        for (size_t i = 0; i < recentCount; ++i) {
            recentErrors += pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
        }
        recentErrors /= recentCount;
        
        // Confidence inversely related to recent errors
        pImpl->confidence = std::max(0.1f, 1.0f - recentErrors);
    }
    
    // Simple predictive coding: predict based on current sensory activity
    // This is a simplified implementation that uses neural activity patterns
    if (pImpl->brain) {
        auto currentState = std::make_unique<SensoryInput>();
        currentState->setData(std::vector<float>(10, 0.0f)); // Placeholder: get from actual sensory neurons
        pImpl->lastPrediction = std::move(currentState);
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, create a simple prediction based on current state
    auto prediction = std::make_unique<SensoryInput>();
    
    const auto& currentData = currentState.getData();
    std::vector<float> predictedData(currentData.size(), 0.0f);
    
    // Simple forward model: small changes with noise
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Add some temporal dynamics
        float change = (i < currentData.size() - 1) ? (currentData[i+1] - currentData[i]) * 0.1f : 0.0f;
        predictedData[i] = currentData[i] + change + (pImpl->rng ? pImpl->rng->nextFloat() * 0.01f - 0.005f : 0.0f);
    }
    
    prediction->setData(predictedData);
    pImpl->lastPrediction = std::make_unique<SensoryInput>(*prediction);
    
    return prediction;
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

float PredictionSystem::getPredictionConfidence() const {
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
    // PLACEHOLDER: Simple training by updating error history
    pImpl->lastPrediction = std::make_unique<SensoryInput>(observation);
    NLM_LOG_INFO("Prediction system trained on observation");
}

void PredictionSystem::processSensoryInput(const SensoryInput& input) {
    // Process new sensory input for prediction
    if (pImpl->brain) {
        // Use prediction error to modulate neural activity
        if (pImpl->lastPrediction) {
            updatePredictions(*pImpl->lastPrediction, input);
        }
        
        // Predict next state based on input
        pImpl->lastPrediction = predictNextState(input);
    }
}

} // namespace nlm
