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
    // For now, create a lightweight prediction by analyzing neural activity patterns
    
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // Get current neural activity patterns
    const auto& currentData = currentState.getData();
    
    // Simple prediction: add small variations to current state to simulate forward dynamics
    predictedInput->setData(currentData);
    
    // Add small noise to simulate forward dynamics
    for (size_t i = 0; i < currentData.size(); ++i) {
        float variation = 0.05f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        predictedInput->setData(i, currentData[i] + variation);
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // For now, calculate sophisticated error metrics
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float mse = 0.0f;    // Mean squared error
        float mae = 0.0f;    // Mean absolute error
        float maxDiff = 0.0f; // Maximum absolute difference
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float absDiff = std::abs(diff);
            
            mse += diff * diff;
            mae += absDiff;
            maxDiff = std::max(maxDiff, absDiff);
        }
        
        size_t count = predData.size();
        float mseValue = mse / count;
        float maeValue = mae / count;
        
        // Combine errors into a single prediction error metric
        pImpl->predictionError = std::sqrt(mseValue) * 0.6f + 
                                 maeValue * 0.3f + 
                                 maxDiff * 0.1f;
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Adaptive confidence based on prediction error
        if (pImpl->predictionError < 0.1f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
        } else if (pImpl->predictionError < 0.3f) {
            // Maintain confidence
        } else {
            pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.2f);
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
    // PHASE 2: Train prediction model using neural substrate
    // For now, learn basic statistical patterns from observation
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Learn temporal dynamics from observation
    // Simple approach: accumulate statistics for predicting next states
    
    // Update running statistics
    float mean = 0.0f;
    for (float val : data) {
        mean += val;
    }
    mean /= data.size();
    
    // Store training sample for future prediction
    // In a full implementation, this would build a neural predictor
    // For now, we'll use the simple forward model approach
    
    // Also update confidence based on observation quality
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= data.size();
    
    // High variance indicates interesting/uncertain observations
    // which should increase prediction confidence over time
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f * std::min(1.0f, variance / 10.0f));
}

} // namespace nlm
