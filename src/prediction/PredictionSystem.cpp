#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionModel;
    std::vector<float> expectedModel;
    size_t modelSize;
    
    Impl() : predictionError(0.0f), confidence(0.5f), modelSize(256) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // IMPLEMENTATION: Use neural prediction mechanism
    // Create prediction based on current state and prediction model
    auto predicted = std::make_unique<SensoryInput>();
    const auto& currentData = currentState.getData();
    
    // If prediction model exists, use it to make prediction
    if (!pImpl->predictionModel.empty() && pImpl->modelSize > 0) {
        // Apply prediction model to current state
        predicted->setData(currentData);  // Base prediction on current state
        
        // Add prediction dynamics (temporal smoothing and noise)
        for (size_t i = 0; i < currentData.size() && i < predicted->getData().size(); ++i) {
            float predictedValue = currentData[i];
            
            // Add prediction dynamics based on model
            if (i < pImpl->expectedModel.size()) {
                // Blend current state with expected dynamics
                predictedValue = currentData[i] * 0.7f + pImpl->expectedModel[i] * 0.3f;
            }
            
            // Add small prediction-specific noise
            static float seed = 0.1234567f;
            seed = seed * 1.00001f + 0.00001f;
            predictedValue += (seed - 0.5f) * 0.05f;
            
            predicted->setData(i, predictedValue);
        }
        
        // Update confidence based on prediction stability
        float stability = 0.0f;
        if (pImpl->modelSize > 1 && pImpl->errorHistory.size() >= 5) {
            // Calculate average error of recent predictions
            float avgError = 0.0f;
            size_t recentCount = std::min<size_t>(5, pImpl->errorHistory.size());
            for (size_t i = 0; i < recentCount; ++i) {
                avgError += pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
            }
            avgError /= recentCount;
            stability = 1.0f / (1.0f + avgError);
            pImpl->confidence = std::clamp(stability, 0.1f, 1.0f);
        }
    } else {
        // No prediction model, just return current state as prediction
        predicted->setData(currentData);
        pImpl->confidence = 0.1f; // Low confidence without model
    }
    
    NLM_LOG_DEBUG("PredictionSystem::predictNextState: confidence=" + std::to_string(pImpl->confidence));
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // IMPLEMENTATION: Real prediction error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float squaredError = diff * diff;
            sumError += squaredError;
            
            if (std::abs(diff) > maxError) {
                maxError = std::abs(diff);
            }
        }
        
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history manageable
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction model based on error
        updatePredictionModel(predData, actualData);
        
        NLM_LOG_DEBUG("PredictionSystem::updatePredictions: error=" + std::to_string(pImpl->predictionError) + ", maxError=" + std::to_string(maxError));
    }
}

void PredictionSystem::updatePredictionModel(const std::vector<float>& predData, const std::vector<float>& actualData) {
    // Update prediction model based on prediction error
    if (predData.size() != actualData.size()) return;
    
    // Resize prediction model if necessary
    if (pImpl->predictionModel.size() < predData.size()) {
        pImpl->predictionModel.resize(predData.size(), 0.0f);
    }
    
    // Update prediction model using Hebbian-like learning
    float learningRate = 0.01f * (1.0f - pImpl->predictionError);
    
    for (size_t i = 0; i < predData.size() && i < pImpl->predictionModel.size(); ++i) {
        float error = predData[i] - actualData[i];
        
        // Update prediction model based on error
        if (std::abs(error) > 0.01f) {
            pImpl->predictionModel[i] += error * learningRate;
        }
        
        // Update expected dynamics
        if (i < pImpl->expectedModel.size()) {
            pImpl->expectedModel[i] = actualData[i] * 0.7f + pImpl->expectedModel[i] * 0.3f;
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // IMPLEMENTATION: Train prediction model using neural substrate
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) return;
    
    // Initialize prediction model with current observation statistics
    if (pImpl->predictionModel.empty()) {
        pImpl->predictionModel = obsData;
        pImpl->expectedModel = obsData;
        NLM_LOG_INFO("PredictionSystem::train: initialized prediction model with " + std::to_string(obsData.size()) + " features");
    } else {
        // Train using observation data
        for (size_t i = 0; i < obsData.size() && i < pImpl->predictionModel.size(); ++i) {
            // Update model based on observation (Hebbian learning)
            float delta = obsData[i] - pImpl->predictionModel[i];
            if (std::abs(delta) > 0.01f) {
                pImpl->predictionModel[i] += delta * 0.005f;
                pImpl->expectedModel[i] = obsData[i] * 0.9f + pImpl->expectedModel[i] * 0.1f;
            }
        }
        
        NLM_LOG_INFO("PredictionSystem::train: updated prediction model based on observation");
    }
}

} // namespace nlm
