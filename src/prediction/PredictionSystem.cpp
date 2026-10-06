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

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
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

// Update prediction system with current observation
void PredictionSystem::updateObservation(const std::vector<float>& observation) {
    // Update prediction model based on new sensory observation
    // For now, update confidence based on observation magnitude
    if (!observation.empty()) {
        // Calculate observation variability (simple measure of surprise)
        float sum = 0.0f;
        float mean = 0.0f;
        for (float val : observation) {
            sum += val;
        }
        mean = sum / observation.size();
        
        float variance = 0.0f;
        for (float val : observation) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= observation.size();
        
        // Higher variance increases uncertainty, decreases confidence
        pImpl->confidence = std::max(0.1f, 1.0f - variance * 10.0f);
        
        // Store observation for training
        pImpl->errorHistory.push_back(mean);
    }
}

// Get predicted state as activity pattern
std::vector<float> PredictionSystem::predictNextStatePattern(const std::vector<float>& currentState) {
    // For now, return a smoothed version of current state
    // TODO PHASE 2: Implement real neural prediction
    std::vector<float> predictedState = currentState;
    
    // Apply some smoothing/filtering to make it more plausible
    if (!predictedState.empty()) {
        // Simple moving average
        float sum = 0.0f;
        for (float val : predictedState) {
            sum += val;
        }
        float mean = sum / predictedState.size();
        
        // Add some noise based on confidence
        float noiseLevel = 1.0f - pImpl->confidence;
        
        // Return smoothed prediction
        for (float& val : predictedState) {
            val = mean + (val - mean) * 0.7f + ((rand() % 100) / 100.0f - 0.5f) * noiseLevel;
            val = std::max(0.0f, std::min(1.0f, val));  // Clamp to [0, 1]
        }
    }
    
    return predictedState;
}

} // namespace nlm
