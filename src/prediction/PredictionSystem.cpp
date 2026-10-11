#include "PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> patternHistory;  // Store neural patterns
    std::vector<float> predictionStrength;
    size_t predictionHorizon;
    bool isEnabled;
    
    // Simple forward model: action -> state transition
    std::vector<std::vector<float>> forwardModel;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionHorizon(5), isEnabled(true) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const std::vector<float>& currentPattern) {
    // REAL implementation using neural patterns
    if (!isEnabled_ || currentPattern.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Store current pattern for learning
    pImpl->patternHistory.push_back(currentPattern);
    if (pImpl->patternHistory.size() > 1000) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    
    // Simple forward prediction based on temporal patterns
    std::vector<float> predictedPattern = currentPattern;
    
    // Apply temporal dynamics (smoothing and trends)
    if (!pImpl->patternHistory.empty()) {
        // Average recent patterns for stability
        std::vector<float> avgPattern(pImpl->patternHistory[0].size(), 0.0f);
        size_t count = std::min<size_t>(10, pImpl->patternHistory.size());
        
        for (size_t i = 0; i < count; ++i) {
            size_t idx = pImpl->patternHistory.size() - count + i;
            for (size_t j = 0; j < avgPattern.size(); ++j) {
                avgPattern[j] += pImpl->patternHistory[idx][j];
            }
        }
        
        for (size_t j = 0; j < avgPattern.size(); ++j) {
            avgPattern[j] /= count;
        }
        
        // Blend current pattern with average for prediction
        for (size_t j = 0; j < predictedPattern.size() && j < avgPattern.size(); ++j) {
            predictedPattern[j] = predictedPattern[j] * 0.7f + avgPattern[j] * 0.3f;
        }
    }
    
    // Apply action-consequence model (simple temporal dynamics)
    std::transform(predictedPattern.begin(), predictedPattern.end(), predictedPattern.begin(),
                   [this](float val) { return std::clamp(val * 1.02f - 0.01f, 0.0f, 1.0f); });
    
    // Create sensory input from predicted pattern
    auto sensoryInput = std::make_unique<SensoryInput>();
    sensoryInput->setData(predictedPattern);
    
    // Update prediction strength based on pattern coherence
    float patternCoherence = 1.0f;
    if (pImpl->patternHistory.size() >= 2) {
        float totalDiff = 0.0f;
        for (size_t i = 1; i < pImpl->patternHistory.size(); ++i) {
            for (size_t j = 0; j < pImpl->patternHistory[i].size() && j < predictedPattern.size(); ++j) {
                totalDiff += std::abs(pImpl->patternHistory[i][j] - predictedPattern[j]);
            }
        }
        patternCoherence = 1.0f - std::min(1.0f, totalDiff / pImpl->patternHistory.size() / predictedPattern.size());
    }
    
    pImpl->predictionStrength.push_back(patternCoherence);
    if (pImpl->predictionStrength.size() > 100) {
        pImpl->predictionStrength.erase(pImpl->predictionStrength.begin());
    }
    
    return sensoryInput;
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

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    if (pImpl->errorHistory.empty()) return 0.5f;
    
    // Higher confidence with lower prediction error
    float avgError = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f) / pImpl->errorHistory.size();
    return std::max(0.1f, 1.0f - avgError);
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

float PredictionSystem::getPredictionStrength() const {
    if (pImpl->predictionStrength.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->predictionStrength.begin(), pImpl->predictionStrength.end(), 0.0f);
    return sum / pImpl->predictionStrength.size();
}

void PredictionSystem::trainFromPattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Store pattern for learning temporal structure
    pImpl->patternHistory.push_back(pattern);
    
    // Keep history bounded
    if (pImpl->patternHistory.size() > 1000) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    
    // Learn simple temporal dynamics
    if (pImpl->patternHistory.size() >= 2) {
        // Update forward model based on observed transitions
        size_t currentIdx = pImpl->patternHistory.size() - 1;
        size_t prevIdx = currentIdx - 1;
        
        if (pImpl->patternHistory[prevIdx].size() == pattern.size()) {
            std::vector<float> transition(pattern.size(), 0.0f);
            for (size_t i = 0; i < pattern.size(); ++i) {
                transition[i] = pattern[i] - pImpl->patternHistory[prevIdx][i];
            }
            
            pImpl->forwardModel.push_back(transition);
            if (pImpl->forwardModel.size() > 100) {
                pImpl->forwardModel.erase(pImpl->forwardModel.begin());
            }
        }
    }
}

void PredictionSystem::applyPrediction(TimestepDuration dt) {
    if (!isEnabled_ || pImpl->patternHistory.empty()) return;
    
    // Apply prediction effects on neural dynamics
    // Higher prediction strength encourages neural patterns to follow predicted trajectories
    
    float avgStrength = 1.0f;
    if (!pImpl->predictionStrength.empty()) {
        avgStrength = std::accumulate(pImpl->predictionStrength.begin(), pImpl->predictionStrength.end(), 0.0f) / pImpl->predictionStrength.size();
    }
    
    // This would be implemented in Brain.cpp with actual neural effects
    // For now, we just store the prediction strength
    if (avgStrength > 0.5f) {
        // Predictions are reliable - could enhance pattern maintenance
        // Could apply to working memory or attention systems
    }
}

void PredictionSystem::clear() {
    pImpl->errorHistory.clear();
    pImpl->patternHistory.clear();
    pImpl->predictionStrength.clear();
    pImpl->forwardModel.clear();
}

} // namespace nlm