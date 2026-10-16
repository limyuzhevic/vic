#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction network weights
    std::vector<float> predictionWeights;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Learning parameters
    float learningRate;
    float predictionDecay;
    
    // Internal state
    float lastPredictionError;
    
    // For pattern completion
    struct Pattern {
        std::vector<float> weights;
        float activation;
        float predictedReward;
        size_t lastUpdated;
    };
    std::vector<Pattern> patternMemory;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), predictionDecay(0.95f), lastPredictionError(0.0f) {
        predictionWeights.resize(256, 0.0f);  // Match typical sensory input size
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement neural prediction using the brain's working memory and prediction weights
    std::unique_ptr<SensoryInput> predictedInput = std::make_unique<SensoryInput>();
    
    const auto& currentData = currentState.getData();
    std::vector<float>& predictedData = predictedInput->getData();
    
    // If no current prediction weights, use current state
    if (pImpl->predictionWeights.empty() || currentData.empty()) {
        predictedData = currentData;
        return predictedInput;
    }
    
    // Apply neural prediction using pattern completion
    predictedData.resize(currentData.size());
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Neural prediction combines current input with learned weights
        // Use exponential decay for older predictions
        float weight = pImpl->predictionWeights[i % pImpl->predictionWeights.size()];
        
        // Add some randomness for exploration
        static std::mt19937 rng(42);  // Fixed for reproducibility
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
        
        // Prediction with noise
        predictedData[i] = currentData[i] * weight + dist(rng);
        
        // Decay over time (simulate forgetting)
        pImpl->predictionWeights[i] *= pImpl->predictionDecay;
    }
    
    // Update confidence based on prediction error
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError * 2.0f);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error using weighted difference
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            // Weight errors based on feature importance
            float weight = 1.0f + pImpl->predictionWeights[i % pImpl->predictionWeights.size()];
            sumError += diff * diff * weight;
        }
        
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Learn from prediction error
        learnFromError(predData, actualData, pImpl->predictionError);
    }
}

void PredictionSystem::learnFromError(const std::vector<float>& predicted,
                                     const std::vector<float>& actual,
                                     float error) {
    if (predicted.size() != actual.size() || predicted.empty()) return;
    
    // Update prediction weights based on error
    for (size_t i = 0; i < predicted.size(); ++i) {
        // Hebbian-like learning: strengthen connections for accurate predictions
        float delta = error * (actual[i] - predicted[i]) * pImpl->learningRate;
        
        // Update weight with bounds
        pImpl->predictionWeights[i] += delta;
        pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], 0.0f, 1.0f);
        
        // Store pattern in memory
        storePattern(predicted, actual, error);
    }
}

void PredictionSystem::storePattern(const std::vector<float>& predicted,
                                   const std::vector<float>& actual,
                                   float error) {
    // Create pattern entry
    Pattern pattern;
    pattern.weights = predicted;
    pattern.activation = 1.0f - std::abs(error);
    pattern.predictedReward = 1.0f - error; // Reward for accurate prediction
    pattern.lastUpdated = pImpl->errorHistory.size();
    
    // Add to pattern memory
    pImpl->patternMemory.push_back(pattern);
    
    // Maintain memory size
    if (pImpl->patternMemory.size() > 100) {
        pImpl->patternMemory.erase(pImpl->patternMemory.begin());
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
    pImpl->patternMemory.clear();
}

void PredictionSystem::trainFromPattern(const std::vector<float>& pattern) {
    // Train prediction system from neural pattern
    // Use pattern completion and learning mechanisms
    
    // Update prediction weights based on pattern
    if (pattern.size() > pImpl->predictionWeights.size()) {
        pImpl->predictionWeights.resize(pattern.size(), 0.0f);
    }
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Hebbian-like learning from pattern
        pImpl->predictionWeights[i] = (pImpl->predictionWeights[i] * 0.8f) + (pattern[i] * 0.2f);
        pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], 0.0f, 1.0f);
    }
    
    // Store pattern in memory for retrieval
    storePatternFromVector(pattern);
}

void PredictionSystem::storePatternFromVector(const std::vector<float>& pattern) {
    // Create pattern entry for completion
    PatternEntry entry;
    entry.pattern = pattern;
    entry.lastUsed = 0;
    entry.accessCount = 1;
    
    // Add to pattern store
    pImpl->patternMemory.push_back(entry);
    
    // Maintain memory size
    if (pImpl->patternMemory.size() > 50) {
        pImpl->patternMemory.erase(pImpl->patternMemory.begin());
    }
}

struct PredictionSystem::PatternEntry {
    std::vector<float> pattern;
    size_t lastUsed;
    size_t accessCount;
};

void PredictionSystem::completePattern(const std::vector<float>& partialPattern,
                                      std::vector<float>& completedPattern) const {
    // Complete partial pattern by filling in missing values
    completedPattern = partialPattern;
    
    // If we have stored patterns, try to complete
    if (!pImpl->patternMemory.empty() && !partialPattern.empty()) {
        // Find best matching stored pattern
        float bestSimilarity = 0.0f;
        const PatternEntry* bestEntry = nullptr;
        
        for (const auto& entry : pImpl->patternMemory) {
            if (entry.pattern.size() == partialPattern.size()) {
                // Compute similarity
                float similarity = 0.0f;
                for (size_t i = 0; i < partialPattern.size(); ++i) {
                    similarity += (1.0f - std::abs(partialPattern[i] - entry.pattern[i]));
                }
                similarity /= partialPattern.size();
                
                if (similarity > bestSimilarity) {
                    bestSimilarity = similarity;
                    bestEntry = &entry;
                }
            }
        }
        
        // Complete using best match
        if (bestSimilarity > 0.5f && bestEntry) {
            for (size_t i = 0; i < completedPattern.size(); ++i) {
                if (completedPattern[i] < 0.1f) {  // Missing value
                    completedPattern[i] = bestEntry->pattern[i];
                }
            }
        }
    }
}

} // namespace nlm
