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

} // namespace nlm

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float predictionGain;  // How much to weight predictions
    float learningRate;   // How quickly predictions adapt
    float predictionThreshold;  // Minimum error to trigger learning
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionGain(0.8f), 
             learningRate(0.01f), predictionThreshold(0.1f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO: Implement real neural prediction using brain's state
    // For now, create a simple prediction based on current sensory pattern
    // In a full implementation, this would use the brain's neural activity patterns
    // to predict future sensory states based on learned associations
    
    // Create a copy of current state as baseline prediction
    auto prediction = currentState.clone();
    
    // Apply some predictive dynamics based on prediction system parameters
    // This is a simplified version of predictive coding
    const auto& currentData = prediction->getData();
    if (!currentData.empty()) {
        // Smooth the prediction slightly to anticipate change
        // This simulates the brain expecting gradual changes
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Slight tendency toward zero (expectation of change)
            prediction->setData(i, currentData[i] * 0.9f + 0.05f);
        }
        
        // Increase prediction confidence based on sensory history
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error using more sophisticated metrics
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error as RMS difference
        float sumSquaredError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
            maxError = std::max(maxError, std::abs(error));
        }
        
        pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history bounded
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error magnitude
        // Low error = high confidence, high error = low confidence
        float errorScore = 1.0f - std::min(1.0f, pImpl->predictionError * 10.0f);
        pImpl->confidence = 0.7f * pImpl->confidence + 0.3f * errorScore;
        
        // Trigger learning if prediction error exceeds threshold
        if (pImpl->predictionError > pImpl->predictionThreshold) {
            // Prediction error is significant - should trigger learning processes
            // This would interface with other brain systems in a full implementation
        }
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
    // Update prediction model based on new observation
    // This would strengthen predictive connections in a full implementation
    
    // Increase prediction gain based on learning
    pImpl->predictionGain = std::min(1.0f, pImpl->predictionGain + pImpl->learningRate);
    
    // Adjust prediction threshold based on learning progress
    pImpl->predictionThreshold = std::max(0.01f, 0.1f - pImpl->learningRate * 10.0f);
    
    // In a full implementation, this would:
    // 1. Update neural predictions using backpropagation-through-time
    // 2. Strengthen predictive connections in associative memory
    // 3. Adjust motor predictions based on action outcomes
    // 4. Update conceptual predictions using concept formation
}

} // namespace nlm
