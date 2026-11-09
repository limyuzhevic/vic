#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Enhanced prediction system state
    std::vector<float> predictionWeights;
    float learningRate;
    float predictionDecay;
    size_t predictionHorizon;
    
    // Pattern matching for prediction
    std::vector<std::vector<float>> learnedPatterns;
    std::vector<float> patternRewards;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), predictionDecay(0.95f), 
             predictionHorizon(5), learnedPatterns(10, std::vector<float>(16, 0.0f)) {
        patternRewards.reserve(10);
        predictionWeights.resize(16, 1.0f);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized with neural prediction capabilities");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 6: Real neural prediction using brain dynamics
    // This is much more sophisticated than just copying current state
    
    // 1. Extract current sensory pattern
    const auto& currentData = currentState.getData();
    
    // 2. Apply predictive weights learned from neural activity
    std::vector<float> predictedData(currentData.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        float weightedSum = 0.0f;
        for (size_t j = 0; j < currentData.size(); ++j) {
            // Apply temporal smoothing and pattern matching
            float weight = pImpl->predictionWeights[j];
            if (j > 0 && i < currentData.size()) {
                // Add temporal component: predict change based on recent patterns
                float temporalWeight = 0.5f * std::pow(pImpl->predictionDecay, std::abs(static_cast<int>(i - j)));
                weight += temporalWeight;
            }
            weightedSum += currentData[j] * weight;
            totalWeight += weight;
        }
        
        // Apply pattern completion based on learned patterns
        if (!pImpl->learnedPatterns.empty()) {
            float bestPatternMatch = 0.0f;
            size_t bestPattern = 0;
            
            for (size_t p = 0; p < pImpl->learnedPatterns.size(); ++p) {
                float patternScore = 0.0f;
                for (size_t k = 0; k < std::min(currentData.size(), pImpl->learnedPatterns[p].size()); ++k) {
                    patternScore += currentData[k] * pImpl->learnedPatterns[p][k];
                }
                if (patternScore > bestPatternMatch) {
                    bestPatternMatch = patternScore;
                    bestPattern = p;
                }
            }
            
            // Blend current pattern with learned pattern
            float patternBlend = 0.3f * bestPatternMatch;
            weightedSum = weightedSum * 0.7f + patternBlend;
        }
        
        predictedData[i] = std::clamp(weightedSum / (totalWeight + 0.001f), 0.0f, 1.0f);
    }
    
    // 3. Apply prediction to create new SensoryInput
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // 4. Update confidence based on prediction accuracy
    // (Would be updated later with actual observation)
    pImpl->confidence = std::min(1.0f, pImpl->confidence * 0.99f + 0.01f);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Enhanced prediction error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // 1. Calculate prediction error with multiple error metrics
        float simpleMSE = 0.0f;
        float maxError = 0.0f;
        float patternError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            simpleMSE += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
            
            // Pattern error for sequence-based prediction
            if (i > 0 && predData.size() > 1) {
                float patternTrend = predData[i] - predData[i - 1];
                float actualTrend = actualData[i] - actualData[i - 1];
                patternError += (patternTrend - actualTrend) * (patternTrend - actualTrend);
            }
        }
        
        // Combine error metrics
        pImpl->predictionError = (simpleMSE / predData.size()) * 0.6f + 
                                (maxError * 0.4f) * 0.4f + 
                                patternError * 0.05f;
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // 2. Update confidence based on error
        if (pImpl->predictionError < 0.01f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
        } else {
            pImpl->confidence = std::max(0.0f, pImpl->confidence - 0.1f);
        }
        
        // 3. Learn from prediction error
        learnFromError(predData, actualData, pImpl->predictionError);
    }
}

