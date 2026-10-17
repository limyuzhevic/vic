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
    // Implement real prediction using neural substrate
    // For now, use current state as prediction
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation
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
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
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

void PredictionSystem::update(const SensoryInput& sensoryInput, float rewardSignal) {
    // Update prediction with new sensory input and reward signal
    // This is the main integration with the brain loop
    pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.1f);  // Decay confidence
    
    if (rewardSignal < 0.0f) {
        // Negative reward suggests prediction was wrong
        pImpl->predictionError = std::max(0.0f, pImpl->predictionError + 0.1f);
    }
    
    // Store current sensory pattern for learning
    pImpl->errorHistory.push_back(pImpl->predictionError);
}

} // namespace nlm
