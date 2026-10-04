#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float trainingAccumulation;
    std::vector<float> predictionPattern;
    std::vector<float> actualPattern;
    
    Impl() : predictionError(0.0f), confidence(0.5f), trainingAccumulation(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, implement a simple placeholder prediction that:
    // 1. Returns a slightly modified version of current state
    // 2. Maintains prediction error and confidence
    // 3. Returns the predicted sensory state
    
    // Store current pattern for comparison
    pImpl->predictionPattern = currentState.getData();
    
    // Create predicted state by applying small transformations
    // This simulates the brain predicting what will happen next
    std::unique_ptr<SensoryInput> predictedState = currentState.clone();
    auto& predictedData = predictedState->getData();
    
    if (!predictedData.empty()) {
        // Add some prediction noise (simulating uncertainty)
        for (size_t i = 0; i < predictedData.size(); ++i) {
            // Add small random variation based on confidence
            float noise = (1.0f - pImpl->confidence) * 0.1f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
            predictedData[i] += noise;
            
            // Clamp to reasonable range
            predictedData[i] = std::max(-1.0f, std::min(1.0f, predictedData[i]));
        }
        
        // Increase confidence slightly for consistent patterns
        if (pImpl->errorHistory.size() > 5) {
            float recentErrorAvg = 0.0f;
            size_t start = std::max(0, static_cast<int>(pImpl->errorHistory.size()) - 5);
            for (size_t i = start; i < pImpl->errorHistory.size(); ++i) {
                recentErrorAvg += pImpl->errorHistory[i];
            }
            recentErrorAvg /= (pImpl->errorHistory.size() - start);
            pImpl->confidence = std::max(0.1f, pImpl->confidence - recentErrorAvg * 0.05f);
        }
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // Update prediction error and confidence based on actual observation
    
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
        
        // Store actual pattern for training
        pImpl->actualPattern = actualData;
        
        // Reduce confidence based on error
        pImpl->confidence = std::max(0.01f, pImpl->confidence - pImpl->predictionError * 0.5f);
        
        // Accumulate training data for learning
        pImpl->trainingAccumulation += pImpl->predictionError;
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
    pImpl->predictionPattern.clear();
    pImpl->actualPattern.clear();
    pImpl->trainingAccumulation = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, implement simple pattern learning that:
    // 1. Accumulates prediction errors
    // 2. Adjusts internal patterns based on actual observations
    // 3. Improves prediction accuracy over time
    
    const auto& obsData = observation.getData();
    
    if (pImpl->predictionPattern.empty()) {
        pImpl->predictionPattern = obsData;
        NLM_LOG_INFO("PredictionSystem training initialized with new pattern");
    } else {
        // Simple Hebbian-like learning: strengthen connections between
        // predicted and actual patterns when prediction error is high
        float patternSimilarity = 0.0f;
        float weight = 0.0f;
        
        if (obsData.size() == pImpl->predictionPattern.size() && !obsData.empty()) {
            for (size_t i = 0; i < obsData.size(); ++i) {
                patternSimilarity += obsData[i] * pImpl->predictionPattern[i];
            }
            patternSimilarity /= obsData.size();
            
            // Update internal prediction pattern based on actual observation
            // More different patterns have stronger learning effect
            float learningRate = pImpl->predictionError * 0.01f;
            for (size_t i = 0; i < obsData.size() && i < pImpl->predictionPattern.size(); ++i) {
                // Move prediction pattern toward actual observation
                float delta = (obsData[i] - pImpl->predictionPattern[i]) * learningRate;
                pImpl->predictionPattern[i] += delta;
                weight += delta * delta;
            }
            
            // Adjust confidence based on learning weight
            pImpl->confidence = std::min(1.0f, pImpl->confidence + weight * 0.1f);
            
            NLM_LOG_INFO("PredictionSystem training: error=" + std::to_string(pImpl->predictionError) + 
                        ", weight=" + std::to_string(weight) + ", confidence=" + std::to_string(pImpl->confidence));
        }
    }
}

} // namespace nlm
