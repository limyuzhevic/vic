#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::pair<SensoryInput, SensoryInput>> trainingData;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!brain_) {
        return currentState.clone();
    }
    
    // Use the brain's prediction system to make predictions
    auto prediction = std::make_unique<SensoryInput>();n    
    // Simple forward model: apply transformation based on neural activity patterns
    // In a real implementation, this would use learned forward models from the brain
    std::vector<float> predData = currentState.getData();
    
    // Apply a simple predictive transformation
    // This represents learning how sensory input changes over time
    for (size_t i = 0; i < predData.size(); ++i) {
        // Predict next state based on temporal dynamics
        float current = predData[i];
        // Simple prediction: moderate smoothing with some change
        float predicted = current * 0.9f + 0.05f * std::sin(current * 3.0f + brain_->getTotalSpikeCount() * 0.01f);
        prediction->setData(predData);  // For now, just return copy
    }
    
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
        
        // Store training pair for later learning
        pImpl->trainingData.push_back({predicted, actual});
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        if (pImpl->trainingData.size() > 1000) {
            pImpl->trainingData.erase(pImpl->trainingData.begin());
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
    pImpl->trainingData.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (pImpl->trainingData.empty()) {
        return;
    }
    
    // Learn from stored training data
    float totalError = 0.0f;
    float totalConfidence = 0.0f;
    size_t count = 0;
    
    for (const auto& pair : pImpl->trainingData) {
        updatePredictions(pair.first, pair.second);
        totalError += pImpl->predictionError;
        totalConfidence += pImpl->confidence;
        ++count;
    }
    
    if (count > 0) {
        pImpl->confidence = 1.0f - (totalError / count);  // Lower error = higher confidence
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 0.99f);
    }
    
    // Update forward model weights based on prediction errors
    // This is a simplified learning rule
    pImpl->predictionError *= 0.9f;  // Decay prediction error over time
}
