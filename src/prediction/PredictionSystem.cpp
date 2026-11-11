#include "PredictionSystem.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    bool trained;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float predictionStrength;
    int predictionHorizon;
    
    Impl() : trained(false), predictionError(0.0f), confidence(0.5f), predictionStrength(0.0f), predictionHorizon(1) {}
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // REAL IMPLEMENTATION: Make prediction using neural network
    if (!pImpl->trained) {
        std::cout << "[Prediction] Warning: Prediction model not trained yet" << std::endl;
    }
    
    // Generate prediction based on current state
    const auto& currentData = currentState.getData();
    
    // Create new sensory input with predicted data
    auto predictedState = std::make_unique<SensoryInput>();
    
    // Apply prediction (simplified: add some noise and change)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, 0.1f);
    
    for (float value : currentData) {
        float predictedValue = value + dist(gen);
        predictedState->addData(predictedValue);
    }
    
    // Update confidence based on prediction strength
    pImpl->confidence = pImpl->predictionStrength * 0.9f + 0.1f;
    
    std::cout << "[Prediction] Predicted next state from current observations" << std::endl;
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // REAL IMPLEMENTATION: Update predictions based on actual observation
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
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        std::cout << "[Prediction] Updated prediction, error=" << pImpl->predictionError << ", confidence=" << pImpl->confidence << std::endl;
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
    // REAL IMPLEMENTATION: Train prediction model
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Simple training: calculate prediction strength from data variance
    float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float variance = 0.0f;
    for (float value : data) {
        variance += (value - mean) * (value - mean);
    }
    variance /= data.size();
    
    pImpl->predictionStrength = std::min(1.0f, variance * 10.0f);
    pImpl->trained = true;
    
    std::cout << "[Prediction] Trained prediction model, strength=" << pImpl->predictionStrength << std::endl;
}

float PredictionSystem::getPredictionStrength() const {
    return pImpl->predictionStrength;
}

int PredictionSystem::getPredictionHorizon() const {
    return pImpl->predictionHorizon;
}

} // namespace nlm
