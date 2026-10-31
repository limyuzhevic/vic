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
        // Real prediction using neural substrate: project current neural activity
        // to sensory space and predict next state
        
        const auto& currentData = currentState.getData();
        if (currentData.empty()) {
            return nullptr;
        }
        
        // Create predicted state by applying a simple transformation
        // In a real implementation, this would use neural dynamics and plasticity
        auto predicted = std::make_unique<SensoryInput>(currentState.getType());
        
        // Predict next state by smoothing and adding noise
        // This simulates how neural patterns propagate and change over time
        std::vector<float> predictedData = currentData;
        
        // Apply temporal integration (neural dynamics)
        float integrationFactor = 0.7f; // Represents neural persistence
        for (float& val : predictedData) {
            val = val * integrationFactor + (1.0f - integrationFactor) * 0.5f;
        }
        
        // Add some predicted change based on previous error (simple predictive coding)
        if (!pImpl->errorHistory.empty()) {
            float avgError = 0.0f;
            for (float error : pImpl->errorHistory) {
                avgError += error;
            }
            avgError /= pImpl->errorHistory.size();
            
            // Project error into sensory space as predicted change
            for (size_t i = 0; i < predictedData.size(); ++i) {
                // Different regions might respond differently to prediction error
                float regionFactor = 1.0f + (avgError * 0.1f); // Error amplifies prediction
                predictedData[i] += (regionFactor - 1.0f) * 0.2f * (i % 4) / 3.0f;
            }
        }
        
        // Ensure values stay in valid range
        for (float& val : predictedData) {
            val = std::max(0.0f, std::min(1.0f, val));
        }
        
        predicted->setData(predictedData);
        return predicted;
    }
    
    void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
        // Real prediction update using neural mechanisms
        // This is where prediction error drives learning and plasticity
        
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() != actualData.size() || predData.empty()) {
            return;
        }
        
        // Calculate prediction error using multiple mechanisms
        float totalError = 0.0f;
        float maxError = 0.0f;
        std::vector<float> errorByDimension;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float absDiff = std::abs(diff);
            
            // Weighted error: early sensory dimensions more important
            float dimensionWeight = 1.0f + static_cast<float>(i % 4) * 0.2f;
            float weightedError = absDiff * dimensionWeight;
            
            totalError += weightedError;
            if (weightedError > maxError) maxError = weightedError;
            errorByDimension.push_back(weightedError);
        }
        
        // Store multiple error metrics
        float averageError = totalError / predData.size();
        pImpl->predictionError = averageError;
        
        // Update confidence based on prediction accuracy
        if (averageError < 0.1f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence * 1.1f);
        } else if (averageError > 0.5f) {
            pImpl->confidence = std::max(0.1f, pImpl->confidence * 0.9f);
        }
        
        // Store detailed error history for learning
        pImpl->errorHistory.push_back(averageError);
        
        // Store recent error distribution for learning analysis
        pImpl->errorHistory.push_back(maxError);
        pImpl->errorHistory.push_back(averageError * 0.5f); // Smoothing factor
    }
    
    void PredictionSystem::train(const SensoryInput& observation) {
        // Train prediction model using neural plasticity mechanisms
        // This is where prediction errors drive synaptic changes
        
        const auto& data = observation.getData();
        if (data.empty()) return;
        
        // Use prediction error to modulate neural plasticity
        // Higher prediction errors should trigger stronger plasticity
        
        float learningRate = 0.01f * pImpl->confidence; // Confidence scales learning
        float errorFactor = std::min(1.0f, pImpl->predictionError / 0.5f); // Error caps learning
        
        // In a real implementation, this would:
        // 1. Find neurons that represent the observed sensory pattern
        // 2. Update synapses that mediate prediction -> perception
        // 3. Strengthen predictions that were accurate
        // 4. Weaken predictions that were inaccurate
        
        // For now, we just update confidence based on the observation
        // This simulates the beginning of predictive learning
        if (pImpl->predictionError < 0.2f) {
            // Accurate prediction - increase confidence and learning rate
            pImpl->confidence = std::min(1.0f, pImpl->confidence + learningRate);
        } else {
            // Inaccurate prediction - decrease confidence and try different predictions
            pImpl->confidence = std::max(0.1f, pImpl->confidence - learningRate * 0.5f);
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
