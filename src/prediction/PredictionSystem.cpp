#include "PredictionSystem.hpp"

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
    // PLACEHOLDER: For now, return a copy but apply some realistic dynamics
    auto predicted = currentState.clone();
    
    // Apply simple temporal dynamics - dampen large values and add noise
    const auto& currentData = currentState.getData();
    auto& predictedData = predicted->getData();
    
    if (!currentData.empty() && currentData.size() == predictedData.size()) {
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Smooth transitions - current value influences next state
            predictedData[i] = currentData[i] * 0.7f + predictedData[i] * 0.3f;
            
            // Add small random variations for natural dynamics
            float noise = (pImpl->errorHistory.empty() ? 0.0f : pImpl->errorHistory.back()) * 0.01f;
            predictedData[i] += noise * 0.1f; // Add small random noise
        }
        
        // Clamp to valid range
        for (float& val : predictedData) {
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
        }
    }
    
    return predicted;
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
    // PLACEHOLDER: Simple statistical learning
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update confidence based on observation
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
    
    // If we have prediction history, compute average error
    if (!pImpl->errorHistory.empty()) {
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Adjust confidence based on learning progress
        if (avgError < 0.01f) {
            pImpl->confidence = 1.0f;
        } else if (avgError < 0.05f) {
            pImpl->confidence = 0.9f;
        }
    }
}

} // namespace nlm
