#include "PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../agent/AgentBrain.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionBuffer;
    
    // Internal prediction model state
    std::vector<float> weights;
    std::vector<float> biases;
    std::default_random_engine rng;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        rng.seed(42);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::update(const std::vector<float>& neuralPattern, SimulationStep step, Timestamp time) {
    // Store current pattern for prediction
    pImpl->predictionBuffer = neuralPattern;
    
    // Simple prediction model: use neural activity to predict next state
    // This is a placeholder for real predictive coding
    
    // Generate prediction based on pattern
    std::vector<float> predictedNext;
    for (float value : neuralPattern) {
        // Add noise and time-based variation to prediction
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
        float noise = dist(pImpl->rng);
        float temporalMod = 0.01f * static_cast<float>(step) / 1000.0f;  // Slowly changing
        
        float predicted = value + noise + temporalMod;
        predicted = std::max(0.0f, std::min(1.0f, predicted));  // Clamp to [0, 1]
        predictedNext.push_back(predicted);
    }
    
    // Set prediction buffer
    pImpl->predictionBuffer = predictedNext;
    
    // Update confidence based on pattern consistency
    float patternConsistency = 0.0f;
    if (!neuralPattern.empty()) {
        float mean = 0.0f;
        for (float v : neuralPattern) mean += v;
        mean /= neuralPattern.size();
        
        float variance = 0.0f;
        for (float v : neuralPattern) {
            float diff = v - mean;
            variance += diff * diff;
        }
        variance /= neuralPattern.size();
        
        patternConsistency = std::exp(-variance * 10.0f);  // Lower variance = higher confidence
        pImpl->confidence = (pImpl->confidence * 0.9f) + (patternConsistency * 0.1f);
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, return a copy of current state
    return currentState.clone();
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const std::vector<float>& currentPattern) {
    // Create a new sensory input based on prediction
    auto predictedSensory = std::make_unique<SensoryInput>();
    // In a real implementation, this would use the prediction system
    // to generate predicted sensory input based on current neural patterns
    return predictedSensory;
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

void PredictionSystem::train(const std::vector<float>& observation) {
    // Simple Hebbian-like learning for prediction model
    // This is a placeholder for real predictive coding training
    
    if (observation.empty()) return;
    
    // Initialize weights if first training
    if (pImpl->weights.empty()) {
        pImpl->weights = observation;
        pImpl->biases = std::vector<float>(observation.size(), 0.1f);
    }
    
    // Simple gradient update (placeholder)
    float learningRate = 0.01f;
    for (size_t i = 0; i < observation.size() && i < pImpl->weights.size(); ++i) {
        float prediction = observation[i] * pImpl->weights[i] + pImpl->biases[i];
        float error = observation[i] - prediction;
        pImpl->weights[i] += learningRate * error * observation[i];
        pImpl->biases[i] += learningRate * error;
    }
}

std::vector<float> PredictionSystem::getPredictedNextState() const {
    return pImpl->predictionBuffer;
}

MotorCommand PredictionSystem::getPredictedAction() const {
    // Based on prediction confidence and error, select action
    if (pImpl->confidence > 0.8f) {
        if (pImpl->predictionError < 0.01f) {
            return MotorCommand::Wait;
        } else {
            return MotorCommand::MoveForward;
        }
    } else if (pImpl->confidence > 0.5f) {
        return MotorCommand::MoveForward;
    } else {
        // Low confidence = exploratory action
        return MotorCommand::Interact;
    }
}

} // namespace nlm
