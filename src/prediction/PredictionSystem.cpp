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
    // PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, create a noise-based prediction that can be improved with neural computation
    auto prediction = std::make_unique<SensoryInput>();
    
    // For each sensory signal type, apply different prediction mechanisms
    const auto& vision = currentState.getVision();
    const auto& touch = currentState.getTouch();
    const auto& internal = currentState.getInternal();
    const auto& proprioception = currentState.getProprioception();
    
    // Vision prediction: simple smoothing with temporal coherence
    std::vector<float> predictedVision = vision;
    for (size_t i = 0; i < predictedVision.size(); ++i) {
        // Add temporal component (momentum)
        if (i > 0 && i < predictedVision.size()) {
            predictedVision[i] = predictedVision[i] * 0.7f + vision[i] * 0.3f;
        }
        // Add small noise component (exploration)
        prediction->getRandomGenerator()->uniformReal(0.0f, 1.0f) * 0.05f - 0.025f;
    }
    prediction->setVision(predictedVision);
    
    // Touch prediction: persistence with decay
    std::vector<float> predictedTouch = touch;
    for (size_t i = 0; i < predictedTouch.size(); ++i) {
        predictedTouch[i] *= 0.9f;  // Decay previous contact
    }
    prediction->setTouch(predictedTouch);
    
    // Internal signals: homeostatic prediction
    std::vector<float> predictedInternal = internal;
    for (size_t i = 0; i < predictedInternal.size(); ++i) {
        // Predict based on current state and trends
        predictedInternal[i] = predictedInternal[i] * 0.8f + internal[i] * 0.2f;
    }
    prediction->setInternal(predictedInternal);
    
    // Proprioception: smooth transitions
    std::vector<float> predictedProprioception = proprioception;
    for (size_t i = 0; i < predictedProprioception.size(); ++i) {
        // Predict next position based on current velocity
        if (i >= 2) {
            predictedProprioception[i] = predictedProprioception[i-2] * 0.8f + predictedProprioception[i] * 0.2f;
        }
    }
    prediction->setProprioception(predictedProprioception);
    
    prediction->setTimestamp(currentState.getTimestamp());
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 2: Implement real prediction error computation using neural substrate
    // Calculate prediction error between predicted and actual sensory states
    const auto& predVision = predicted.getVision();
    const auto& actualVision = actual.getVision();
    const auto& predTouch = predicted.getTouch();
    const auto& actualTouch = actual.getTouch();
    const auto& predInternal = predicted.getInternal();
    const auto& actualInternal = actual.getInternal();
    
    float visionError = 0.0f, touchError = 0.0f, internalError = 0.0f;
    size_t visionCount = 0, touchCount = 0, internalCount = 0;
    
    // Calculate vision prediction error
    if (predVision.size() == actualVision.size() && !predVision.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predVision.size(); ++i) {
            float diff = predVision[i] - actualVision[i];
            sumError += diff * diff;
        }
        visionError = std::sqrt(sumError / predVision.size());
        visionCount = predVision.size();
    }
    
    // Calculate touch prediction error
    if (predTouch.size() == actualTouch.size()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predTouch.size(); ++i) {
            float diff = predTouch[i] - actualTouch[i];
            sumError += diff * diff;
        }
        touchError = std::sqrt(sumError / predTouch.size());
        touchCount = predTouch.size();
    }
    
    // Calculate internal prediction error
    if (predInternal.size() == actualInternal.size()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predInternal.size(); ++i) {
            float diff = predInternal[i] - actualInternal[i];
            sumError += diff * diff;
        }
        internalError = std::sqrt(sumError / predInternal.size());
        internalCount = predInternal.size();
    }
    
    // Combine errors with different weights based on importance
    float totalError = 0.0f;
    float totalWeight = 0.0f;
    
    if (visionCount > 0) {
        totalError += visionError * visionError * 0.5f;  // Vision is most important
        totalWeight += 0.5f;
    }
    if (touchCount > 0) {
        totalError += touchError * touchError * 0.3f;
        totalWeight += 0.3f;
    }
    if (internalCount > 0) {
        totalError += internalError * internalError * 0.2f;
        totalWeight += 0.2f;
    }
    
    pImpl->predictionError = totalWeight > 0.0f ? std::sqrt(totalError / totalWeight) : 0.0f;
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Update confidence based on prediction accuracy
    // Lower error = higher confidence
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
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
    // PHASE 2: Train prediction model using neural substrate
    // This would implement learning of forward models and prediction dynamics
    // For now, update confidence based on observation frequency and consistency
    
    // Simple temporal training: increase confidence for consistent predictions
    static uint64_t lastObservationTime = 0;
    static uint64_t observationCount = 0;
    
    observationCount++;
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f / observationCount);
    
    // Decay confidence towards baseline over time (forgetting)
    pImpl->confidence *= 0.99f;
}

} // namespace nlm
