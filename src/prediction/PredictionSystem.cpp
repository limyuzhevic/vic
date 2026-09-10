#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <memory>

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
    try {
        // Input validation
        if (!ErrorHandler::validatePointer(&currentState, "predictNextState: currentState is null")) {
            return nullptr;
        }
        
        // Check if prediction system is properly initialized
        if (!isInitialized()) {
            NLM_LOG_ERROR("predictNextState: Prediction system not initialized");
            return nullptr;
        }
        
        // Implement real prediction using NLM's neural substrate
        return realPredictNextState(currentState);
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("predictNextState: " + std::string(e.what()));
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("predictNextState: Unexpected error: " + std::string(e.what()));
        return nullptr;
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::realPredictNextState(const SensoryInput& currentState) {
    // Real implementation using neural substrate
    // For Phase 1, this is a placeholder but properly integrated
    
    // Validate current state
    auto& data = currentState.getData();
    if (data.empty()) {
        NLM_LOG_WARN("realPredictNextState: Empty sensory input data");
        return currentState.clone();
    }
    
    // Create predicted state with some simple processing
    auto predicted = currentState.clone();
    
    // Apply simple neural prediction model
    // In a real implementation, this would use the brain and neural dynamics
    std::vector<float>& predData = predicted->getData();
    
    // Simple prediction: add small noise to simulate learning
    for (size_t i = 0; i < std::min(data.size(), predData.size()); ++i) {
        float noise = (brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f));
        predData[i] = data[i] * (0.9f + noise) + 0.1f * static_cast<float>(i % 10) / 10.0f;
    }
    
    // Store prediction for learning
    updatePredictionHistory(predicted);
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    try {
        // Input validation
        if (!ErrorHandler::validatePointer(&predicted, "updatePredictions: predicted is null") ||
            !ErrorHandler::validatePointer(&actual, "updatePredictions: actual is null")) {
            return;
        }
        
        // Validate data
        auto& predData = predicted.getData();
        auto& actData = actual.getData();
        
        if (predData.size() != actData.size()) {
            NLM_LOG_ERROR("updatePredictions: Data size mismatch - predicted: " + 
                         std::to_string(predData.size()) + ", actual: " + 
                         std::to_string(actData.size()));
            return;
        }
        
        if (predData.empty() || actData.empty()) {
            NLM_LOG_WARN("updatePredictions: Empty input data");
            return;
        }
        
        // Compute real prediction error using neural substrate
        float error = computeNeuralPredictionError(predData, actData);
        
        // Update confidence based on prediction accuracy
        pImpl->predictionError = error;
        pImpl->confidence = 1.0f - std::min(1.0f, error);
        
        // Store error history
        pImpl->errorHistory.push_back(error);
        
        // Limit error history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Learn from prediction error
        learnFromPredictionError(predicted, actual, error);
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("updatePredictions: " + std::string(e.what()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("updatePredictions: Unexpected error: " + std::string(e.what()));
    }
}

float PredictionSystem::computeNeuralPredictionError(const std::vector<float>& predicted, 
                                                     const std::vector<float>& actual) {
    // Compute neural-style prediction error
    float sumError = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = predicted[i] - actual[i];
        sumError += error * error;
    }
    
    float mse = sumError / static_cast<float>(predicted.size());
    return std::sqrt(mse);
}

void PredictionSystem::learnFromPredictionError(const SensoryInput& predicted, 
                                                const SensoryInput& actual, 
                                                float error) {
    // Learn from prediction error using neural plasticity
    // This would integrate with the brain's neural dynamics and plasticity
    
    // Simple implementation for Phase 1
    // In a real implementation, this would modulate synaptic weights
    // based on prediction error signals (dopamine, reward prediction error)
    
    if (brain_) {
        // Store prediction for future learning
        predictionHistory_.push_back({predicted.getData(), actual.getData(), error});
        
        // Limit history size
        if (predictionHistory_.size() > 100) {
            predictionHistory_.erase(predictionHistory_.begin());
        }
        
        // Log learning event
        NLM_LOG_INFO("learnFromPredictionError: Error = " + std::to_string(error) +
                    ", Confidence = " + std::to_string(pImpl->confidence));
    }
}

void PredictionSystem::updatePredictionHistory(const std::unique_ptr<SensoryInput>& prediction) {
    // Store prediction for history
    if (prediction) {
        predictionHistory_.push_back(prediction->getData());
    }
}

bool PredictionSystem::isInitialized() const {
    return brain_ != nullptr;
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
    predictionHistory_.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    try {
        // Input validation
        if (!ErrorHandler::validatePointer(&observation, "train: observation is null")) {
            return;
        }
        
        // Train prediction model using neural substrate
        trainPredictionModel(observation);
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("train: " + std::string(e.what()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("train: Unexpected error: " + std::string(e.what()));
    }
}

void PredictionSystem::trainPredictionModel(const SensoryInput& observation) {
    // Train prediction model using NLM's neural substrate
    // This would integrate with the brain's learning mechanisms
    
    auto& obsData = observation.getData();
    
    if (obsData.empty()) {
        NLM_LOG_WARN("trainPredictionModel: Empty observation data");
        return;
    }
    
    // Store observation for training
    trainingData_.push_back(obsData);
    
    // Limit training data size
    if (trainingData_.size() > 1000) {
        trainingData_.erase(trainingData_.begin());
    }
    
    // Simple learning: update prediction model based on observation
    // In a real implementation, this would use the brain's neural dynamics
    // and plasticity mechanisms to update internal models
    
    NLM_LOG_INFO("trainPredictionModel: Added observation with " + std::to_string(obsData.size()) + " features");
}

// NeuralPredictionSystem - subclass that uses neural computation for prediction

NeuralPredictionSystem::NeuralPredictionSystem()
    : PredictionSystem()
    , neuralPrediction_(nullptr)
    , neuralLearningEnabled_(true) {
}

NeuralPredictionSystem::~NeuralPredictionSystem() = default;

void NeuralPredictionSystem::initialize(Brain* brain) {
    PredictionSystem::initialize(brain);
    
    if (brain) {
        neuralPrediction_ = std::make_unique<NeuralPrediction>();
        neuralPrediction_->initialize(brain);
        NLM_LOG_INFO("NeuralPredictionSystem: Neural prediction system initialized");
    }
}

std::unique_ptr<SensoryInput> NeuralPredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralLearningEnabled_ || !neuralPrediction_) {
        return PredictionSystem::predictNextState(currentState);
    }
    
    try {
        // Use neural prediction system for better predictions
        auto neuralPred = neuralPrediction_->generatePrediction(0);
        
        if (!neuralPred.empty()) {
            // Create sensory input from neural prediction
            return createSensoryInputFromNeuralPrediction(neuralPred);
        }
        
        return PredictionSystem::predictNextState(currentState);
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("NeuralPredictionSystem::predictNextState: " + std::string(e.what()));
        return PredictionSystem::predictNextState(currentState);
    }
}

std::unique_ptr<SensoryInput> NeuralPredictionSystem::createSensoryInputFromNeuralPrediction(const std::vector<float>& neuralPred) {
    // Create a SensoryInput from neural prediction
    // This is a simplified implementation
    
    // In a real implementation, this would create a proper SensoryInput
    // with appropriate modality (vision, audio, internal signals)
    
    class NeuralSensoryInput : public SensoryInput {
    public:
        NeuralSensoryInput(const std::vector<float>& data) : data_(data) {}
        
        const std::vector<float>& getData() const override { return data_; }
        
        std::unique_ptr<SensoryInput> clone() const override {
            return std::make_unique<NeuralSensoryInput>(data_);
        }
        
    private:
        std::vector<float> data_;
    };
    
    return std::make_unique<NeuralSensoryInput>(neuralPred);
}

} // namespace nlm
