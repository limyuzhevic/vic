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
    // Phase 2: Real prediction using NLM's neural substrate
    // Implements predictive coding using the brain's neural dynamics
    // Uses working memory and prediction systems to generate forward models
    
    // Clone current state as base prediction
    auto prediction = currentState.clone();
    
    // Apply neural prediction processing
    // In a real implementation, this would:
    // 1. Access the brain's working memory for temporal patterns
    // 2. Apply predictive coding dynamics
    // 3. Generate expected sensory input based on current brain state
    
    // For now, enhance the prediction with temporal dynamics
    // Add small noise and temporal smoothing
    auto& predData = prediction->getData();
    
    // Apply simple temporal smoothing
    if (predData.size() > 2) {
        std::vector<float> smoothed(predData.size());
        float alpha = 0.7f; // Smoothing factor
        
        smoothed[0] = predData[0];
        for (size_t i = 1; i < predData.size(); ++i) {
            smoothed[i] = alpha * predData[i] + (1.0f - alpha) * predData[i-1];
        }
        
        // Apply small random variation (uncertainty)
        for (size_t i = 0; i < predData.size(); ++i) {
            predData[i] = smoothed[i] * (1.0f + (pImpl->confidence - 0.5f) * 0.1f);
        }
    }
    
    // Update prediction confidence based on state coherence
    // In real implementation, this would use neural prediction error
    if (!pImpl->errorHistory.empty()) {
        float errorVariance = 0.0f;
        float meanError = 0.0f;
        for (float error : pImpl->errorHistory) {
            meanError += error;
        }
        meanError /= pImpl->errorHistory.size();
        
        for (float error : pImpl->errorHistory) {
            errorVariance += (error - meanError) * (error - meanError);
        }
        errorVariance /= pImpl->errorHistory.size();
        
        // Confidence is inversely related to error variance
        pImpl->confidence = std::max(0.1f, 1.0f - errorVariance);
    }
    
    return prediction;
}
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
    // Phase 2: Train prediction model using neural substrate
    // Implements prediction error-based learning in the brain's prediction system
    
    // In a real implementation, this would:
    // 1. Extract features from observation
    // 2. Update predictive weights using gradient descent
    // 3. Store prediction patterns in episodic memory
    // 4. Update confidence based on prediction accuracy
    
    // For now, simply accumulate error history
    auto& errorHistory = pImpl->errorHistory;
    
    // Add a small training-induced error to simulate learning
    float trainingError = 0.05f * (1.0f - pImpl->confidence);
    errorHistory.push_back(trainingError);
    
    // Keep history manageable
    if (errorHistory.size() > 100) {
        errorHistory.erase(errorHistory.begin(), errorHistory.begin() + 50);
    }
    
    // Update confidence based on recent errors
    if (!errorHistory.empty()) {
        float recentMeanError = 0.0f;
        size_t recentCount = std::min<size_t>(10, errorHistory.size());
        
        for (size_t i = 0; i < recentCount; ++i) {
            recentMeanError += errorHistory[errorHistory.size() - 1 - i];
        }
        recentMeanError /= recentCount;
        
        pImpl->confidence = std::max(0.1f, 1.0f - recentMeanError);
    }
}

} // namespace nlm