void PredictionSystem::learnFromError(const std::vector<float>& prediction, 
                                      const std::vector<float>& actual,
                                      float error) {
    // 1. Update prediction weights based on error
    for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
        // Hebbian-like learning: strengthen connections that lead to accurate predictions
        float predictionAccuracy = 1.0f - error;
        
        // Find corresponding input feature
        size_t inputIdx = std::min(i, prediction.size() - 1);
        float inputValue = prediction[inputIdx];
        
        // Update weight based on prediction accuracy
        pImpl->predictionWeights[i] += pImpl->learningRate * predictionAccuracy * inputValue;
        
        // Apply weight decay
        pImpl->predictionWeights[i] *= pImpl->predictionDecay;
    }
    
    // 2. Store successful patterns for pattern completion
    if (error < 0.05f) {
        // Successful prediction - store pattern for future use
        std::vector<float> pattern = prediction;
        if (pattern.size() > 16) {
            pattern.resize(16);  // Keep only first 16 features
        }
        
        float patternReward = 1.0f - error;
        
        // Add to learned patterns
        pImpl->learnedPatterns.push_back(pattern);
        pImpl->patternRewards.push_back(patternReward);
        
        // Keep only top patterns by reward
        std::vector<size_t> patternIndices(pImpl->learnedPatterns.size());
        for (size_t i = 0; i < patternIndices.size(); ++i) patternIndices[i] = i;
        
        std::sort(patternIndices.begin(), patternIndices.end(),
                  [&](size_t a, size_t b) { return pImpl->patternRewards[a] > pImpl->patternRewards[b]; });
        
        // Keep only top 8 patterns
        if (pImpl->learnedPatterns.size() > 8) {
            std::vector<std::vector<float>> newPatterns;
            std::vector<float> newRewards;
            for (size_t i = 0; i < std::min<size_t>(8, patternIndices.size()); ++i) {
                newPatterns.push_back(pImpl->learnedPatterns[patternIndices[i]]);
                newRewards.push_back(pImpl->patternRewards[patternIndices[i]]);
            }
            pImpl->learnedPatterns = std::move(newPatterns);
            pImpl->patternRewards = std::move(newRewards);
        }
    }
    
    // 3. Decay old patterns
    for (auto& reward : pImpl->patternRewards) {
        reward *= pImpl->predictionDecay;
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
    pImpl->predictionError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using neural patterns
    const auto& data = observation.getData();
    
    // 1. Use observation to refine prediction weights
    for (size_t i = 0; i < std::min(data.size(), pImpl->predictionWeights.size()); ++i) {
        pImpl->predictionWeights[i] = std::min(2.0f, pImpl->predictionWeights[i] + data[i] * 0.1f);
    }
    
    // 2. Store observation as a learned pattern
    std::vector<float> pattern = data;
    if (pattern.size() > 16) {
        pattern.resize(16);
    }
    
    pImpl->learnedPatterns.push_back(pattern);
    pImpl->patternRewards.push_back(1.0f);
    
    // Keep only top patterns
    if (pImpl->learnedPatterns.size() > 10) {
        std::vector<size_t> patternIndices(pImpl->learnedPatterns.size());
        for (size_t i = 0; i < patternIndices.size(); ++i) patternIndices[i] = i;
        
        std::sort(patternIndices.begin(), patternIndices.end(),
                  [&](size_t a, size_t b) { return pImpl->patternRewards[a] > pImpl->patternRewards[b]; });
        
        if (pImpl->learnedPatterns.size() > 10) {
            std::vector<std::vector<float>> newPatterns;
            std::vector<float> newRewards;
            for (size_t i = 0; i < std::min<size_t>(10, patternIndices.size()); ++i) {
                newPatterns.push_back(pImpl->learnedPatterns[patternIndices[i]]);
                newRewards.push_back(pImpl->patternRewards[patternIndices[i]]);
            }
            pImpl->learnedPatterns = std::move(newPatterns);
            pImpl->patternRewards = std::move(newRewards);
        }
    }
    
    NLM_LOG_INFO("PredictionSystem trained with new observation pattern");
}

} // namespace nlm
