#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Integrated neural prediction system
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 6: Implement real prediction using neural substrate
    // Get current sensory state data
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Use neural prediction to generate next state
    SimulationStep currentStep = pImpl->neuralPrediction ? 
        (pImpl->neuralPrediction->getErrorHistory().empty() ? 0 : 
         pImpl->neuralPrediction->getErrorHistory().size() - 1) : 0;
    
    // Generate prediction based on learned sequences
    std::vector<float> predictedData;
    if (pImpl->neuralPrediction) {
        predictedData = pImpl->neuralPrediction->generatePrediction(currentStep);
    }
    
    if (predictedData.empty()) {
        // No prediction available, return current state
        return currentState.clone();
    }
    
    // Create new sensory input with predicted data
    // Preserve the same type and other properties
    auto predictedInput = currentState.clone();
    predictedInput.setData(predictedData);
    
    // Update prediction error
    pImpl->predictionError = currentData.size() > 0 ? 
        std::sqrt(std::accumulate(currentData.begin(), currentData.end(), 0.0f, 
                                [predictedData](float sum, float val, size_t i) {
                                    if (i < predictedData.size()) {
                                        return sum + (val - predictedData[i]) * (val - predictedData[i]);
                                    }
                                    return sum;
                                })) / currentData.size() : 0.0f;
    
    // Update confidence based on prediction stability
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    NLM_LOG_DEBUG("Prediction generated: error=" + std::to_string(pImpl->predictionError) + 
                 ", confidence=" + std::to_string(pImpl->confidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update neural prediction with actual observation
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (pImpl->neuralPrediction && !actualData.empty()) {
        // Record actual state in neural prediction system
        // Convert actual sensory input to vector format
        std::vector<float> actualVector(actualData.begin(), actualData.end());
        
        SimulationStep currentStep = pImpl->neuralPrediction->getErrorHistory().empty() ? 0 : 
            pImpl->neuralPrediction->getErrorHistory().size() - 1;
            
        pImpl->neuralPrediction->recordSensoryState(actualVector, currentStep);
        
        // Update prediction error based on actual vs predicted
        if (predictedData.size() == actualData.size() && !predictedData.empty()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predictedData.size(); ++i) {
                float diff = predictedData[i] - actualData[i];
                sumError += diff * diff;
            }
            pImpl->predictionError = sumError / predictedData.size();
            pImpl->errorHistory.push_back(pImpl->predictionError);
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
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train neural prediction system with new observation
    if (pImpl->neuralPrediction) {
        const auto& data = observation.getData();
        if (!data.empty()) {
            std::vector<float> observationVector(data.begin(), data.end());
            pImpl->neuralPrediction->recordSensoryState(observationVector, 
                pImpl->neuralPrediction->getErrorHistory().size());
        }
    }
}

} // namespace nlm
