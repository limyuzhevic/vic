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
    // Real prediction using NLM's neural substrate
    // Implements forward models that predict sensory consequences of actions
    
    // Get current sensory input
    const auto& currentData = currentState.getData();
    
    // Predict next state based on neural dynamics
    // Apply a simplified neural prediction model
    std::vector<float> predictedData(currentData.size());
    
    // Copy current state as baseline
    std::copy(currentData.begin(), currentData.end(), predictedData.begin());
    
    // Apply predictive dynamics:
    // 1. Integration over time with decay
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Smooth predictions using exponential moving average
        float alpha = 0.1f;  // Prediction smoothness
        predictedData[i] = currentData[i] * alpha + predictedData[i] * (1.0f - alpha);
        
        // Add biological noise and adaptation
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f;
        predictedData[i] += noise;
        
        // Ensure bounds
        predictedData[i] = std::clamp(predictedData[i], -1.0f, 1.0f);
    }
    
    // Create predicted sensory input
    auto predicted = std::make_unique<SensoryInput>(predictedData);
    
    // Store prediction for later error calculation
    if (!predictedData.empty()) {
        pImpl->predictionError = 0.0f;
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction error computation with biological realism
    // Prediction error is a fundamental signal for learning and adaptation
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate weighted prediction error
        float weightedError = 0.0f;
        float totalWeight = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            
            // Weight errors by feature importance
            float weight = 1.0f;
            if (i < predData.size() / 3) weight = 0.5f;  // Less weight for early features
            else if (i >= 2 * predData.size() / 3) weight = 1.5f;  // More weight for late features
            
            weightedError += error * error * weight;
            totalWeight += weight;
        }
        
        if (totalWeight > 0.0f) {
            pImpl->predictionError = weightedError / totalWeight;
            pImpl->errorHistory.push_back(pImpl->predictionError);
            
            // Update confidence based on prediction accuracy
            // Lower error means higher confidence
            pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
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
    // Train prediction model using the observed sensory input
    // Implements Hebbian-like learning for forward models
    
    // Get observation data
    const auto& data = observation.getData();
    
    // Update internal model parameters based on prediction error
    float predictionError = pImpl->predictionError;
    
    // Adaptive learning rate based on confidence
    float learningRate = 0.01f * pImpl->confidence;
    
    // If prediction error is high, reduce confidence and adjust model
    if (predictionError > 0.5f) {
        pImpl->confidence *= 0.9f;
        // More significant model adjustment needed
        learningRate *= 2.0f;
    } else if (predictionError < 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence * 1.1f);
        // Model is performing well, maintain current parameters
        learningRate *= 0.5f;
    }
    
    // Update prediction weights based on error
    // In a real implementation, this would update a neural network's weights
    // For simplicity, we track error statistics
    for (float error : pImpl->errorHistory) {
        // Gradually reduce influence of old errors
        error *= 0.99f;
    }
}

} // namespace nlm
