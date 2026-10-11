#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Prediction model parameters
    float predictionError;
    float confidence;
    float learningRate;
    
    // Historical data
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> patternHistory;
    std::vector<std::unique_ptr<SensoryInput>> predictionHistory;
    
    // Prediction memory (LRU cache)
    size_t memorySize;
    std::deque<std::vector<float>> recentPatterns;
    std::deque<float> patternWeights;
    
    // Prediction error modulation
    float errorDecayRate;
    float confidenceDecayRate;
    
    // Integration with brain systems
    Brain* brain;
    
    // Performance metrics
    size_t successfulPredictions;
    size_t totalPredictions;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f)
            , memorySize(100), errorDecayRate(0.01f), confidenceDecayRate(0.005f)
            , successfulPredictions(0), totalPredictions(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain integration");
}

void PredictionSystem::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

void PredictionSystem::setMemorySize(size_t size) {
    pImpl->memorySize = size;
    // Trim history if needed
    while (pImpl->recentPatterns.size() > pImpl->memorySize) {
        pImpl->recentPatterns.pop_front();
        pImpl->patternWeights.pop_front();
    }
}

void PredictionSystem::updateFromNeuralActivity(const std::vector<float>& neuralPattern) {
    if (pImpl->brain && pImpl->brain->getPredictionSystem() != this) {
        // Store neural pattern for prediction
        pImpl->recentPatterns.push_back(neuralPattern);
        pImpl->patternWeights.push_back(1.0f);  // Start with full weight
        
        // Decay weights
        for (auto& weight : pImpl->patternWeights) {
            weight *= (1.0f - pImpl->errorDecayRate);
        }
        
        // Trim to memory size
        while (pImpl->recentPatterns.size() > pImpl->memorySize) {
            pImpl->recentPatterns.pop_front();
            pImpl->patternWeights.pop_front();
        }
        
        // Update prediction model
        if (!pImpl->recentPatterns.empty()) {
            // Create average pattern from recent activity
            std::vector<float> avgPattern(pImpl->recentPatterns.front().size(), 0.0f);
            float totalWeight = 0.0f;
            
            for (size_t i = 0; i < pImpl->recentPatterns.size(); ++i) {
                float weight = pImpl->patternWeights[i];
                totalWeight += weight;
                
                for (size_t j = 0; j < avgPattern.size(); ++j) {
                    avgPattern[j] += pImpl->recentPatterns[i][j] * weight;
                }
            }
            
            if (totalWeight > 0.0f) {
                for (float& value : avgPattern) {
                    value /= totalWeight;
                }
                
                // Normalize pattern
                float norm = 0.0f;
                for (float value : avgPattern) {
                    norm += value * value;
                }
                
                if (norm > 0.0f) {
                    float scale = std::sqrt(norm);
                    for (float& value : avgPattern) {
                        value /= scale;
                    }
                }
                
                // Store in pattern history
                pImpl->patternHistory.push_back(avgPattern);
                
                // Trim pattern history
                if (pImpl->patternHistory.size() > pImpl->memorySize * 2) {
                    pImpl->patternHistory.erase(pImpl->patternHistory.begin());
                }
            }
        }
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem not initialized - brain not available");
        return nullptr;
    }
    
    pImpl->totalPredictions++;
    
    // Get current sensory pattern
    const auto& currentData = currentState.getData();
    
    // Find most similar pattern from memory
    size_t bestMatch = findBestPatternMatch(currentData);
    std::unique_ptr<SensoryInput> prediction;
    
    if (bestMatch != SIZE_MAX && !pImpl->patternHistory.empty()) {
        // Create prediction based on most similar stored pattern
        const auto& pattern = pImpl->patternHistory[bestMatch];
        
        // Convert pattern back to sensory input
        // In practice, this would map neural patterns to sensory representations
        prediction = std::make_unique<SensoryInput>(currentData.size());
        
        // Add some variation to make prediction uncertain
        for (size_t i = 0; i < currentData.size(); ++i) {
            float variation = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f;  // ±10% variation
            prediction->getData()[i] = currentData[i] + variation;
            prediction->getData()[i] = std::clamp(prediction->getData()[i], 0.0f, 1.0f);
        }
        
        // Calculate confidence based on similarity
        float similarity = calculatePatternSimilarity(currentData, pattern);
        pImpl->confidence = similarity;
        
        if (similarity > 0.7f) {
            pImpl->successfulPredictions++;
        }
        
        // Store prediction
        pImpl->predictionHistory.push_back(prediction->clone());
        
        // Trim history
        if (pImpl->predictionHistory.size() > pImpl->memorySize) {
            pImpl->predictionHistory.pop_front();
        }
        
        NLM_LOG_INFO("Prediction made with confidence: " + std::to_string(pImpl->confidence));
    } else {
        // No good match - predict current state with low confidence
        prediction = currentState.clone();
        pImpl->confidence = 0.1f;
        NLM_LOG_INFO("Prediction based on current state only - low confidence");
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error (mean squared error)
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Add prediction to history for learning
        if (!predData.empty()) {
            pImpl->patternHistory.push_back(predData);
            if (pImpl->patternHistory.size() > pImpl->memorySize * 2) {
                pImpl->patternHistory.erase(pImpl->patternHistory.begin());
            }
        }
        
        NLM_LOG_INFO("Prediction updated - error: " + std::to_string(pImpl->predictionError) + 
                   ", confidence: " + std::to_string(pImpl->confidence));
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Add to training data
    pImpl->patternHistory.push_back(data);
    
    // Update prediction model based on new observation
    if (!pImpl->recentPatterns.empty()) {
        // Adjust pattern weights based on prediction error
        for (auto& weight : pImpl->patternWeights) {
            weight *= (1.0f + pImpl->learningRate);
            weight = std::min(1.0f, weight);  // Don't exceed 1.0
        }
    }
    
    NLM_LOG_INFO("Prediction system trained with new observation");
}

std::vector<float> PredictionSystem::getSimilarPattern(const std::vector<float>& queryPattern) const {
    if (pImpl->patternHistory.empty()) {
        return std::vector<float>();
    }
    
    size_t bestMatch = findBestPatternMatch(queryPattern);
    if (bestMatch != SIZE_MAX) {
        return pImpl->patternHistory[bestMatch];
    }
    
    return std::vector<float>();
}

size_t PredictionSystem::findBestPatternMatch(const std::vector<float>& queryPattern) const {
    if (pImpl->patternHistory.empty()) {
        return SIZE_MAX;
    }
    
    float bestSimilarity = 0.0f;
    size_t bestIndex = SIZE_MAX;
    
    for (size_t i = 0; i < pImpl->patternHistory.size(); ++i) {
        float similarity = calculatePatternSimilarity(pImpl->patternHistory[i], queryPattern);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

float PredictionSystem::calculatePatternSimilarity(const std::vector<float>& pattern1,
                                                  const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
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

float PredictionSystem::getCurrentError() const {
    return pImpl->predictionError;
}

bool PredictionSystem::isReady() const {
    return pImpl->brain != nullptr && !pImpl->patternHistory.empty();
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->patternHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->recentPatterns.clear();
    pImpl->patternWeights.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->successfulPredictions = 0;
    pImpl->totalPredictions = 0;
}

void PredictionSystem::reset() {
    clearHistory();
}

} // namespace nlm
