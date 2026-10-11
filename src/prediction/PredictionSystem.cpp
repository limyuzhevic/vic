#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<float> predictiveWeights;
    std::vector<float> predictiveBias;
    std::vector<float> predictionBuffer;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        // Initialize with reasonable size for prediction system
        predictiveWeights.resize(256, 0.0f);
        predictiveBias.resize(256, 0.0f);
        predictionBuffer.resize(256, 0.0f);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 2: Implement real neural prediction using brain substrate
    // For now, implement a placeholder that:
    // 1. Extracts neural features from current sensory input
    // 2. Applies predictive weights to generate predictions
    // 3. Adds noise to create variability
    // 4. Returns new sensory input based on predictions
    
    NLM_LOG_INFO("Prediction system: Generating next state prediction");
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Create prediction based on neural dynamics
    // Use current state as input, apply weights, add some noise
    std::unique_ptr<SensoryInput> prediction = currentState.clone();
    auto& predictionData = prediction->getData();
    
    // Apply predictive dynamics (placeholder implementation)
    // In real Phase 2, this would use brain neural populations
    for (size_t i = 0; i < std::min(predictionData.size(), pImpl->predictionBuffer.size()); ++i) {
        // Predictive dynamics: current state + weighted transformation
        float prediction = predictionData[i] * 0.7f + pImpl->predictiveBias[i] * 0.3f;
        
        // Add noise to create prediction variability
        float noise = (pImpl->confidence - 0.5f) * 0.1f;  // Noise based on confidence
        float randomComponent = noise * 2.0f - 1.0f;  // [-1, 1] range
        
        predictionData[i] = prediction + randomComponent;
        
        // Clamp to reasonable range
        predictionData[i] = std::clamp(predictionData[i], -1.0f, 1.0f);
    }
    
    // Update prediction buffer
    pImpl->predictionBuffer = predictionData;
    
    NLM_LOG_INFO("Prediction system: Generated prediction with confidence " + 
                 std::to_string(pImpl->confidence));
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 2: Implement real prediction error computation
    // Calculate prediction error as the difference between
    // expected and actual sensory input
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
        }
        
        pImpl->predictionError = sumSquaredError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        NLM_LOG_INFO("Prediction system: Updated - error=" + std::to_string(pImpl->predictionError) + 
                     ", confidence=" + std::to_string(pImpl->confidence));
    } else {
        NLM_LOG_WARNING("Prediction system: Size mismatch in prediction update");
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // PHASE 2: Train prediction model using neural plasticity
    // In real Phase 2, this would use brain STDP and Hebbian learning
    
    const auto& obsData = observation.getData();
    
    // Update predictive weights based on observation
    // This is a placeholder - real implementation would use brain neural mechanisms
    for (size_t i = 0; i < std::min(obsData.size(), pImpl->predictiveWeights.size()); ++i) {
        // Hebbian-like weight update: strengthen for frequent observations
        float weightUpdate = obsData[i] * 0.01f;  // Simple learning rule
        pImpl->predictiveWeights[i] += weightUpdate;
        
        // Clamp weights to reasonable range
        pImpl->predictiveWeights[i] = std::clamp(pImpl->predictiveWeights[i], -1.0f, 1.0f);
    }
    
    // Update bias terms
    for (size_t i = 0; i < std::min(obsData.size(), pImpl->predictiveBias.size()); ++i) {
        pImpl->predictiveBias[i] = obsData[i] * 0.1f;
    }
    
    NLM_LOG_INFO("Prediction system: Trained on observation " + 
                 std::to_string(obsData.size()) + " elements");
}

} // namespace nlm
