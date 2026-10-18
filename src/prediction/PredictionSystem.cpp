#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionModels;  // Simple associative memory for predictions
    size_t modelCount;
    
    Impl() : predictionError(0.0f), confidence(0.5f), modelCount(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Improved prediction: Use recent sensory patterns to predict next state
    // This is a simple autoregressive model based on neural state patterns
    
    const auto& currentData = currentState.getData();
    auto predictedInput = std::make_unique<SensoryInput>();
    
    if (currentData.empty()) {
        return predictedInput;
    }
    
    // Simple prediction: extrapolate from recent patterns with some noise
    // In a real implementation, this would use the brain's recurrent dynamics
    predictedInput->setData(currentData);
    
    // Add some temporal continuity to the prediction
    for (size_t i = 0; i < currentData.size(); ++i) {
        float predictedValue = currentData[i];
        
        // Add temporal derivative (simple velocity prediction)
        if (i > 0 && i < currentData.size() - 1) {
            float velocity = currentData[i + 1] - currentData[i - 1];
            predictedValue += velocity * 0.1f;
        }
        
        // Add small random noise for exploration
        predictedValue += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        
        predictedInput->setValue(i, predictedValue);
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Improved prediction error computation
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
        
        // Keep history to reasonable size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    // Improved confidence calculation based on prediction error and history
    if (pImpl->errorHistory.empty()) {
        return 0.5f;
    }
    
    float avgError = 0.0f;
    for (float error : pImpl->errorHistory) {
        avgError += error;
    }
    avgError /= pImpl->errorHistory.size();
    
    // Lower error = higher confidence
    pImpl->confidence = 1.0f / (1.0f + avgError);
    
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Improved training: Store observation pattern for future predictions
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) return;
    
    // Store the pattern as a prediction model
    pImpl->predictionModels.push_back(obsData);
    pImpl->modelCount++;
    
    // Keep limited number of models
    if (pImpl->predictionModels.size() > 100) {
        pImpl->predictionModels.erase(pImpl->predictionModels.begin());
        pImpl->modelCount--;
    }
}

} // namespace nlm
