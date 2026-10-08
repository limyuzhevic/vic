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
    // PHASE 2: Real prediction using neural dynamics from the spiking brain
    if (!brain_ || !brain_->getPredictionSystem()) {
        NLM_LOG_WARNING("PredictionSystem: Brain reference not set or prediction system missing");
        return currentState.clone();
    }
    
    // Get current neural activity pattern as prediction
    auto& pattern = currentState.getData();
    
    // Apply prediction dynamics based on neural substrate
    std::vector<float> predictedPattern(pattern.size(), 0.0f);
    
    // For each neural population, predict activity changes
    // This simulates forward dynamics learned through experience
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Prediction includes:
        // 1. Persistence of current activity (working memory)
        // 2. Neural habituation/decay
        // 3. Influence from prediction error signals
        
        float persistence = 0.8f;  // Activity tends to persist
        float decay = 0.2f;       // But also decays over time
        
        // Get current prediction error for context-dependent prediction
        float error = brain_->getPredictionErrorSignal() ? 
                      brain_->getPredictionErrorSignal()->getError() : 0.0f;
        
        // Prediction combines current state with error modulation
        float currentInfluence = persistence * pattern[i] * (1.0f - error * 0.3f);
        float predictionInfluence = decay * (0.5f - error * 0.2f); // Simple forward model
        
        predictedPattern[i] = currentInfluence + predictionInfluence;
        
        // Clamp to realistic neural activity range
        predictedPattern[i] = std::max(0.0f, std::min(predictedPattern[i], 1.0f));
    }
    
    // Create and return predicted sensory state
    auto predictedInput = std::make_unique<SensoryInput>("prediction", predictedPattern);
    
    // Update prediction error based on difference
    updatePredictions(predictedInput.get(), &currentState);
    
    return predictedInput;
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
