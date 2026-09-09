#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction weights (stored in working memory)
    std::vector<float> predictionWeights;
    std::vector<float> weightCorrections;
    std::vector<float> eligibilityTraces;
    
    // Fast and slow learning rates
    float fastLearningRate;
    float slowLearningRate;
    
    // Prediction statistics
    float predictionAccuracy;
    float predictionVariance;
    float explorationBonus;
    
    // Neural working memory integration
    size_t maxWorkingMemorySize;
    size_t minWorkingMemorySize;
    
    // Prediction error tracking
    float predictionError;
    float predictionErrorVariance;
    float predictionErrorGain;
    
    // Confidence and uncertainty
    float confidence;
    float uncertainty;
    
    // Prediction history for statistics
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<float> accuracyHistory;
    std::vector<float> explorationHistory;
    
    // Brain reference for neural integration
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), uncertainty(0.5f),
             fastLearningRate(0.1f), slowLearningRate(0.01f),
             predictionAccuracy(0.5f), predictionVariance(0.25f),
             explorationBonus(0.0f), predictionErrorVariance(0.25f),
             predictionErrorGain(1.0f), brain(nullptr) {
        predictionWeights.resize(256, 0.0f);
        weightCorrections.assign(256, 0.0f);
        eligibilityTraces.assign(256, 0.0f);
        maxWorkingMemorySize = 128;
        minWorkingMemorySize = 16;
        errorHistory.reserve(1000);
        confidenceHistory.reserve(1000);
        accuracyHistory.reserve(1000);
        explorationHistory.reserve(1000);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // In a full implementation, brain would be set by the Brain class
    // This is a placeholder - actual integration happens through Brain::getPredictionSystem()
}

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::storePattern(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return;
    
    // Store pattern in prediction weights for learning
    // Use neural working memory to influence prediction weights
    std::vector<float> workingMemoryPattern = brain_->getWorkingMemory()->retrieve();
    
    if (!workingMemoryPattern.empty() && pattern.size() == pImpl->predictionWeights.size()) {
        // Combine working memory patterns with new observations
        for (size_t i = 0; i < pattern.size(); ++i) {
            // Blend new pattern with existing prediction weights
            float weight = 0.7f; // Learning rate for pattern storage
            pImpl->predictionWeights[i] = weight * pattern[i] + (1.0f - weight) * pImpl->predictionWeights[i];
            
            // Also update working memory with this pattern
            brain_->getWorkingMemory()->store(pattern, 1.0f);
        }
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Get current neural activity patterns from working memory
    std::vector<float> currentPattern;
    if (brain_) {
        NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            currentPattern = workingMemory->retrieve();
        }
    }
    
    // Use neural firing patterns as predictions
    std::unique_ptr<SensoryInput> prediction = currentState.clone();
    auto& predData = prediction->getData();
    
    if (!currentPattern.empty() && !predData.empty()) {
        // Apply prediction weights to current neural patterns
        size_t minSize = std::min(currentPattern.size(), predData.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            // Use prediction weights to scale neural activity
            float weight = pImpl->predictionWeights[i % pImpl->predictionWeights.size()];
            predData[i] = currentPattern[i] * weight;
            
            // Add some prediction noise based on uncertainty
            float noise = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * pImpl->uncertainty * 0.1f;
            predData[i] += noise;
        }
        
        // Fill remaining data with scaled version of existing values
        if (predData.size() > minSize) {
            float avgValue = std::accumulate(predData.begin(), predData.begin() + minSize, 0.0f) / minSize;
            for (size_t i = minSize; i < predData.size(); ++i) {
                predData[i] = avgValue * 0.8f;
            }
        }
    } else {
        // Fallback if no pattern available
        for (float& value : predData) {
            value *= 0.8f;  // Decay prediction slightly
        }
    }
    
    // Update prediction statistics
    updatePredictionStatistics(*prediction);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error based on neural difference
        float sumSquaredError = 0.0f;
        float sumAbsError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = actualData[i] - predData[i];
            sumSquaredError += error * error;
            sumAbsError += std::abs(error);
        }
        
        // Update prediction error
        float oldError = pImpl->predictionError;
        pImpl->predictionError = sumSquaredError / predData.size();
        
        // Update error variance for uncertainty tracking
        float errorChange = pImpl->predictionError - oldError;
        pImpl->predictionErrorVariance = 0.9f * pImpl->predictionErrorVariance + 0.1f * errorChange * errorChange;
        
        // Update prediction error statistics (neuromodulation)
        updatePredictionErrorStats(pImpl->predictionError);
        
        // Update error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction weights based on error (supervised learning)
        updatePredictionWeights(predData, actualData);
        
        // Update confidence based on prediction error and error variance
        updateConfidence();
        
        // Update exploration bonus based on prediction error
        pImpl->explorationBonus = std::sqrt(pImpl->predictionError) * pImpl->predictionErrorGain;
    }
}

