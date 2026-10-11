#include "PredictionSystem.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> learnedPatterns;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
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

void PredictionSystem::updatePredictionError(float error) {
    // Update prediction error directly
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Update confidence based on prediction error
    pImpl->confidence = std::max(0.1f, 1.0f - predictionError);
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

void PredictionSystem::train(const std::vector<float>& pattern) {
    // Train prediction model with pattern vector
    if (!pattern.empty()) {
        pImpl->learnedPatterns.push_back(pattern);
        
        // Simple averaging for learned pattern
        if (pImpl->learnedPatterns.size() == 1) {
            // First pattern - just store
        } else {
            // Average with previous patterns
            auto& lastPattern = pImpl->learnedPatterns.back();
            const auto& prevPattern = pImpl->learnedPatterns[pImpl->learnedPatterns.size() - 2];
            
            for (size_t i = 0; i < std::min(lastPattern.size(), prevPattern.size()); ++i) {
                lastPattern[i] = (lastPattern[i] + prevPattern[i]) * 0.5f;
            }
        }
        
        // Update confidence based on pattern diversity
        if (pImpl->learnedPatterns.size() > 1) {
            float diversity = 0.0f;
            for (size_t i = 0; i < pImpl->learnedPatterns.size() - 1; ++i) {
                float sim = computeSimilarity(pImpl->learnedPatterns[i], pImpl->learnedPatterns.back());
                diversity += (1.0f - sim);
            }
            pImpl->confidence = std::min(1.0f, diversity / (pImpl->learnedPatterns.size() - 1));
        }
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model with sensory input
    const auto& data = observation.getData();
    train(data);
}

float PredictionSystem::computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
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

} // namespace nlm
