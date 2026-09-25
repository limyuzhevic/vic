#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<float> predictionBuffer;
    std::vector<float> errorBuffer;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using forward model integration
    // This function uses the brain's prediction system to forecast next sensory state
    
    // Get current sensory data
    const std::vector<float>& currentData = currentState.getData();
    
    if (currentData.empty()) {
        NLM_LOG_WARNING("Cannot predict from empty sensory data");
        return currentState.clone();
    }
    
    // Create prediction buffer
    pImpl->predictionBuffer = currentData;
    
    // Apply forward model transformations (simplified version)
    // In a real implementation, this would use the brain's prediction system
    // For now, implement a simple forward model that smooths and extrapolates
    
    // Apply temporal smoothing (first-order Markov model)
    std::vector<float> predictedData = currentData;
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Add small drift for temporal dynamics
        float drift = (i + 1) * 0.01f * currentData[i];
        predictedData[i] += drift;
        
        // Apply neural modulation factor
        float modulation = 0.5f + pImpl->confidence * 0.5f;
        predictedData[i] *= modulation;
    }
    
    // Clamp to valid range
    for (float& value : predictedData) {
        value = std::clamp(value, 0.0f, 1.0f);
    }
    
    // Create new observation with predicted data
    // For simplicity, create a generic SensoryInput
    // In a real implementation, this would create the appropriate sensory input type
    class PredictedSensoryInput : public SensoryInput {
        std::vector<float> predictedData_;
        
    public:
        explicit PredictedSensoryInput(const std::vector<float>& data) : predictedData_(data) {}
        
        const char* getType() const override {
            return "PredictedInput";
        }
        
        const std::vector<float>& getData() const override {
            return predictedData_;
        }
        
        size_t getDimensions() const override {
            return predictedData_.size();
        }
        
        std::unique_ptr<SensoryInput> clone() const override {
            return std::make_unique<PredictedSensoryInput>(predictedData_);
        }
    };
    
    NLM_LOG_DEBUG("Prediction system generated prediction with error history size: " + 
                 std::to_string(pImpl->errorHistory.size()));
    
    return std::make_unique<PredictedSensoryInput>(predictedData);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!predicted.getData().empty() && !actual.getData().empty()) {
        // Compute prediction error
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        // Only compare if same dimensionality
        if (predData.size() == actualData.size()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            
            pImpl->predictionError = sumError / predData.size();
            pImpl->errorHistory.push_back(pImpl->predictionError);
            pImpl->errorBuffer.push_back(pImpl->predictionError);
            
            // Update confidence based on prediction error
            // Lower error = higher confidence
            pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
            
            // Limit error history size
            if (pImpl->errorHistory.size() > 1000) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin(), 
                                        pImpl->errorHistory.begin() + 100);
            }
            if (pImpl->errorBuffer.size() > 100) {
                pImpl->errorBuffer.erase(pImpl->errorBuffer.begin(), 
                                       pImpl->errorBuffer.begin() + 50);
            }
            
            NLM_LOG_DEBUG("Updated predictions: error=" + std::to_string(pImpl->predictionError) + 
                         ", confidence=" + std::to_string(pImpl->confidence));
        } else {
            NLM_LOG_WARNING("Prediction update failed: dimension mismatch " + 
                           std::to_string(predData.size()) + " vs " + 
                           std::to_string(actualData.size()));
        }
    } else {
        NLM_LOG_WARNING("Cannot update predictions with empty data");
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
    pImpl->errorBuffer.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using neural substrate
    // This function would integrate with the brain's learning mechanisms
    
    const std::vector<float>& data = observation.getData();
    if (data.empty()) {
        NLM_LOG_WARNING("Cannot train prediction model with empty data");
        return;
    }
    
    // Store observation for future predictions
    pImpl->predictionBuffer = data;
    
    // In a real implementation, this would:
    // 1. Update the brain's prediction system
    // 2. Train forward model parameters
    // 3. Update synaptic weights based on prediction error
    // 4. Integrate with neuromodulatory systems
    
    NLM_LOG_DEBUG("Prediction model trained on observation data with " + 
                 std::to_string(data.size()) + " dimensions");
}

} // namespace nlm
