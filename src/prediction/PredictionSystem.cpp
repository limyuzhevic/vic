#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Current prediction state
    std::vector<float> currentPrediction;
    std::vector<float> lastObservation;
    
    // Prediction error tracking
    float predictionError;
    float predictionConfidence;
    float predictionStability;
    
    // Temporal dynamics
    std::deque<float> errorHistory;
    std::deque<float> confidenceHistory;
    
    // Neural prediction components
    std::unordered_map<uint64_t, std::vector<float>> patternMemory;
    std::unordered_map<uint64_t, std::vector<float>> predictionWeights;
    
    // Prediction error propagation
    std::priority_queue<float> errorBuffer;
    float accumulatedError;
    
    // Learning components
    float learningRate;
    float predictionThreshold;
    
    Impl() : brain(nullptr), predictionError(0.0f), predictionConfidence(0.5f), 
             predictionStability(0.5f), accumulatedError(0.0f), learningRate(0.01f), 
             predictionThreshold(0.5f) {
        errorHistory.reserve(1000);
        confidenceHistory.reserve(1000);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem: Brain not set");
        return currentState.clone();
    }
    
    // Get current sensory observation
    const auto& currentData = currentState.getData();
    pImpl->lastObservation = currentData;
    
    // Use neural patterns for prediction
    generateNeuralPrediction(currentData);
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(pImpl->currentPrediction);
    
    // Update confidence based on prediction stability
    updatePredictionConfidence(currentData);
    
    NLM_LOG_DEBUG("PredictionSystem: Generated prediction with confidence " + 
                  std::to_string(pImpl->predictionConfidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error
    float totalError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        totalError += error * error;
        maxError = std::max(maxError, std::abs(error));
    }
    
    pImpl->predictionError = std::sqrt(totalError / predData.size());
    
    // Update accumulated error for learning
    pImpl->accumulatedError += pImpl->predictionError;
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.pop_front();
    }
    
    // Update prediction stability based on error
    if (pImpl->predictionError > pImpl->predictionThreshold) {
        pImpl->predictionStability *= (1.0f - pImpl->learningRate);
        NLM_LOG_INFO("PredictionSystem: High prediction error, reducing stability to " + 
                     std::to_string(pImpl->predictionStability));
    } else {
        pImpl->predictionStability = std::min(1.0f, pImpl->predictionStability + 0.001f);
    }
    
    // Train neural prediction networks with error
    trainPredictionNetworks(predData, actualData, pImpl->predictionError);
    
    // Propagate error through neural systems
    propagatePredictionErrorThroughBrain(pImpl->predictionError);
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->predictionConfidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->confidenceHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->accumulatedError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Create pattern representation
    uint64_t patternId = generatePatternId(data);
    
    // Store observation in pattern memory
    pImpl->patternMemory[patternId] = data;
    
    // Update prediction weights based on this observation
    updatePredictionWeights(patternId, data);
    
    // Retrain prediction networks
    retrainPredictionNetworks();
    
    NLM_LOG_DEBUG("PredictionSystem: Trained with observation of size " + std::to_string(data.size()));
}

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with neural integration");
}

void PredictionSystem::generateNeuralPrediction(const std::vector<float>& currentData) {
    if (currentData.empty()) return;
    
    // Use brain connectivity for prediction
    if (!pImpl->brain) return;
    
    // Generate prediction based on current neural state
    pImpl->currentPrediction.clear();
    pImpl->currentPrediction.reserve(currentData.size());
    
    // Start with current state as base prediction
    pImpl->currentPrediction = currentData;
    
    // Apply learned transformations based on prediction weights
    for (const auto& [patternId, weights] : pImpl->predictionWeights) {
        if (weights.size() == currentData.size()) {
            for (size_t i = 0; i < currentData.size(); ++i) {
                pImpl->currentPrediction[i] *= weights[i];
            }
        }
    }
    
    // Apply neural dynamics (simple leaky integration)
    for (auto& pred : pImpl->currentPrediction) {
        pred = pred * 0.9f + pred * 0.1f;  // Small dynamics update
    }
}

