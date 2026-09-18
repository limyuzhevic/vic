#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> currentPrediction;
    std::vector<float> previousPrediction;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Enhanced prediction with neural integration
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Store current prediction for error calculation
    pImpl->previousPrediction = pImpl->currentPrediction;
    pImpl->currentPrediction = currentData;
    
    // Simple forward prediction based on temporal dynamics
    std::unique_ptr<SensoryInput> nextInput = std::make_unique<SensoryInput>();
    
    // Apply smooth temporal transition
    std::vector<float> predictedData = currentData;
    if (!currentData.empty()) {
        // Simulate change over time (placeholder for real prediction)
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Add temporal dynamics: smooth transitions with decay
            predictedData[i] = currentData[i] * 0.8f + 
                             (pImpl->previousPrediction.empty() ? 0.0f : pImpl->previousPrediction[i] * 0.2f);
        }
        
        // Add small random exploration for curiosity-driven behavior
        if (pImpl->brain && pImpl->brain->getCuriosity()) {
            float curiosity = pImpl->brain->getCuriosity()->getLevel();
            for (size_t i = 0; i < currentData.size(); ++i) {
                // Modulate prediction based on curiosity
                float variation = (std::rand() % 100) / 100.0f * curiosity * 0.1f;
                predictedData[i] += variation - 0.05f * curiosity;
            }
        }
    }
    
    nextInput->setData(predictedData);
    NLM_LOG_INFO("Predicted next state (confidence: " + std::to_string(getConfidence()) + ")");
    
    return nextInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation with neural integration
    // PLACEHOLDER: Calculate enhanced prediction error with neuromodulation
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Enhanced error calculation
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            if (std::abs(diff) > maxError) {
                maxError = std::abs(diff);
            }
        }
        
        pImpl->predictionError = sumError / predData.size();
        
        // Apply neuromodulation to prediction error
        if (pImpl->brain) {
            if (pImpl->brain->getPredictionErrorSignal()) {
                pImpl->brain->getPredictionErrorSignal()->recordError(pImpl->predictionError, 0);
            }
            
            if (pImpl->brain->getDopamine()) {
                // Dopamine response to prediction error
                pImpl->brain->getDopamine()->signalRewardPredictionError(pImpl->predictionError);
            }
            
            if (pImpl->brain->getCuriosity()) {
                // Curiosity increases with prediction error
                pImpl->brain->getCuriosity()->update(pImpl->predictionError, pImpl->predictionError, 0.001f);
            }
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        NLM_LOG_INFO("Prediction error updated: " + std::to_string(pImpl->predictionError) + 
                    " (confidence: " + std::to_string(pImpl->confidence) + ")");
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
    // TODO PHASE 2: Train prediction model using neural plasticity
    // PLACEHOLDER: Enhanced training with neuromodulation
    
    const auto& data = observation.getData();
    if (data.empty()) {
        return;
    }
    
    // Store observation for training
    if (pImpl->currentPrediction.empty()) {
        pImpl->currentPrediction = data;
    }
    
    // Apply reward-modulated plasticity if neuromodulation is available
    if (pImpl->brain && pImpl->brain->getDopamine()) {
        float plasticity = pImpl->brain->getDopamine()->getPlasticityFactor();
        // Update prediction weights based on dopamine-modulated learning
        // This would integrate with the brain's plasticity systems
        NLM_LOG_INFO("Prediction training with dopamine modulation (factor: " + std::to_string(plasticity) + ")");
    }
    
    NLM_LOG_INFO("Prediction model trained on observation (size: " + std::to_string(data.size()) + ")");
}

void PredictionSystem::updateWithCurrentState() {
    // Update prediction based on current brain state
    if (!pImpl->currentPrediction.empty() && pImpl->brain) {
        // Enhance prediction based on attention and working memory
        if (pImpl->brain->getAttention()) {
            float attentionWinners = pImpl->brain->getAttention()->getWinners().size();
            if (attentionWinners > 0) {
                // Boost prediction confidence based on attention
                pImpl->confidence *= (1.0f + attentionWinners * 0.1f);
                pImpl->confidence = std::min(1.0f, pImpl->confidence);
            }
        }
        
        // Enhance prediction based on working memory
        if (pImpl->brain->getWorkingMemory()) {
            float memoryActivity = pImpl->brain->getWorkingMemory()->getMemoryActivity();
            if (memoryActivity > 0.5f) {
                // Strong working memory reinforces prediction
                pImpl->confidence = std::min(1.0f, pImpl->confidence * 1.2f);
            }
        }
    }
}

float PredictionSystem::getCurrentConfidence() const {
    return pImpl->confidence;
}

} // namespace nlm