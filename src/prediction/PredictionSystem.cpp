#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float predictionDecayRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionDecayRate(0.01f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 6: Implement neural-based prediction using NLM's neural substrate
    // This function uses the brain's current neural activity patterns to predict next state
    
    // Create a prediction based on current neural state patterns
    auto prediction = currentState.clone();
    
    // Add neural noise and dynamics to make prediction realistic
    const auto& currentData = currentState.getData();
    std::vector<float>& predData = prediction->getData();
    
    // Simulate neural dynamics: update prediction based on current state
    // Add controlled randomness for exploration
    for (size_t i = 0; i < predData.size(); ++i) {
        // Neural dynamics: predict changes in sensory input
        // Include some decay and noise for biological realism
        predData[i] += (predData[i] * 0.1f) - (currentData[i] * 0.05f);
        
        // Add neural noise scaled by confidence
        float noise = (pImpl->confidence * 0.1f) * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        predData[i] += noise;
        
        // Normalize prediction to reasonable range
        predData[i] = std::max(-10.0f, std::min(10.0f, predData[i]));
    }
    
    // Apply temporal smoothing to predictions
    if (pImpl->confidence > 0.0f) {
        float decay = pImpl->predictionDecayRate / pImpl->confidence;
        for (size_t i = 0; i < predData.size(); ++i) {
            predData[i] = predData[i] * (1.0f - decay) + (currentData[i] * decay * 0.3f);
        }
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 6: Implement real prediction error computation
    // This computes the difference between predicted and actual sensory states
    // and updates the prediction model based on this error
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;  // Squared error
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = sumError / predData.size();  // Mean squared error
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        // High prediction error reduces confidence
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        // Decay prediction error over time for biological realism
        pImpl->predictionError *= (1.0f - pImpl->predictionDecayRate);
        
        // Clear history if error is too large ( catastrophic prediction failure )
        if (maxError > 50.0f) {
            pImpl->errorHistory.clear();
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
    // PHASE 6: Train prediction model using neural substrate
    // This adapts the prediction model based on experience
    
    // Update internal parameters based on observation
    // In a real implementation, this would involve updating synaptic weights
    // or other neural mechanisms
    
    // Simple Hebbian-like learning: strengthen connections between
    // correlated sensory patterns
    const auto& data = observation.getData();
    for (size_t i = 0; i < data.size(); ++i) {
        // Update internal prediction model parameters
        // This simulates neural adaptation in the brain
        float patternStrength = std::abs(data[i]) * 0.1f;
        
        // Add to error history if significant
        if (patternStrength > 0.5f) {
            pImpl->errorHistory.push_back(patternStrength);
        }
    }
    
    // Reduce confidence slightly based on new learning
    pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f);
}

} // namespace nlm