void PredictionSystem::updatePredictionConfidence(const std::vector<float>& currentData) {
    if (currentData.empty()) {
        pImpl->predictionConfidence = 0.0f;
        return;
    }
    
    // Compute confidence based on pattern matching
    float similaritySum = 0.0f;
    int matchCount = 0;
    
    uint64_t patternId = generatePatternId(currentData);
    
    // Check similarity to stored patterns
    for (const auto& [id, pattern] : pImpl->patternMemory) {
        if (id == patternId) continue;  // Skip self
        
        float similarity = computePatternSimilarity(currentData, pattern);
        if (similarity > 0.1f) {
            similaritySum += similarity;
            matchCount++;
        }
    }
    
    if (matchCount > 0) {
        pImpl->predictionConfidence = similaritySum / matchCount;
        pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence * 2.0f);
    } else {
        pImpl->predictionConfidence = 0.1f;  // Low confidence for novel patterns
    }
    
    pImpl->confidenceHistory.push_back(pImpl->predictionConfidence);
    if (pImpl->confidenceHistory.size() > 1000) {
        pImpl->confidenceHistory.pop_front();
    }
}

void PredictionSystem::trainPredictionNetworks(const std::vector<float>& predData,
                                              const std::vector<float>& actualData,
                                              float error) {
    if (predData.size() != actualData.size()) return;
    
    // Update prediction weights based on prediction error
    for (size_t i = 0; i < predData.size(); ++i) {
        float target = actualData[i];
        float current = predData[i];
        
        // Gradient descent update
        float gradient = 2.0f * (current - target);
        float learningAdjustment = -pImpl->learningRate * gradient;
        
        // Store adjustment in weight buffer
        if (i < pImpl->predictionWeights.size()) {
            // Update existing pattern weight
            for (auto& [patternId, weights] : pImpl->predictionWeights) {
                if (i < weights.size()) {
                    weights[i] += learningAdjustment;
                    weights[i] = std::clamp(weights[i], 0.0f, 2.0f);
                }
            }
        }
    }
}

void PredictionSystem::updatePredictionWeights(uint64_t patternId, const std::vector<float>& pattern) {
    // Create or update prediction weights for this pattern
    std::vector<float> newWeights(pattern.size(), 1.0f);
    
    // Weight based on pattern frequency and stability
    size_t patternCount = pImpl->patternMemory.count(patternId);
    float weightFactor = 1.0f / std::max(1.0f, static_cast<float>(patternCount));
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        newWeights[i] = pattern[i] * weightFactor;
    }
    
    pImpl->predictionWeights[patternId] = newWeights;
}

void PredictionSystem::retrainPredictionNetworks() {
    // Periodically retrain all prediction networks
    if (pImpl->patternMemory.size() < 10) return;  // Need enough data
    
    // Compute average weights across all patterns
    std::vector<float> avgWeights(1024, 0.0f);  // Assume max pattern size
    size_t totalPatterns = 0;
    
    for (const auto& [patternId, weights] : pImpl->predictionWeights) {
        if (weights.size() < avgWeights.size()) {
            for (size_t i = 0; i < weights.size(); ++i) {
                avgWeights[i] += weights[i];
            }
            totalPatterns++;
        }
    }
    
    if (totalPatterns > 0) {
        for (size_t i = 0; i < avgWeights.size(); ++i) {
            avgWeights[i] /= totalPatterns;
        }
        
        // Update predictionWeights with average for novel predictions
        for (auto& [patternId, weights] : pImpl->predictionWeights) {
            if (weights.size() != avgWeights.size()) {
                weights = avgWeights;
            }
        }
    }
}

void PredictionSystem::propagatePredictionErrorThroughBrain(float error) {
    if (!pImpl->brain || error < 0.001f) return;
    
    // Use prediction error to modulate neural excitability
    float errorModulation = error * 2.0f;  // Scale error to reasonable range
    
    // Apply to working memory
    if (auto* workingMem = pImpl->brain->getWorkingMemory()) {
        // Increase working memory capacity based on prediction error
        workingMem->setDecayRate(workingMem->getDecayRate() * (1.0f + errorModulation * 0.1f));
    }
    
    // Apply to attention system
    if (auto* attention = pImpl->brain->getAttention()) {
        // Focus attention on surprising inputs
        attention->setInhibitionStrength(attention->getInhibitionStrength() * (1.0f - errorModulation * 0.05f));
    }
    
    // Update prediction error signal in brain
    if (auto* predError = pImpl->brain->getPredictionErrorSignal()) {
        predError->update(error);
    }
    
    // Update neuromodulation
    if (auto* dopamine = pImpl->brain->getDopamine()) {
        // Dopamine scales with prediction error (reward prediction error)
        dopamine->update(error);
    }
}

uint64_t PredictionSystem::generatePatternId(const std::vector<float>& pattern) {
    if (pattern.empty()) return 0;
    
    // Simple hash function for pattern identification
    uint64_t hash = 0;
    for (float val : pattern) {
        hash = hash * 31 + static_cast<uint64_t>(val * 1000.0f);
    }
    
    return hash;
}

float PredictionSystem::computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm