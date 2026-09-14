#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction integration
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<PredictionError> predictionErrorSignal;
    
    // Sensory state integration
    std::vector<float> currentSensoryState;
    SimulationStep lastStep;
    
    // Brain region for sensory processing
    RegionId sensoryRegionId;
    
    Impl() : predictionError(0.0f), confidence(0.5f), lastStep(0), sensoryRegionId(INVALID_REGION_ID) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    // Initialize neural prediction system
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    
    // Initialize prediction error system
    pImpl->predictionErrorSignal = std::make_unique<PredictionError>();
    pImpl->predictionErrorSignal->initialize(brain);
    
    // Connect prediction error to dopamine system if available
    if (brain->getDopamine()) {
        pImpl->predictionErrorSignal->connectToDopamine(brain->getDopamine());
    }
    
    // Get sensory region for prediction
    pImpl->sensoryRegionId = RegionId(1); // Default to region 1 (should be configurable)
    
    NLM_LOG_INFO("PredictionSystem initialized with neural prediction and error systems");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Get current state from sensory input
    pImpl->currentSensoryState = currentState.getData();
    
    // Update neural prediction system
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(pImpl->currentSensoryState, pImpl->lastStep);
        auto predictedState = pImpl->neuralPrediction->generatePrediction(pImpl->lastStep);
        
        // Create predicted sensory input
        auto predictedInput = std::make_unique<SensoryInput>();
        
        // For now, just copy the data - in real implementation would use neural activity patterns
        predictedInput->setData(predictedState);
        
        pImpl->confidence = pImpl->neuralPrediction->getPredictionConfidence();
        pImpl->predictionError = pImpl->neuralPrediction->getPredictionError();
        
        return predictedInput;
    }
    
    // Fallback to simple copy if neural prediction not available
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Update neural prediction system with actual observation
        if (pImpl->neuralPrediction) {
            float error = pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->lastStep);
            pImpl->predictionError = error;
            pImpl->confidence = 1.0f - std::min(1.0f, error);
        }
        
        // Compute prediction error using neural system
        if (pImpl->predictionErrorSignal) {
            pImpl->predictionErrorSignal->computeError(predData, actualData);
            pImpl->predictionError = pImpl->predictionErrorSignal->getError();
            pImpl->confidence = pImpl->predictionErrorSignal->getErrorComponents().confidence;
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 10000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
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
    if (pImpl->predictionErrorSignal) {
        pImpl->predictionErrorSignal->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train neural prediction system with new observation
    if (pImpl->neuralPrediction) {
        // Extract state from observation
        const auto& data = observation.getData();
        if (!data.empty()) {
            // Update prediction model with new experience
            pImpl->neuralPrediction->recordSensoryState(data, pImpl->lastStep);
            
            // The system learns through synaptic plasticity
            // NeuralPrediction maintains internal representations
        }
    }
}

void PredictionSystem::step(SimulationStep step, Timestamp time) {
    pImpl->lastStep = step;
    
    // Update neural prediction with temporal dynamics
    if (pImpl->neuralPrediction && pImpl->lastStep > 0) {
        // Update prediction based on time
        pImpl->neuralPrediction->generatePrediction(pImpl->lastStep);
    }
}

} // namespace nlm
