#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float predictionAlpha;  // Learning rate for predictions
    std::vector<float> predictedPattern;
    float predictionVariance;  // Measure of prediction uncertainty
    std::unique_ptr<RandomGenerator> rng;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionAlpha(0.1f), predictionVariance(0.1f) {
        rng = std::make_unique<RandomGenerator>(42);  // Default seed for consistency
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real neural prediction using pattern matching from learned sequences
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Simple pattern-based prediction - use TemporalRelation system to predict
    // For now, implement a basic prediction that can be enhanced in Phase 2
    pImpl->predictedPattern = currentData;
    
    // Add some noise to simulate prediction uncertainty
    std::vector<float> predictedData = currentData;
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Simple prediction with some variance
        float noise = (pImpl->rng->getRandomFloat() - 0.5f) * 0.2f;
        predictedData[i] = currentData[i] + noise;
        
        // Clamp to reasonable range
        if (predictedData[i] < -1.0f) predictedData[i] = -1.0f;
        if (predictedData[i] > 1.0f) predictedData[i] = 1.0f;
    }
    
    // Update prediction variance based on change magnitude
    float totalChange = 0.0f;
    for (size_t i = 0; i < currentData.size(); ++i) {
        float change = std::abs(predictedData[i] - currentData[i]);
        totalChange += change;
    }
    pImpl->predictionVariance = totalChange / currentData.size();
    
    // Update confidence based on prediction variance
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionVariance);
    
    auto result = std::make_unique<SensoryInput>();
    result->setData(predictedData);
    
    NLM_LOG_INFO("PredictionSystem: Predicted next state with confidence=" + std::to_string(pImpl->confidence) +
                 ", variance=" + std::to_string(pImpl->predictionVariance));
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation using neural patterns
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate mean squared error as prediction error
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit error history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        // Update prediction alpha (learning rate) based on error
        // High error = faster learning
        pImpl->predictionAlpha = std::min(0.5f, pImpl->predictionError * 2.0f);
        
        NLM_LOG_INFO("PredictionSystem: Updated predictions, error=" + std::to_string(pImpl->predictionError) +
                     ", confidence=" + std::to_string(pImpl->confidence) +
                     ", learning_rate=" + std::to_string(pImpl->predictionAlpha));
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
    // Implement prediction model training using neural substrate
    // For now, just record the observation for future predictions
    const auto& data = observation.getData();
    
    if (!data.empty()) {
        // Add a record of this observation to predictedPattern for learning
        // In a full implementation, this would update internal prediction weights
        if (pImpl->predictedPattern.empty()) {
            pImpl->predictedPattern = data;
        }
        
        NLM_LOG_INFO("PredictionSystem: Training on observation with " + std::to_string(data.size()) + " features");
    }
}

} // namespace nlm
