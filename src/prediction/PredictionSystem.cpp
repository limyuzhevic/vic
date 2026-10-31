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
    float predictionAlpha;  // Learning rate for predictions
    std::vector<float> predictionModel;  // Neural prediction model weights
    std::vector<float> lastPrediction;   // Last successful prediction
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionAlpha(0.05f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using neural substrate
    // The prediction system learns to predict future states based on patterns
    
    const auto& currentData = currentState.getData();
    size_t inputSize = currentData.size();
    
    // If we haven't trained yet, initialize prediction model
    if (pImpl->predictionModel.empty()) {
        pImpl->predictionModel.resize(inputSize, 0.0f);
        // Initialize with small random weights
        for (float& w : pImpl->predictionModel) {
            w = (static_cast<float>(rand()) / RAND_MAX) * 0.2f - 0.1f;
        }
    }
    
    // Create predicted state based on neural prediction
    std::vector<float> predictedData(inputSize, 0.0f);
    
    // Apply prediction model: use neural activity patterns to predict next state
    // This is a simple forward model - real implementation would use neural dynamics
    for (size_t i = 0; i < inputSize; ++i) {
        // Predict based on current state and learned patterns
        float prediction = currentData[i] * 0.7f;
        
        // Add influence from previous predictions (temporal continuity)
        if (!pImpl->lastPrediction.empty() && i < pImpl->lastPrediction.size()) {
            prediction += pImpl->lastPrediction[i] * 0.3f;
        }
        
        // Add noise for exploration
        prediction += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        
        predictedData[i] = std::clamp(prediction, 0.0f, 1.0f);
    }
    
    // Store prediction for next time step
    pImpl->lastPrediction = predictedData;
    
    // Create and return sensory input with predicted data
    auto result = std::make_unique<SensoryInput>(predictedData.size());
    result->setData(predictedData);
    
    NLM_LOG_INFO("PredictionSystem: Predicted next state with error " + 
                 std::to_string(getPredictionError()));
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                       const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float squared = diff * diff;
            sumError += squared;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        // Compute prediction error
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        
        // Update confidence based on error consistency
        float avgError = maxError;  // Use max error as confidence indicator
        pImpl->confidence = std::max(0.1f, 1.0f - avgError * 5.0f);
        
        // Update prediction model with learning
        train(predData);
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
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
    // Train prediction model using observed sensory input
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Simple Hebbian-like learning for prediction model
    for (size_t i = 0; i < std::min(data.size(), pImpl->predictionModel.size()); ++i) {
        // Update prediction weights based on observation
        // This is a simplified model - real implementation would use neural dynamics
        pImpl->predictionModel[i] *= (1.0f - pImpl->predictionAlpha);
        pImpl->predictionModel[i] += data[i] * pImpl->predictionAlpha;
        
        // Clamp weights to reasonable range
        pImpl->predictionModel[i] = std::clamp(pImpl->predictionModel[i], -1.0f, 1.0f);
    }
}

} // namespace nlm
