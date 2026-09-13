#include "PredictionSystem.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float predictionDecayRate;
    std::vector<float> predictionBuffers;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionDecayRate(0.95f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement basic predictive coding: predict based on current state with added noise
    auto prediction = currentState.clone();
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return prediction;
    }
    
    // Simple prediction model: apply small temporal dynamics
    auto& predData = prediction->getData();
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Add small temporal variation to predict next state
        float variation = static_cast<float>(std::rand()) / RAND_MAX * 0.1f - 0.05f;
        predData[i] += variation;
        
        // Decay effect over time
        predData[i] *= pImpl->predictionDecayRate;
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error and update confidence
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
        
        // Update confidence based on error magnitude
        // Lower error = higher confidence
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
        
        // Limit history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    } else {
        pImpl->predictionError = 1.0f; // Max error if dimensions don't match
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
    pImpl->confidence = 0.5f;
    pImpl->predictionError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Simple training: accumulate statistics and update predictions
    const auto& data = observation.getData();
    
    if (!data.empty()) {
        // Store observation for future predictions
        if (pImpl->predictionBuffers.size() < 100) {
            pImpl->predictionBuffers.insert(pImpl->predictionBuffers.end(), data.begin(), data.end());
        } else {
            // Rolling buffer update
            std::copy(data.begin(), data.end(), pImpl->predictionBuffers.begin());
        }
        
        // Simple expectation learning: update confidence based on observation variance
        if (!data.empty()) {
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            
            // Update confidence based on predictability (lower variance = higher confidence)
            pImpl->confidence = std::max(0.1f, 1.0f - variance);
        }
    }
}

void PredictionSystem::update() {
    // Public update method - wrapper around prediction cycle
    // This method can be called from brain to trigger prediction system update
    // Uses current state to predict next state and updates based on prediction error
    
    // For now, this is a simple placeholder that can be expanded
    // In a real implementation, this would:
    // 1. Get current sensory state
    // 2. Predict next state using predictNextState()
    // 3. Compare with actual state if available
    // 4. Update predictions and confidence
    
    // Simple update: decay confidence over time to simulate prediction uncertainty
    pImpl->confidence *= pImpl->predictionDecayRate;
    pImpl->confidence = std::max(0.1f, pImpl->confidence);
}

} // namespace nlm
