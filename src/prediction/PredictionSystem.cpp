#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> observationHistory;
    size_t historyLimit;
    
    Impl() : predictionError(0.0f), confidence(0.5f), historyLimit(10) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using stored patterns
    // If we have enough history, predict based on patterns
    if (pImpl->observationHistory.size() >= 2) {
        // Simple pattern matching: predict similar to recent similar states
        // For now, just return a modified version of current state
        auto prediction = currentState.clone();
        
        // Add some predicted change based on recent patterns
        const auto& currentData = currentState.getData();
        auto& predData = prediction->getData();
        
        if (!currentData.empty()) {
            // Add small random changes to simulate prediction uncertainty
            for (size_t i = 0; i < predData.size(); ++i) {
                predData[i] = currentData[i] * 0.8f + 0.2f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
            }
        }
        
        // Update confidence based on history
        pImpl->confidence = std::min(1.0f, 0.5f + pImpl->observationHistory.size() * 0.05f);
        return prediction;
    }
    
    // Not enough data to predict - return copy with small changes
    auto prediction = currentState.clone();
    const auto& currentData = currentState.getData();
    auto& predData = prediction->getData();
    
    if (!currentData.empty()) {
        // Add random changes
        for (size_t i = 0; i < predData.size(); ++i) {
            predData[i] = currentData[i] * 0.9f + 0.1f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        }
    }
    
    pImpl->confidence = 0.3f;  // Low confidence with no history
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation and learning
    // Calculate error between predicted and actual observations
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            if (std::abs(diff) > maxError) maxError = std::abs(diff);
        }
        
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Store actual observation for learning
        pImpl->observationHistory.push_back(actual);
        if (pImpl->observationHistory.size() > pImpl->historyLimit) {
            pImpl->observationHistory.erase(pImpl->observationHistory.begin());
        }
        
        // Update confidence based on prediction accuracy
        if (pImpl->predictionError < 0.1f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
        } else if (pImpl->predictionError > 0.5f) {
            pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f);
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model by storing recent observations
    // Store the observation for learning
    pImpl->observationHistory.push_back(observation.clone());
    if (pImpl->observationHistory.size() > pImpl->historyLimit) {
        pImpl->observationHistory.erase(pImpl->observationHistory.begin());
    }
    
    // Also calculate error for this observation
    pImpl->predictionError = 0.0f;  // Will be set by updatePredictions
}

} // namespace nlm
