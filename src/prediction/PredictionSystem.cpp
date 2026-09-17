#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float maxPredictionError;
    float confidence;
    float predictionBias;
    std::vector<float> errorHistory;
    size_t trainingCount;
    std::vector<float> lastTrainingSample;
    
    Impl() : predictionError(0.0f), maxPredictionError(0.0f), confidence(0.5f), 
             predictionBias(0.0f), trainingCount(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Integrate with NeuralPrediction for actual prediction
    // Use neural dynamics to predict future states based on current activity
    
    // Get current sensory pattern
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        NLM_LOG_WARNING("Cannot predict: empty current state");
        return currentState.clone();
    }
    
    // For now, implement basic prediction using current state
    // TODO: Integrate with NeuralPrediction when properly connected
    std::vector<float> predictedData = currentData;
    
    // Simple prediction: add noise based on current state variability
    float avg = 0.0f;
    for (float val : currentData) {
        avg += val;
    }
    avg /= currentData.size();
    
    // Create predicted state with some variation
    std::vector<float> predicted(currentData.size());
    for (size_t i = 0; i < currentData.size(); ++i) {
        float variation = (i % 5 == 0) ? 0.1f : 0.0f;  // Occasional spikes
        predicted[i] = currentData[i] + (variation - avg * 0.01f);
    }
    
    // Create new sensory input with predicted data
    return std::make_unique<SensoryInput>(predicted);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update prediction error based on comparison
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = sqrt(sumSquaredError / predData.size());
        pImpl->maxPredictionError = maxError;  // Track max error for learning
        
        // Update confidence based on prediction accuracy
        if (pImpl->predictionError < 0.1f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
        } else {
            pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.1f);
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit history size
        if (pImpl->errorHistory.size() > 1000) {
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Integrate prediction training with neural plasticity
    // Use observation to update internal predictive models
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Store observation for learning
    // In a real implementation, this would update neural prediction networks
    // For now, just track statistics
    
    pImpl->lastTrainingSample = data;
    pImpl->trainingCount++;
    
    // Simple learning: adjust prediction parameters based on observation
    float avg = 0.0f;
    for (float val : data) avg += val;
    avg /= data.size();
    
    pImpl->predictionBias = (pImpl->predictionBias * 0.9f) + (avg * 0.1f);
}

} // namespace nlm