void PredictionSystem::updatePredictionWeights(const std::vector<float>& predicted, const std::vector<float>& actual) {
    // Compute weight corrections using delta rule
    // Fast learning for recent errors
    float delta = pImpl->fastLearningRate;
    
    // Use working memory to modulate learning rate
    if (brain_) {
        NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            float memoryActivity = workingMemory->getMemoryActivity();
            delta *= (1.0f + memoryActivity * 0.5f);
        }
    }
    
    // Update prediction weights based on prediction error
    for (size_t i = 0; i < predicted.size() && i < pImpl->predictionWeights.size(); ++i) {
        float weightChange = delta * (actual[i] - predicted[i]) * predicted[i];
        pImpl->weightCorrections[i] = weightChange;
        
        // Apply weight corrections with momentum
        pImpl->predictionWeights[i] += weightChange;
        
        // Apply eligibility traces for delayed credit assignment
        pImpl->eligibilityTraces[i] = 0.9f * pImpl->eligibilityTraces[i] + 1.0f;
        pImpl->predictionWeights[i] += pImpl->slowLearningRate * pImpl->eligibilityTraces[i] * weightChange;
        
        // Clamp weights to reasonable range
        pImpl->predictionWeights[i] = std::max(0.0f, std::min(pImpl->predictionWeights[i], 2.0f));
    }
}

void PredictionSystem::updateConfidence() {
    // Update confidence based on prediction error and error history
    float errorStdDev = std::sqrt(pImpl->predictionErrorVariance);
    
    // Confidence is inverse of prediction error variance
    // Clamp to avoid division by zero
    float errorVariance = std::max(pImpl->predictionErrorVariance, 0.01f);
    pImpl->confidence = 1.0f - std::min(1.0f, std::sqrt(errorVariance));
    
    // Smooth confidence update
    pImpl->confidence = 0.9f * pImpl->confidence + 0.1f * (1.0f - pImpl->predictionError);
    
    // Update uncertainty based on confidence
    pImpl->uncertainty = 1.0f - pImpl->confidence;
    
    // Update confidence history
    pImpl->confidenceHistory.push_back(pImpl->confidence);
    if (pImpl->confidenceHistory.size() > 1000) {
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
    }
}

void PredictionSystem::updatePredictionErrorStats(float error) {
    // Update prediction error statistics for neuromodulation
    // This integrates with dopamine and curiosity systems
    if (brain_) {
        // Find prediction error neuromodulator
        auto* predError = brain_->getPredictionErrorSignal();
        if (predError) {
            // Scale error by prediction confidence
            float scaledError = error * pImpl->confidence;
            predError->updatePredictionError(scaledError);
        }
        
        // Update curiosity based on prediction error
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) {
            // High prediction error increases curiosity
            float curiosityBoost = error * 0.5f * (1.0f - pImpl->confidence);
            curiosity->increaseCuriosity(curiosityBoost);
        }
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Update prediction model weights using prediction errors
    const auto& obsData = observation.getData();
    
    if (!obsData.empty()) {
        // Fast learning - immediate weight updates from recent prediction errors
        fastLearning(obsData);
        
        // Slow learning - gradual consolidation of predictions
        slowLearning(obsData);
        
        // Update prediction accuracy statistics
        updatePredictionStatistics(observation);
        
        // Store trained observation in working memory
        if (brain_) {
            brain_->getWorkingMemory()->store(obsData, 1.0f);
        }
    }
}

