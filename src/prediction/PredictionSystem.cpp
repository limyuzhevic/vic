#include "PredictionSystem.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Temporal sequence learning
    std::vector<SensoryInput> sequenceBuffer;
    std::vector<float> predictionWeights;
    std::unique_ptr<RandomGenerator> rng;
    
    // Store recent sensory patterns for learning
    static constexpr size_t MAX_SEQUENCE_LENGTH = 20;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        rng = std::make_unique<RandomGenerator>(42);
        predictionWeights.resize(MAX_SEQUENCE_LENGTH, 1.0f / MAX_SEQUENCE_LENGTH);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement actual prediction using stored sequences and neural dynamics
    // Predict next state based on recent sequence patterns
    
    auto prediction = currentState.clone();
    const auto& currentData = currentState.getData();
    
    // Simple predictive model: blend current state with historical patterns
    if (!pImpl->sequenceBuffer.empty() && currentData.size() > 0) {
        // Store current state in sequence buffer for learning
        pImpl->sequenceBuffer.push_back(currentState.clone());
        if (pImpl->sequenceBuffer.size() > pImpl->MAX_SEQUENCE_LENGTH) {
            pImpl->sequenceBuffer.erase(pImpl->sequenceBuffer.begin());
        }
        
        // Calculate prediction using weighted average of recent sequence patterns
        float predictionBias = 0.0f;
        for (size_t i = 0; i < std::min(pImpl->sequenceBuffer.size(), pImpl->MAX_SEQUENCE_LENGTH); ++i) {
            const auto& seqData = pImpl->sequenceBuffer[i].getData();
            if (!seqData.empty()) {
                for (size_t j = 0; j < currentData.size(); ++j) {
                    if (j < seqData.size()) {
                        predictionBias += (seqData[j] - currentData[j]) * pImpl->predictionWeights[i];
                    }
                }
            }
        }
        
        // Apply prediction bias to adjust confidence
        pImpl->confidence = std::max(0.1f, std::min(1.0f, 1.0f - std::abs(predictionBias)));
    }
    
    // For now, return current state with small random noise for exploration
    auto& predData = prediction->getData();
    for (float& val : predData) {
        val += (pImpl->rng->nextFloat() - 0.5f) * 0.1f;
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        
        // Update confidence based on prediction error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Store in error history with exponential moving average
        if (pImpl->errorHistory.empty()) {
            pImpl->errorHistory.push_back(pImpl->predictionError);
        } else {
            float ema = 0.9f * pImpl->errorHistory.back() + 0.1f * pImpl->predictionError;
            pImpl->errorHistory.push_back(ema);
        }
        
        // Limit error history size
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
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
    // Train prediction model using temporal sequence learning
    if (!observation.getData().empty()) {
        // Update prediction weights based on successful predictions
        float predictionError = pImpl->predictionError;
        
        // Adjust weights based on recent prediction error
        for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
            // Recent predictions should have higher influence
            float recentFactor = 1.0f - (i / static_cast<float>(pImpl->predictionWeights.size()));
            if (predictionError < 0.1f) {
                // Good prediction: reinforce weights
                pImpl->predictionWeights[i] *= (1.0f + recentFactor * 0.01f);
            } else {
                // Bad prediction: reduce influence of recent patterns
                pImpl->predictionWeights[i] *= (1.0f - recentFactor * 0.005f);
            }
            
            // Normalize weights to maintain sum = 1
            pImpl->predictionWeights[i] = std::max(0.0f, pImpl->predictionWeights[i]);
        }
        
        // Renormalize all weights
        float weightSum = 0.0f;
        for (float weight : pImpl->predictionWeights) {
            weightSum += weight;
        }
        if (weightSum > 0.0f) {
            for (float& weight : pImpl->predictionWeights) {
                weight /= weightSum;
            }
        }
    }
}

} // namespace nlm

} // namespace nlm
