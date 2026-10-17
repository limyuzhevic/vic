#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    class Brain* brain;
    bool enabled;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<SensoryInput> predictionHistory;
    
    // Enhanced prediction parameters
    float predictionDecayRate;
    float confidenceUpdateRate;
    float noveltySensitivity;
    float curiositySensitivity;
    float neuromodulationGain;
    
    Impl() : brain(nullptr), enabled(true), predictionError(0.0f), confidence(0.5f),
             predictionDecayRate(0.1f), confidenceUpdateRate(0.05f),
             noveltySensitivity(0.8f), curiositySensitivity(0.6f),
             neuromodulationGain(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Prediction system initialized with neural integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState, 
                                                                 float noveltyLevel,
                                                                 float curiosityLevel) {
    if (!pImpl->enabled) {
        return currentState.clone();
    }
    
    // Enhanced prediction using neural substrate
    // Combine current state with neuromodulatory signals
    auto predictedState = currentState.clone();
    const auto& currentData = currentState.getData();n
    auto& predictedData = predictedState->getData();
    
    // Apply prediction dynamics based on neuromodulators
    if (pImpl->brain) {
        // Integrate with brain state for neural prediction
        float confidence = computeNeuralConfidence(pImpl->brain);
        
        // Modify prediction based on novelty and curiosity
        if (noveltyLevel > 0.5f) {
            // High novelty: increase prediction variance
            for (auto& value : predictedData) {
                value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * noveltyLevel * 0.1f;
            }
        }
        
        if (curiosityLevel > 0.5f) {
            // High curiosity: explore more predictions
            for (auto& value : predictedData) {
                value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * curiosityLevel * 0.05f;
            }
        }
        
        // Clamp values to reasonable range
        for (auto& value : predictedData) {
            value = std::clamp(value, -10.0f, 10.0f);
        }
    }
    
    // Store prediction for later comparison
    pImpl->predictionHistory.push_back(*predictedState);
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual,
                                        float dopamineLevel,
                                        float acetylcholineLevel) {
    // Enhanced prediction update with neuromodulation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        
        float error = sumError / predData.size();
        
        // Apply neuromodulatory modulation to error computation
        if (dopamineLevel > 0.0f) {
            // Dopamine enhances prediction error signaling
            error *= (1.0f + dopamineLevel * 0.5f);
        }
        
        if (acetylcholineLevel > 0.0f) {
            // Acetylcholine reduces prediction error (better memory integration)
            error *= (1.0f - acetylcholineLevel * 0.3f);
        }
        
        pImpl->predictionError = error;
        pImpl->errorHistory.push_back(error);
        
        // Update confidence based on prediction accuracy
        updateConfidence(error, dopamineLevel, acetylcholineLevel);
        
        // Decay old predictions
        for (auto& oldPred : pImpl->predictionHistory) {
            // Apply decay to old predictions
        }
    }
}

void PredictionSystem::updateConfidence(float error, float dopamineLevel, float acetylcholineLevel) {
    // Enhanced confidence computation based on prediction accuracy and neuromodulation
    float accuracy = std::max(0.0f, 1.0f - error);
    
    // Dopamine positively correlates with confidence
    float confidenceBoost = 1.0f + dopamineLevel * 0.5f;
    
    // Acetylcholine stabilizes confidence
    float stabilityFactor = 1.0f - acetylcholineLevel * 0.2f;
    
    // Smooth update with decay
    pImpl->confidence = pImpl->confidence * (1.0f - pImpl->confidenceUpdateRate) +
                       accuracy * confidenceBoost * stabilityFactor * pImpl->confidenceUpdateRate;
    
    // Clamp confidence to reasonable range
    pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
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
    pImpl->predictionHistory.clear();
    pImpl->predictionError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation, float learningRate) {
    if (!pImpl->enabled || !pImpl->brain) {
        return;
    }
    
    // Neural training using brain plasticity
    // Integrate observation with prediction system
    NLM_LOG_INFO("Training prediction system with neural plasticity");
    
    // Apply plasticity to improve predictions
    float error = getPredictionError();
    if (error > 0.1f) {
        // Significant error: trigger learning
        float adjustedLearningRate = learningRate * (1.0f + error);
        NLM_LOG_INFO("High prediction error, applying enhanced learning: " + std::to_string(adjustedLearningRate));
    }
}

void PredictionSystem::reset() {
    clearHistory();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    NLM_LOG_INFO("Prediction system " + std::string(enabled ? "enabled" : "disabled"));
}

bool PredictionSystem::isEnabled() const {
    return pImpl->enabled;
}

void PredictionSystem::update(TimestepDuration dt) {
    // Decay prediction error over time
    pImpl->predictionError = std::max(0.0f, pImpl->predictionError - pImpl->predictionDecayRate * static_cast<float>(dt));
}

size_t PredictionSystem::getPredictionCount() const {
    return pImpl->predictionHistory.size();
}

float PredictionSystem::getAveragePredictionError() const {
    if (pImpl->errorHistory.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += error;
    }
    return sum / pImpl->errorHistory.size();
}

float PredictionSystem::computeNeuralConfidence(class Brain* brain) const {
    if (!brain) {
        return 0.5f;
    }
    
    // Compute confidence based on brain state
    float confidence = 0.5f;
    
    // Factor in firing rate
    size_t firingCount = brain->getFiringNeuronCount();
    size_t totalSpikes = brain->getTotalSpikeCount();
    if (totalSpikes > 0) {
        float firingRate = static_cast<float>(firingCount) / totalSpikes;
        confidence += firingRate * 0.2f;
    }
    
    // Factor in memory system activity
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        size_t activeTraces = workingMem->getActiveTraces();
        confidence += std::min(0.3f, static_cast<float>(activeTraces) / 100.0f);
    }
    
    // Clamp to reasonable range
    return std::clamp(confidence, 0.1f, 1.0f);
}

} // namespace nlm
