#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <mutex>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() 
    : pImpl(new Impl)
    , curiosityLevel_(0.0f)
    , attentionWeight_(0.0f)
    , learningSignal_(0.0f) {
    NLM_LOG_INFO("PredictionSystem: Created enhanced prediction system");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    std::lock_guard<std::mutex> lock(modelMutex_);
    
    // Create prediction based on learned patterns
    auto predictedInput = std::make_unique<SensoryInput>();
    
    if (!averagePattern_.empty() && !observationHistory_.empty()) {
        // Add some noise to make prediction uncertain
        std::vector<float> predictedData = averagePattern_;
        float noiseLevel = 0.1f; // Prediction uncertainty
        
        // Initialize random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> noiseDist(0.0f, noiseLevel);
        
        for (size_t i = 0; i < predictedData.size(); ++i) {
            float noise = noiseDist(gen);
            predictedData[i] += noise;
        }
        
        predictedInput->setData(predictedData);
        
        // Store current state as last prediction
        lastPredictedState_ = currentState.getData();
        
    } else {
        // If no learned patterns, just repeat current state with noise
        auto currentData = currentState.getData();
        std::vector<float> predictedData = currentData;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> noiseDist(0.0f, 0.05f);
        
        for (size_t i = 0; i < predictedData.size(); ++i) {
            float noise = noiseDist(gen);
            predictedData[i] += noise;
        }
        
        predictedInput->setData(predictedData);
        lastPredictedState_ = currentData;
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted next state from current observation");
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    std::lock_guard<std::mutex> lock(modelMutex_);
    
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() == actualData.size() && !predictedData.empty()) {
        float sumSquaredError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predictedData.size(); ++i) {
            float error = predictedData[i] - actualData[i];
            sumSquaredError += error * error;
            maxError = std::max(maxError, std::abs(error));
        }
        
        pImpl->predictionError = std::sqrt(sumSquaredError / predictedData.size());
        pImpl->predictionError = std::min(pImpl->predictionError, 1.0f); // Clamp to [0,1]
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error (lower error = higher confidence)
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        // Use prediction error to modulate learning
        usePredictionError(pImpl->predictionError);
        
    } else {
        pImpl->predictionError = 1.0f; // Maximum error for mismatch
        pImpl->confidence = 0.1f;
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Updated predictions, error = " + std::to_string(pImpl->predictionError));
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Simple prediction model training
    // Track statistics of observations for prediction
    std::lock_guard<std::mutex> lock(modelMutex_);
    
    // Add to observation history
    observationHistory_.push_back(observation.getData());
    if (observationHistory_.size() > 1000) {
        observationHistory_.erase(observationHistory_.begin());
    }
    
    // Compute average patterns
    if (!observationHistory_.empty()) {
        size_t totalSize = 0;
        std::vector<float> sumPattern;
        for (const auto& obs : observationHistory_) {
            totalSize += obs.size();
            if (sumPattern.empty()) {
                sumPattern = obs;
            } else if (sumPattern.size() == obs.size()) {
                for (size_t i = 0; i < obs.size(); ++i) {
                    sumPattern[i] += obs[i];
                }
            }
        }
        
        if (totalSize > 0 && !sumPattern.empty()) {
            averagePattern_.resize(sumPattern.size());
            for (size_t i = 0; i < sumPattern.size(); ++i) {
                averagePattern_[i] = sumPattern[i] / totalSize;
            }
        }
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Trained on observation, patterns = " + std::to_string(observationHistory_.size()));
}

void PredictionSystem::usePredictionError(float error) {
    // Use prediction error to modulate other brain systems
    // This is a simplified implementation - real systems would have more complex interactions
    
    // Update curiosity - prediction errors drive exploration
    curiosityLevel_ = error * 2.0f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Update attention - high prediction errors draw attention
    attentionWeight_ = error * 0.5f;
    attentionWeight_ = std::clamp(attentionWeight_, 0.0f, 1.0f);
    
    // Record learning signal
    learningSignal_ = error;
    
    // For now, we could update neural weights based on prediction error
    // In a real implementation, this would involve synaptic plasticity
    
    NLM_LOG_DEBUG("PredictionSystem: Used prediction error = " + std::to_string(error));
}

float PredictionSystem::getCuriosityLevel() const {
    return curiosityLevel_;
}

float PredictionSystem::getAttentionWeight() const {
    return attentionWeight_;
}

float PredictionSystem::getLearningSignal() const {
    return learningSignal_;
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
    std::lock_guard<std::mutex> lock(modelMutex_);
    
    pImpl->errorHistory.clear();
    observationHistory_.clear();
    averagePattern_.clear();
    
    NLM_LOG_INFO("PredictionSystem: History cleared");
}

void PredictionSystem::reset() {
    std::lock_guard<std::mutex> lock(modelMutex_);
    
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
    
    observationHistory_.clear();
    averagePattern_.clear();
    
    curiosityLevel_ = 0.0f;
    attentionWeight_ = 0.0f;
    learningSignal_ = 0.0f;
    
    NLM_LOG_INFO("PredictionSystem: Reset");
}

} // namespace nlm