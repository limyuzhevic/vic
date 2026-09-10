// PredictionSystem implementation
// Integrates with NeuralPrediction for real-time prediction capabilities

#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Integrated neural prediction engine
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Current sensory state buffer
    std::vector<float> currentSensoryState;
    
    // Prediction history for learning
    std::deque<std::vector<float>> predictionHistory;
    
    // Error tracking
    std::vector<float> errorHistory;
    float confidenceLevel;
    
    // Learning rate for prediction system
    float learningRate;
    
    // State tracking
    SimulationStep currentStep;
    
    Impl() : neuralPrediction(nullptr), confidenceLevel(0.5f), learningRate(0.01f), currentStep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl()) {
    // Create neural prediction engine
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    NLM_LOG_INFO("PredictionSystem initialized");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->neuralPrediction) {
        NLM_LOG_ERROR("NeuralPrediction not initialized");
        return nullptr;
    }
    
    // Extract sensory data from input
    const auto& data = currentState.getData();
    
    // Generate prediction using neural prediction
    std::vector<float> predictedPattern = pImpl->neuralPrediction->generatePrediction(pImpl->currentStep);
    
    if (predictedPattern.empty()) {
        NLM_LOG_INFO("No prediction generated");
        return nullptr;
    }
    
    // Create sensory input from predicted pattern
    auto result = std::make_unique<SensoryInput>(predictedPattern);
    
    // Store in history
    pImpl->currentSensoryState = predictedPattern;
    pImpl->predictionHistory.push_back(predictedPattern);
    if (pImpl->predictionHistory.size() > 100) {
        pImpl->predictionHistory.pop_front();
    }
    
    // Update confidence based on prediction consistency
    updateConfidence();
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty() || predictedData.size() != actualData.size()) {
        NLM_LOG_ERROR("Invalid prediction or actual data");
        return;
    }
    
    // Compute prediction error
    float error = computePredictionError(predictedData, actualData);
    
    // Store error for analysis
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update neural prediction with error
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->currentStep);
    }
    
    // Apply learning
    applyLearning(predictedData, actualData, error);
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    
    // Return mean squared error
    float sum = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += error * error;
    }
    return std::sqrt(sum / pImpl->errorHistory.size());
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidenceLevel;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->currentSensoryState.clear();
    pImpl->confidenceLevel = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    
    if (data.empty()) {
        NLM_LOG_ERROR("Cannot train on empty observation");
        return;
    }
    
    // Store observation for learning
    pImpl->currentSensoryState = data;
    
    // Update neural prediction
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(data, pImpl->currentStep);
    }
    
    // Increment step
    pImpl->currentStep++;
}

void PredictionSystem::updateConfidence() {
    // Simple confidence calculation based on prediction history
    if (pImpl->predictionHistory.size() < 2) {
        pImpl->confidenceLevel = 0.5f;
        return;
    }
    
    // Compare recent predictions with average
    size_t recentCount = std::min(size_t(10), pImpl->predictionHistory.size());
    float sumConsistency = 0.0f;
    
    for (size_t i = 1; i < recentCount; ++i) {
        const auto& prev = pImpl->predictionHistory[i - 1];
        const auto& curr = pImpl->predictionHistory[i];
        
        if (prev.size() == curr.size()) {
            float similarity = 0.0f;
            for (size_t j = 0; j < prev.size(); ++j) {
                similarity += std::abs(prev[j] - curr[j]);
            }
            similarity = 1.0f - (similarity / prev.size());
            sumConsistency += similarity;
        }
    }
    
    pImpl->confidenceLevel = (recentCount > 0) ? (sumConsistency / recentCount) : 0.5f;
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f; // Maximum error
    }
    
    float sum = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sum += diff * diff;
    }
    
    return std::sqrt(sum / predicted.size());
}

void PredictionSystem::applyLearning(const std::vector<float>& predicted, const std::vector<float>& actual, float error) {
    // Simple learning rule: modulate neural connections based on prediction error
    if (pImpl->neuralPrediction && error > 0.1f) { // Only learn from significant errors
        float learningSignal = error * pImpl->learningRate;
        
        // Modulate prediction neurons based on error
        pImpl->neuralPrediction->modulatePredictionSynapses(error, 1.0f - error);
    }
}

} // namespace nlm
