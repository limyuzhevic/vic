#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    if (brain) {
        neuralPrediction_ = std::make_unique<NeuralPrediction>();
        neuralPrediction_->initialize(brain);
        NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction");
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralPrediction_) {
        return currentState.clone();
    }
    
    // Convert current sensory input to vector format
    const auto& currentData = currentState.getData();
    
    // Generate prediction using NeuralPrediction system
    // NeuralPrediction returns neural activity patterns as vectors
    std::vector<float> prediction = neuralPrediction_->generatePrediction(0);
    
    // Create a new SensoryInput with the predicted state
    auto predictedInput = std::make_unique<Vision>();
    predictedInput->setData(prediction);
    
    NLM_LOG_INFO("PredictionSystem::predictNextState generated prediction");
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!neuralPrediction_) {
        return;
    }
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        float error = sumError / predData.size();
        pImpl->predictionError = error;
        pImpl->errorHistory.push_back(error);
        
        // Update neural prediction with observation
        neuralPrediction_->updateWithObservation(actualData, 1);
        
        NLM_LOG_INFO("PredictionSystem::updatePredictions computed error: " + std::to_string(error));
    }
}

float PredictionSystem::getPredictionError() const {
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
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!neuralPrediction_) {
        return;
    }
    
    const auto& data = observation.getData();
    neuralPrediction_->recordSensoryState(data, 0);
    
    NLM_LOG_INFO("PredictionSystem::train completed");
}

} // namespace nlm
