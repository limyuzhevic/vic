// BrainIntegration.cpp - Implementation of BrainIntegration class

#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct BrainIntegration::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Integration configuration
    float surpriseThreshold;
    size_t sequenceMemorySize;
    size_t predictionHorizon;
    
    // State
    float currentPredictionError;
    float currentNovelty;
    float currentCuriosity;
    
    Impl() : brain(nullptr), surpriseThreshold(0.3f), sequenceMemorySize(10), 
             predictionHorizon(1), currentPredictionError(0.0f), 
             currentNovelty(0.0f), currentCuriosity(0.0f) {}
};

BrainIntegration::BrainIntegration() : pImpl(new Impl) {}

BrainIntegration::~BrainIntegration() = default;

void BrainIntegration::initialize(Brain* brainPtr) {
    if (!brainPtr) {
        NLM_LOG_ERROR("BrainIntegration: Cannot initialize with null brain");
        return;
    }
    
    pImpl->brain = brainPtr;
    
    // Initialize neural prediction system
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brainPtr);
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Configure from brain config if available
    if (brainPtr->getConfig()) {
        auto config = brainPtr->getConfig();
        
        pImpl->surpriseThreshold = config->getOr<float>("prediction_surprise_threshold", 
                                                     BrainIntegrationConfig::DEFAULT_PREDICTION_SURPRISE_THRESHOLD);
        
        pImpl->sequenceMemorySize = config->getOr<size_t>("prediction_sequence_memory_size",
                                                       BrainIntegrationConfig::DEFAULT_PREDICTION_SEQUENCE_MEMORY_SIZE);
        
        pImpl->predictionHorizon = config->getOr<size_t>("prediction_horizon_steps",
                                                       BrainIntegrationConfig::DEFAULT_PREDICTION_HORIZON_STEPS);
        
        // Apply configuration to neural prediction
        pImpl->neuralPrediction->setSequenceMemorySize(pImpl->sequenceMemorySize);
        pImpl->neuralPrediction->setPredictionHorizon(pImpl->predictionHorizon);
    }
    
    NLM_LOG_INFO("BrainIntegration initialized");
}

void BrainIntegration::updatePredictionSystem(SimulationStep currentStep) {
    if (!pImpl->brain || !pImpl->neuralPrediction) {
        return;
    }
    
    // Get current sensory state from brain
    std::vector<float> currentSensory;
    for (const auto* neuron : pImpl->brain->getWorkingMemory()->getMemoryNeurons()) {
        currentSensory.push_back(1.0f);  // Simplified - in real implementation get actual activation
    }
    
    if (currentSensory.empty()) {
        return;
    }
    
    // Record current state for prediction
    pImpl->neuralPrediction->recordSensoryState(currentSensory, currentStep);
    
    // Generate prediction for next state
    std::vector<float> predicted = pImpl->neuralPrediction->generatePrediction(currentStep);
    
    // Update with actual observation to compute error
    float predictionError = pImpl->neuralPrediction->updateWithObservation(currentSensory, currentStep);
    
    pImpl->currentPredictionError = predictionError;
    
    if (predictionError > 0.0f) {
        // Update prediction system
        pImpl->predictionSystem->updatePredictions(
            CreateSensoryInput(currentSensory),  // Would create proper SensoryInput
            CreateSensoryInput(predicted)
        );
        
        // Update neuromodulation systems
        if (pImpl->brain->getNovelty()) {
            pImpl->brain->getNovelty()->updateWithPredictionError(predictionError);
            pImpl->currentNovelty = pImpl->brain->getNovelty()->getLevel();
        }
        
        if (pImpl->brain->getCuriosity()) {
            pImpl->brain->getCuriosity()->update(predictionError, predictionError, 0.001);
            pImpl->currentCuriosity = pImpl->brain->getCuriosity()->getLevel();
        }
        
        // Check if surprising event
        if (predictionError > pImpl->surpriseThreshold) {
            NLM_LOG_INFO("Surprising prediction error detected: " + std::to_string(predictionError));
        }
    }
}

NeuralPrediction* BrainIntegration::getNeuralPrediction() {
    return pImpl->neuralPrediction.get();
}

PredictionSystem* BrainIntegration::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

void BrainIntegration::setSequenceMemorySize(size_t size) {
    pImpl->sequenceMemorySize = size;
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->setSequenceMemorySize(size);
    }
}

void BrainIntegration::setPredictionHorizon(size_t steps) {
    pImpl->predictionHorizon = steps;
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->setPredictionHorizon(steps);
    }
}

void BrainIntegration::setPredictionSurpriseThreshold(float threshold) {
    pImpl->surpriseThreshold = threshold;
}

float BrainIntegration::getPredictionError() const {
    return pImpl->currentPredictionError;
}

float BrainIntegration::getNoveltyLevel() const {
    return pImpl->currentNovelty;
}

float BrainIntegration::getCuriosityLevel() const {
    return pImpl->currentCuriosity;
}

void BrainIntegration::reset() {
    pImpl->neuralPrediction->clearHistory();
    pImpl->predictionSystem->clearHistory();
    pImpl->currentPredictionError = 0.0f;
    pImpl->currentNovelty = 0.0f;
    pImpl->currentCuriosity = 0.0f;
}

SensoryInput BrainIntegration::CreateSensoryInput(const std::vector<float>& data) {
    // Placeholder - would create proper SensoryInput in Phase 2
    SensoryInput input;
    // In real implementation, set up proper SensoryInput with type and data
    return input;
}

} // namespace nlm