#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Training state
    size_t trainingSteps;
    float patternLearningRate;
    float confidenceDecayRate;
    
    // Pattern storage for prediction
    std::vector<std::vector<float>> learnedPatterns;
    std::vector<float> patternStrengths;
    
    Impl() : predictionError(0.0f), confidence(0.5f), trainingSteps(0), 
             patternLearningRate(0.01f), confidenceDecayRate(0.001f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const class SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
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
    }
}

void PredictionSystem::train(const class SensoryInput& observation) {
    pImpl->trainingSteps++;
    
    // Extract sensory pattern from observation
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Learn pattern from observation
    // In a real implementation, this would extract patterns from neural activity
    // For now, we'll store the raw observation as a learned pattern
    if (pImpl->learnedPatterns.size() < 100) {  // Limit memory
        pImpl->learnedPatterns.push_back(data);
        pImpl->patternStrengths.push_back(1.0f);
    }
    
    // Update confidence based on prediction accuracy
    // This would normally track how well the prediction system matches reality
    pImpl->confidence = std::max(0.1f, pImpl->confidence - pImpl->confidenceDecayRate);
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
    pImpl->learnedPatterns.clear();
    pImpl->patternStrengths.clear();
}

}} // namespace nlm

