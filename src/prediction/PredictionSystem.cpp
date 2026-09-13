#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Create neural prediction system if brain exists
    if (brain) {
        neuralPrediction_ = std::make_unique<NeuralPrediction>();
        neuralPrediction_->initialize(brain);
        
        // Configure based on brain settings
        auto config = brain->getConfig();
        if (config) {
            neuralPrediction_->setSequenceMemorySize(config->getOr<size_t>("sequence_memory_size", 100));
            neuralPrediction_->setPredictionHorizon(config->getOr<size_t>("prediction_horizon", 5));
        }
    }
    
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural prediction for real prediction
    if (neuralPrediction_) {
        // Neural prediction would need to produce a SensoryInput
        // For now, return current state as placeholder
        auto predictedInput = currentState.clone();
        NLM_LOG_INFO("PredictionSystem: Using neural prediction");
        return predictedInput;
    }
    
    // Fallback to placeholder
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Use neural prediction to update and compute error
    if (neuralPrediction_) {
        // Update neural prediction with actual observation
        // This would normally compute prediction error
        float error = neuralPrediction_->updateWithObservation(actual.getData(), 0);
        pImpl->predictionError = error;
        pImpl->errorHistory.push_back(error);
        
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    } else {
        // Fallback to simple error calculation
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
    // Train neural prediction system
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(observation.getData(), 0);
        NLM_LOG_INFO("PredictionSystem: Training neural prediction");
    }
}

} // namespace nlm
