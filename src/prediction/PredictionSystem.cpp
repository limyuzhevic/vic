#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionWeights;
    std::vector<float> predictionBuffer;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), predictionError(0.0f) {
        predictionWeights.resize(10, 0.1f); // Initialize with 10 weight components
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem not initialized");
        return currentState.clone();
    }
    
    // Get working memory and attention for prediction context
    std::vector<float> currentPattern;
    std::vector<float> features;
    
    // Extract working memory traces as prediction input
    if (pImpl->brain->getWorkingMemory()) {
        for (const auto& neuronId : pImpl->brain->getWorkingMemory()->getMemoryNeurons()) {
            float activation = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuronId);
            if (activation > 0.01f) {
                currentPattern.push_back(activation);
            }
        }
    }
    
    // Get attention winners to influence prediction
    if (pImpl->brain->getAttention()) {
        for (const auto& winner : pImpl->brain->getAttention()->getWinners()) {
            features.push_back(1.0f);
        }
    }
    
    // Combine current state with working memory for prediction
    std::vector<float> predictionInput = currentPattern;
    if (!features.empty()) {
        predictionInput.insert(predictionInput.end(), features.begin(), features.end());
    }
    
    // Generate prediction using weighted combination
    std::unique_ptr<SensoryInput> prediction = currentState.clone();
    if (!predictionInput.empty() && prediction) {
        // Update prediction weights based on current input
        updatePredictionWeights(predictionInput);
        
        // Apply learned weights to generate prediction
        applyPredictions(prediction, predictionInput);
    }
    
    // Update prediction confidence based on error history
    updateConfidence();
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error between predicted and actual sensory states
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
        
        // Keep only recent error history
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionError));
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using the observation
    const auto& data = observation.getData();
    if (!data.empty()) {
        // Update prediction buffer with new observation
        pImpl->predictionBuffer.push_back(data.back());
        if (pImpl->predictionBuffer.size() > 50) {
            pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
        }
        
        // Adapt prediction weights based on new observation
        adaptPredictions(data);
        
        NLM_LOG_INFO("Prediction model trained with " + std::to_string(data.size()) + " features");
    }
}

void PredictionSystem::updatePredictionFromSensory(const SensoryInput& sensoryInput) {
    // Update prediction system from actual sensory input
    std::vector<float> sensoryData = sensoryInput.getData();
    if (!sensoryData.empty() && pImpl->brain) {
        // Create internal signal for prediction error computation
        std::vector<float> predicted = generatePrediction(sensoryData);
        float error = computePredictionError(predicted, sensoryData);
        
        // Update prediction error signal for neuromodulation
        if (pImpl->brain->getPredictionErrorSignal()) {
            pImpl->brain->getPredictionErrorSignal()->computeError(error, error);
        }
    }
}

void PredictionSystem::updatePredictionFromAttention() {
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // Get attention weights to modulate prediction
        std::vector<float> attentionWeights;
        // In a real implementation, we would extract attention weights from the attentional system
        // For now, use the attention winners as features
        for (const auto& winner : pImpl->brain->getAttention()->getWinners()) {
            attentionWeights.push_back(1.0f);
        }
        
        if (!attentionWeights.empty()) {
            // Update prediction based on attention
            modulatePredictionByAttention(attentionWeights);
        }
    }
}

void PredictionSystem::updatePredictionFromWorkingMemory() {
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        std::vector<float> wmTrace;
        // Extract working memory traces
        for (const auto& neuronId : pImpl->brain->getWorkingMemory()->getMemoryNeurons()) {
            float activation = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuronId);
            if (activation > 0.01f) {
                wmTrace.push_back(activation);
            }
        }
        
        if (!wmTrace.empty()) {
            // Update prediction based on working memory
            enhancePredictionWithWorkingMemory(wmTrace);
        }
    }
}

void PredictionSystem::computePredictionError() {
    if (pImpl->brain) {
        // Get current sensory input from the brain
        // For now, use a placeholder error based on the prediction system state
        std::vector<float> currentPrediction(pImpl->predictionBuffer.begin(), pImpl->predictionBuffer.end());
        std::vector<float> dummyActual(currentPrediction.size(), 0.0f);
        
        // Compute error
        pImpl->predictionError = computePredictionError(currentPrediction, dummyActual);
        
        // Update prediction error signal
        if (pImpl->brain->getPredictionErrorSignal()) {
            pImpl->brain->getPredictionErrorSignal()->computeError(pImpl->predictionError, pImpl->predictionError);
        }
    }
}

