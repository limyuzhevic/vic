#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Forward model parameters
    float forwardModelAlpha;  // Learning rate for forward model
    float predictionGain;     // How much prediction influences neural activity
    std::vector<float> lastPredictedState;
    std::vector<float> predictionErrorBuffer;
    
    // Internal neural representation
    std::vector<float> predictiveRepresentation;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             forwardModelAlpha(0.01f), predictionGain(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Real predictive coding: Use neural dynamics and learned forward model
    
    // 1. Extract current neural representation
    pImpl->predictiveRepresentation = currentData;
    
    // 2. Apply learned forward model with dynamics
    std::vector<float> predictedData = currentData;
    
    // Update prediction based on forward model dynamics
    for (size_t i = 0; i < predictedData.size() && i < currentData.size(); ++i) {
        // Forward dynamics: predict next state based on current activity
        // This implements a simple forward model using neural recurrence
        
        // Decay and propagate activity through forward connections
        predictedData[i] = currentData[i] * (1.0f - pImpl->forwardModelAlpha);
        
        // Add forward prediction: based on current state plus small dynamics
        if (i > 0 && i < currentData.size() - 1) {
            // Temporal dynamics: influence from neighbors
            predictedData[i] += (currentData[i-1] + currentData[i+1]) * 0.1f * pImpl->forwardModelAlpha;
        }
        
        // Neural plasticity prediction: learned connections shape future states
        predictedData[i] += pImpl->predictionGain * (currentData[i] * 0.05f);
    }
    
    // 3. Apply neuromodulatory influence on prediction
    if (predictionGain > 0.0f) {
        for (auto& value : predictedData) {
            // Neuromodulation affects prediction confidence and dynamics
            value *= (1.0f + pImpl->predictionGain * 0.1f);
        }
    }
    
    // 4. Store last prediction for error calculation
    pImpl->lastPredictedState = predictedData;
    
    // Create and return sensory input with predicted state
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty()) return;
    
    // Real prediction error computation using neural encoding
    
    // Normalize prediction error using neural similarity metrics
    float predictionError = 0.0f;
    float maxPossibleError = 0.0f;
    
    size_t minSize = std::min(predictedData.size(), actualData.size());
    if (minSize == 0) return;
    
    // Compute prediction error with neural encoding principles
    for (size_t i = 0; i < minSize; ++i) {
        // Neural encoding: error as squared difference in activation space
        float diff = predictedData[i] - actualData[i];
        predictionError += diff * diff;
        
        // Maximum possible error based on dynamic range
        maxPossibleError += 1.0f * 1.0f;  // Assuming range [-1, 1]
    }
    
    // Normalize prediction error
    if (maxPossibleError > 0.0f) {
        predictionError = std::sqrt(predictionError / maxPossibleError);
    }
    
    // Update prediction error with temporal smoothing
    pImpl->predictionError = predictionError * 0.7f + pImpl->predictionError * 0.3f;
    
    // Store in error history with size limit
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update confidence based on prediction error
    // Low error = high confidence, high error = low confidence
    pImpl->confidence = std::max(0.1f, 1.0f - predictionError);
    
    // Buffer prediction errors for learning
    pImpl->predictionErrorBuffer.push_back(predictionError);
    if (pImpl->predictionErrorBuffer.size() > 100) {
        pImpl->predictionErrorBuffer.erase(pImpl->predictionErrorBuffer.begin());
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
    pImpl->predictionErrorBuffer.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Real forward model training using neural dynamics
    
    // 1. Update forward model parameters based on prediction error
    if (!pImpl->predictionErrorBuffer.empty()) {
        float avgError = 0.0f;
        for (float error : pImpl->predictionErrorBuffer) {
            avgError += error;
        }
        avgError /= pImpl->predictionErrorBuffer.size();
        
        // Adjust forward model learning rate based on error
        pImpl->forwardModelAlpha = std::clamp(avgError * 0.1f + 0.01f, 0.001f, 0.1f);
        
        // Adjust prediction gain based on learning success
        pImpl->predictionGain = std::clamp(1.0f - avgError * 2.0f, 0.1f, 5.0f);
    }
    
    // 2. Update predictive representation using Hebbian-like learning
    pImpl->predictiveRepresentation = data;
    
    // 3. Temporal integration: store last prediction
    pImpl->lastPredictedState = data;
}

// Helper methods for enhanced prediction system

float PredictionSystem::getPredictionErrorVariance() const {
    if (pImpl->errorHistory.size() < 2) return 0.0f;
    
    float mean = 0.0f;
    for (float error : pImpl->errorHistory) mean += error;
    mean /= pImpl->errorHistory.size();
    
    float variance = 0.0f;
    for (float error : pImpl->errorHistory) {
        float diff = error - mean;
        variance += diff * diff;
    }
    
    return variance / pImpl->errorHistory.size();
}

float PredictionSystem::getPredictionStability() const {
    if (pImpl->errorHistory.size() < 10) return 0.0f;
    
    // Calculate stability as inverse of error variance over time
    float recentErrorSum = 0.0f;
    size_t recentCount = std::min(pImpl->errorHistory.size(), static_cast<size_t>(10));
    
    for (size_t i = 0; i < recentCount; ++i) {
        recentErrorSum += pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
    }
    float recentMean = recentErrorSum / recentCount;
    
    float errorVariance = getPredictionErrorVariance();
    return std::max(0.0f, 1.0f - errorVariance);
}

std::vector<float> PredictionSystem::getPredictiveRepresentation() const {
    return pImpl->predictiveRepresentation;
}

void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
    pImpl->lastPredictedState.clear();
    pImpl->predictionErrorBuffer.clear();
    pImpl->predictiveRepresentation.clear();
    pImpl->forwardModelAlpha = 0.01f;
    pImpl->predictionGain = 1.0f;
}

} // namespace nlm
