#include "PredictionSystem.hpp"

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
    // Enhanced placeholder: Use prediction error and confidence to influence prediction
    // and integrate with brain's internal state
    
    auto prediction = currentState.clone();
    
    // Enhance prediction based on prediction error history
    float errorInfluence = 0.0f;
    if (!pImpl->errorHistory.empty()) {
        // Use recent error history to modify prediction
        size_t historySize = std::min(pImpl->errorHistory.size(), static_cast<size_t>(10));
        float avgError = 0.0f;
        for (size_t i = 0; i < historySize; ++i) {
            avgError += pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
        }
        avgError /= historySize;
        
        // Apply error influence to prediction data
        const auto& data = prediction->getData();
        std::vector<float> newData(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            // Apply error modulation - higher error leads to more prediction variance
            float errorFactor = std::min(avgError * 2.0f, 1.0f);  // Clamp to [0, 1]
            newData[i] = data[i] * (1.0f + errorFactor * 0.5f);
        }
        prediction->setData(newData);
        
        errorInfluence = avgError;
    }
    
    // Update confidence based on prediction error
    if (!pImpl->errorHistory.empty()) {
        // Decrease confidence with higher prediction errors
        float recentError = pImpl->errorHistory.back();
        pImpl->confidence = std::max(0.1f, pImpl->confidence - recentError * 0.1f);
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
}

} // namespace nlm
