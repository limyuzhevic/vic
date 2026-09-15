#include "PredictionSystem.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
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

void PredictionSystem::trackPredictionError(float error) {
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Update confidence based on error consistency
    if (!pImpl->errorHistory.empty()) {
        float maxHistory = static_cast<float>(pImpl->errorHistory.size());
        size_t recentSize = std::min<size_t>(10, pImpl->errorHistory.size());
        if (recentSize > 1) {
            // Low error variance = high confidence
            float sum = 0.0f;
            float mean = 0.0f;
            
            // Calculate mean of recent errors
            for (size_t i = pImpl->errorHistory.size() - recentSize; 
                 i < pImpl->errorHistory.size(); ++i) {
                sum += pImpl->errorHistory[i];
            }
            mean = sum / static_cast<float>(recentSize);
            
            // Calculate variance
            float variance = 0.0f;
            for (size_t i = pImpl->errorHistory.size() - recentSize; 
                 i < pImpl->errorHistory.size(); ++i) {
                float diff = pImpl->errorHistory[i] - mean;
                variance += diff * diff;
            }
            variance /= static_cast<float>(recentSize);
            
            // Confidence inversely related to variance
            pImpl->confidence = std::max(0.0f, 1.0f - variance * 10.0f);
        }
    }
}

void PredictionSystem::adjustExploration(float noveltyLevel) {
    // Exploration increases with prediction error and novelty
    pImpl->confidence = std::max(0.0f, pImpl->confidence - noveltyLevel * 0.1f);
    
    // Ensure confidence doesn't go below minimum
    if (pImpl->confidence < 0.1f) {
        pImpl->confidence = 0.1f;
    }
}

float PredictionSystem::getExplorationLevel() const {
    return 1.0f - pImpl->confidence;
}

void PredictionSystem::updateExploration(TimestepDuration dt) {
    // Exploration naturally decays over time when not actively used
    if (pImpl->confidence < 0.5f) {
        // Increase exploration when confidence is low
        pImpl->confidence = std::min(1.0f, pImpl->confidence + dt * 0.05f);
    }
}

} // namespace nlm
