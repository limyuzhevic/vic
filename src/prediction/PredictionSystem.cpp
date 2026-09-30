#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Integrated neural prediction system
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    // Initialize the integrated neural prediction system
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction to generate predictions for next state
    // This enables real-time predictive coding
    
    if (pImpl->neuralPrediction) {
        // Get current sensory state
        const auto& currentData = currentState.getData();
        
        // Generate prediction for next timestep
        auto prediction = pImpl->neuralPrediction->generatePrediction(0);  // Using current step as 0
        
        // Convert prediction back to SensoryInput
        // In a full implementation, this would properly encode the prediction
        auto result = std::make_unique<SensoryInput>();
        
        // For now, just use the prediction as-is
        // In reality, this would be converted to proper sensory representation
        NLM_LOG_DEBUG("PredictionSystem: Generated prediction from NeuralPrediction");
        
        return result;
    }
    
    // Fallback to placeholder implementation
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update neural prediction based on actual observation
    // This computes prediction error and drives learning
    
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
        
        // Also update neural prediction if available
        if (pImpl->neuralPrediction) {
            pImpl->neuralPrediction->updateWithObservation(actualData, 0);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    // Integrate prediction error from neural prediction if available
    if (pImpl->neuralPrediction) {
        // Get prediction error from neural prediction system
        return pImpl->neuralPrediction->getPredictionError();
    }
    
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    // Calculate confidence based on prediction error consistency
    if (pImpl->errorHistory.empty()) {
        return 0.5f;
    }
    
    // Lower error history means higher confidence
    float maxError = *std::max_element(pImpl->errorHistory.begin(), pImpl->errorHistory.end());
    if (maxError > 0.0f) {
        float avgError = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f) / pImpl->errorHistory.size();
        pImpl->confidence = 1.0f - (avgError / maxError);
    }
    
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train the neural prediction system
    if (pImpl->neuralPrediction) {
        const auto& data = observation.getData();
        pImpl->neuralPrediction->recordSensoryState(data, 0);
    }
}

} // namespace nlm