void PredictionSystem::updatePredictionWeights(const std::vector<float>& input) {
    if (input.empty()) return;
    
    // Update prediction weights based on input using Hebbian-like learning
    float learningRate = 0.01f;
    
    for (size_t i = 0; i < std::min(input.size(), pImpl->predictionWeights.size()); ++i) {
        // Strengthen weights for correlated inputs
        pImpl->predictionWeights[i] += input[i] * learningRate;
        
        // Apply sigmoid to keep weights in reasonable range
        pImpl->predictionWeights[i] = 1.0f / (1.0f + std::exp(-pImpl->predictionWeights[i]));
    }
}

void PredictionSystem::applyPredictions(std::unique_ptr<SensoryInput>& prediction, const std::vector<float>& input) {
    if (prediction && !input.empty() && !pImpl->predictionWeights.empty()) {
        // For now, just update the prediction buffer
        // In a real implementation, we would apply the weights to generate the actual prediction
        pImpl->predictionBuffer = input;
    }
}

void PredictionSystem::adaptPredictions(const std::vector<float>& observation) {
    // Adapt predictions based on new observation using online learning
    if (observation.empty()) return;
    
    float adaptationRate = 0.02f;
    
    for (size_t i = 0; i < std::min(observation.size(), pImpl->predictionBuffer.size()); ++i) {
        // Adjust prediction based on observation error
        float target = observation[i];
        float current = pImpl->predictionBuffer[i];
        pImpl->predictionBuffer[i] += (target - current) * adaptationRate;
    }
}

void PredictionSystem::modulatePredictionByAttention(const std::vector<float>& attentionWeights) {
    // Modulate prediction generation based on attention
    if (attentionWeights.empty()) return;
    
    // Scale prediction by attention weights
    for (size_t i = 0; i < std::min(attentionWeights.size(), pImpl->predictionBuffer.size()); ++i) {
        pImpl->predictionBuffer[i] *= attentionWeights[i];
    }
}

void PredictionSystem::enhancePredictionWithWorkingMemory(const std::vector<float>& wmTrace) {
    // Enhance prediction using working memory traces
    if (wmTrace.empty()) return;
    
    // Blend working memory into prediction
    float blendFactor = 0.3f;
    for (size_t i = 0; i < std::min(wmTrace.size(), pImpl->predictionBuffer.size()); ++i) {
        pImpl->predictionBuffer[i] = pImpl->predictionBuffer[i] * (1.0f - blendFactor) + wmTrace[i] * blendFactor;
    }
}

std::vector<float> PredictionSystem::generatePrediction(const std::vector<float>& input) const {
    // Generate prediction from input using learned weights
    std::vector<float> prediction;
    
    if (pImpl->predictionBuffer.empty()) {
        return input; // Return input as baseline prediction
    }
    
    // Simple weighted prediction for now
    for (size_t i = 0; i < std::min(pImpl->predictionBuffer.size(), input.size()); ++i) {
        float weight = !pImpl->predictionWeights.empty() ? 
                      pImpl->predictionWeights[i % pImpl->predictionWeights.size()] : 0.5f;
        prediction.push_back(pImpl->predictionBuffer[i] * weight);
    }
    
    return prediction;
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    // Compute prediction error between predicted and actual values
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 0.0f;
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    return std::sqrt(sumSquaredError / predicted.size());
}

void PredictionSystem::updateConfidence() {
    // Update prediction confidence based on error history
    if (pImpl->errorHistory.empty()) {
        pImpl->confidence = 0.5f;
        return;
    }
    
    // Calculate average error from history
    float avgError = 0.0f;
    for (float error : pImpl->errorHistory) {
        avgError += error;
    }
    avgError /= pImpl->errorHistory.size();
    
    // Convert error to confidence (lower error = higher confidence)
    pImpl->confidence = std::max(0.0f, 1.0f - avgError);
}

void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
    pImpl->predictionBuffer.clear();
}

} // namespace nlm
