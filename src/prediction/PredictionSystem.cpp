#include "PredictionSystem.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    NeuralEpisodicMemory* episodicMemory;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float lastPredictionScore;
    
    Impl() : episodicMemory(nullptr), predictionError(0.0f), confidence(0.5f), lastPredictionScore(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(NeuralEpisodicMemory* episodicMemory) {
    pImpl->episodicMemory = episodicMemory;
    NLM_LOG_INFO("PredictionSystem initialized with episodic memory integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 2: Real prediction using NLM's neural substrate with episodic memory
    const auto& currentData = currentState.getData();
    
    // Get relevant past experiences from episodic memory
    auto similarEpisodes = retrieveSimilarPredictions(currentState);
    
    std::unique_ptr<SensoryInput> prediction = std::make_unique<SensoryInput>();
    auto& predictedData = prediction->getData();
    
    // If we have relevant episodes, use their patterns to inform prediction
    if (!similarEpisodes.empty()) {
        // Average the sensory states from similar episodes
        size_t patternSize = currentData.size();
        if (!patternSize) {
            predictedData = currentData; // Fallback
            return prediction;
        }
        
        std::vector<float> patternSum(patternSize, 0.0f);
        size_t validEpisodes = 0;
        
        for (const auto* episode : similarEpisodes) {
            if (episode->sensoryState.size() == patternSize) {
                for (size_t i = 0; i < patternSize; ++i) {
                    patternSum[i] += episode->sensoryState[i];
                }
                validEpisodes++;
            }
        }
        
        if (validEpisodes > 0) {
            // Compute weighted average - more recent/higher reward episodes get more weight
            predictedData.resize(patternSize);
            for (size_t i = 0; i < patternSize; ++i) {
                float weightedSum = 0.0f;
                float weightSum = 0.0f;
                
                for (const auto* episode : similarEpisodes) {
                    if (episode->sensoryState.size() == patternSize) {
                        float weight = episode->reward > 0 ? episode->reward : 0.1f;
                        weight *= (1000.0f - episode->age) / 1000.0f; // Recency bias
                        weightedSum += episode->sensoryState[i] * weight;
                        weightSum += weight;
                    }
                }
                
                if (weightSum > 0.0f) {
                    predictedData[i] = weightedSum / weightSum;
                } else {
                    predictedData[i] = currentData[i];
                }
            }
            
            // Compute confidence based on number and similarity of episodes
            pImpl->confidence = std::min(1.0f, static_cast<float>(validEpisodes) * 0.3f);
            if (similarEpisodes.size() > 3) {
                pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.2f);
            }
        } else {
            predictedData = currentData;
            pImpl->confidence = 0.3f;
        }
        
        // Apply some temporal smoothing - blend with current state
        for (size_t i = 0; i < patternSize; ++i) {
            predictedData[i] = predictedData[i] * 0.7f + currentData[i] * 0.3f;
        }
        
        pImpl->lastPredictionScore = pImpl->confidence;
    } else {
        // No similar episodes, fall back to current state with low confidence
        predictedData = currentData;
        pImpl->confidence = 0.3f;
        pImpl->lastPredictionScore = 0.3f;
    }
    
    return prediction;
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
        
        // Update confidence based on prediction error
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
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
    // TODO PHASE 2: Train prediction model
    // Placeholder - in a full implementation, this would train neural networks
    // to better predict based on episodic memory patterns
    NLM_LOG_INFO("PredictionSystem::train called - placeholder implementation");
}

void PredictionSystem::storePrediction(const SensoryInput& prediction, const SensoryInput& actual, float error, float confidence) {
    if (!pImpl->episodicMemory) return;
    
    // Create a simplified episodic memory item for the prediction outcome
    EpisodicMemoryItem item;
    item.timestamp = 0; // Would need actual step number
    item.sensoryState = prediction.getData();
    item.resultingSensoryState = actual.getData();
    item.reward = std::max(0.0f, 1.0f - error); // Convert error to reward
    item.novelty = error; // Higher error = higher novelty
    item.age = 0;
    
    pImpl->episodicMemory->storeEpisode(item);
    NLM_LOG_INFO("PredictionSystem stored prediction outcome in episodic memory");
}

std::vector<const EpisodicMemoryItem*> PredictionSystem::retrieveSimilarPredictions(const SensoryInput& currentState) const {
    if (!pImpl->episodicMemory) return {};
    
    const auto& currentData = currentState.getData();
    return pImpl->episodicMemory->retrieveSimilar(currentData, 5);
}

void PredictionSystem::updateEpisodicMemoryBasedOnPrediction(const SensoryInput& currentState, const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->episodicMemory) return;
    
    // Compute prediction error
    float error = 0.0f;
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        error = sumError / predData.size();
    }
    
    // Update episodic memory with prediction outcome
    storePrediction(predicted, actual, error, pImpl->confidence);
    
    NLM_LOG_INFO("PredictionSystem updated episodic memory with prediction outcome");
}

} // namespace nlm
