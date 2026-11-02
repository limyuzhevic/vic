#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionWeights;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->predictionWeights.resize(100, 0.01f);  // Initialize prediction weights
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem not initialized");
        return currentState.clone();
    }
    
    // For now, use neural activity patterns to generate predictions
    // In Phase 6, this will use the full neural substrate
    const auto& currentData = currentState.getData();
    auto prediction = std::make_unique<SensoryInput>();
    prediction->setData(currentData);  // Start with same data
    
    // Add prediction noise based on neural variability
    auto& predData = prediction->getData();
    for (float& value : predData) {
        value += (pImpl->brain->getRandomGenerator()->nextFloat() - 0.5f) * 0.1f;
    }
    
    // Apply prediction confidence modulation
    if (pImpl->confidence < 0.5f) {
        // Low confidence = high variance
        for (float& value : predData) {
            value *= 2.0f;
        }
    }
    
    NLM_LOG_DEBUG("Prediction generated with error: " + std::to_string(pImpl->predictionError));
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
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
        
        // Limit history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction error
        pImpl->confidence = std::max(0.01f, 1.0f - pImpl->predictionError);
        NLM_LOG_DEBUG("Prediction error updated: " + std::to_string(pImpl->predictionError) + ", confidence: " + std::to_string(pImpl->confidence));
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
    // Simple Hebbian-style prediction training
    const auto& data = observation.getData();
    for (size_t i = 0; i < std::min(data.size(), pImpl->predictionWeights.size()); ++i) {
        pImpl->predictionWeights[i] += data[i] * 0.001f;  // Small weight updates
    }
    
    // Decay weights over time
    for (float& weight : pImpl->predictionWeights) {
        weight *= 0.999f;
    }
}

void PredictionSystem::update(TimestepDuration dt) {
    // Simple prediction decay over time
    pImpl->confidence *= std::pow(0.999f, dt);
}

} // namespace nlm
