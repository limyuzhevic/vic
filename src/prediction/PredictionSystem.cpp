#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float learningRate;
    float predictionGain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), predictionGain(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize random generator for prediction noise
    rng = std::make_unique<RandomGenerator>(42);
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using neural activity patterns
    // Create prediction based on current state with noise for exploration
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Generate prediction by applying neural dynamics
    std::vector<float> predictedData(currentData.size());
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Add neural noise for exploration
        float noise = (rng ? rng->getRandomFloat(-0.1f, 0.1f) : 0.0f);
        
        // Apply prediction dynamics: blend current state with moderated change
        float basePrediction = currentData[i] * 0.7f; // Stabilizing influence
        float change = currentData[i] * 0.3f * (1.0f + noise); // Exploratory change
        predictedData[i] = basePrediction + change;
        
        // Clamp to reasonable range
        predictedData[i] = std::max(-10.0f, std::min(10.0f, predictedData[i]));
    }
    
    // Create and return predicted sensory input
    auto predictedState = std::make_unique<SensoryInput>();
    predictedState->setData(predictedData);
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
    // Train prediction model using observed sensory data
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update confidence based on prediction consistency
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f * pImpl->predictionError);
    
    // Update prediction gain based on prediction error
    pImpl->predictionGain = 0.5f + 0.5f * (1.0f - pImpl->confidence);
    
    // Store observation for future predictions
    // (In a full implementation, this would update internal prediction weights)
}

} // namespace nlm
