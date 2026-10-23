#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), neuralPrediction_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralPrediction_) {
        return currentState.clone();
    }
    
    // Use neural prediction to generate next state
    std::vector<float> predictedData = neuralPrediction_->generatePrediction(0);
    
    if (!predictedData.empty()) {
        // Create a Vision input with the predicted data
        auto predictedInput = std::make_unique<Vision>(1, predictedData.size());
        predictedInput->setData(predictedData);
        return predictedInput;
    }
    
    // Fallback to current state if prediction fails
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                        const SensoryInput& actual) {
    if (!neuralPrediction_) {
        // Basic error calculation
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
            
            // Update confidence based on error
            pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        }
    } else {
        // Use neural prediction error computation
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            float error = sumError / predData.size();
            pImpl->predictionError = neuralPrediction_->updateWithObservation(actualData, 0);
            pImpl->errorHistory.push_back(pImpl->predictionError);
            
            // Update confidence based on prediction error
            pImpl->confidence = 1.0f - std::min(1.0f, pImpl->predictionError);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionError();
    }
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionConfidence();
    }
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (neuralPrediction_) {
        // Use neural prediction training
        const auto& data = observation.getData();
        neuralPrediction_->recordSensoryState(data, 0);
    }
}

} // namespace nlm