void PredictionSystem::fastLearning(const std::vector<float>& observation) {
    // Fast learning with high learning rate for immediate correction
    float learningRate = pImpl->fastLearningRate;
    
    // Use working memory to modulate learning
    if (brain_) {
        NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            std::vector<float> workingMemoryPattern = workingMemory->retrieve();
            
            // Apply working memory strength to learning rate
            float memoryStrength = workingMemory->getMemoryActivity();
            learningRate *= (1.0f + memoryStrength);
            
            // Update weights based on working memory patterns
            for (size_t i = 0; i < workingMemoryPattern.size() && i < pImpl->predictionWeights.size(); ++i) {
                float prediction = pImpl->predictionWeights[i];
                float error = observation[i % observation.size()] - prediction;
                float weightChange = learningRate * workingMemoryPattern[i] * error;
                pImpl->predictionWeights[i] += weightChange;
                pImpl->eligibilityTraces[i] += weightChange;
            }
        }
    }
    
    // Fallback learning without working memory
    if (!brain_ || brain_->getWorkingMemory()->getActiveTraces() == 0) {
        for (size_t i = 0; i < std::min(observation.size(), pImpl->predictionWeights.size()); ++i) {
            float error = observation[i] - pImpl->predictionWeights[i];
            float weightChange = learningRate * error;
            pImpl->predictionWeights[i] += weightChange;
        }
    }
}

void PredictionSystem::slowLearning(const std::vector<float>& observation) {
    // Slow learning for memory consolidation
    float learningRate = pImpl->slowLearningRate;
    
    // Use eligibility traces for delayed credit assignment
    for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
        // Apply slow learning with eligibility traces
        float prediction = pImpl->predictionWeights[i];
        float error = observation[i % observation.size()] - prediction;
        float weightUpdate = learningRate * pImpl->eligibilityTraces[i] * error;
        pImpl->predictionWeights[i] += weightUpdate;
        
        // Decay eligibility traces
        pImpl->eligibilityTraces[i] *= 0.95f;
    }
}

void PredictionSystem::updatePredictionStatistics(const SensoryInput& observation) {
    const auto& obsData = observation.getData();
    
    if (!obsData.empty()) {
        // Update prediction accuracy - measure how well predictions match reality
        float predictionMatch = 0.0f;
        size_t count = 0;
        
        for (size_t i = 0; i < std::min(obsData.size(), pImpl->predictionWeights.size()); ++i) {
            float prediction = pImpl->predictionWeights[i];
            predictionMatch += 1.0f - std::abs(prediction - obsData[i]);
            count++;
        }
        
        if (count > 0) {
            predictionMatch /= count;
            pImpl->predictionAccuracy = 0.9f * pImpl->predictionAccuracy + 0.1f * predictionMatch;
            
            // Update accuracy history
            pImpl->accuracyHistory.push_back(pImpl->predictionAccuracy);
            if (pImpl->accuracyHistory.size() > 1000) {
                pImpl->accuracyHistory.erase(pImpl->accuracyHistory.begin());
            }
        }
        
        // Update prediction variance based on weight distribution
        float variance = 0.0f;
        if (!pImpl->predictionWeights.empty()) {
            float mean = std::accumulate(pImpl->predictionWeights.begin(), 
                                       pImpl->predictionWeights.end(), 0.0f) / pImpl->predictionWeights.size();
            for (float weight : pImpl->predictionWeights) {
                variance += (weight - mean) * (weight - mean);
            }
            variance /= pImpl->predictionWeights.size();
        }
        pImpl->predictionVariance = 0.8f * pImpl->predictionVariance + 0.2f * variance;
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
    pImpl->confidenceHistory.clear();
    pImpl->accuracyHistory.clear();
    pImpl->explorationHistory.clear();
    // Reset some internal states
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->uncertainty = 0.5f;
}

} // namespace nlm
