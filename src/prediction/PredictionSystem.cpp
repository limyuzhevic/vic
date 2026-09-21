#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    class Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain) {
    if (pImpl) {
        pImpl->brain = brain;
        parentBrain = brain;
    }
}

class Brain* PredictionSystem::getBrain() const {
    return pImpl->brain;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!parentBrain) {
        return currentState.clone();
    }
    
    // Get current sensory state
    const auto& sensoryState = currentState.getData();
    
    // Check neuromodulation influence on prediction
    if (parentBrain->getPredictionErrorSignal()) {
        float predictionError = parentBrain->getPredictionErrorSignal()->getLevel();
        // High prediction error increases prediction variance
        float variance = 1.0f + predictionError * 2.0f;
    }
    
    // Check attention mechanism influence
    if (parentBrain->getAttention()) {
        // Attention modulates prediction strength
        // Neurons with high attention get stronger prediction weights
        parentBrain->getAttention()->update(0.001f);
    }
    
    // Apply prediction error to concept formation
    if (parentBrain->getConceptFormation() && pImpl->predictionError > 0.5f) {
        parentBrain->getConceptFormation()->integratePredictionError(
            pImpl->predictionError, parentBrain->getTotalSpikeCount());
    }
    
    // Generate prediction based on neural dynamics
    // In a real implementation, this would use the brain's internal state
    // For now, return a modified version of current state
    auto prediction = currentState.clone();
    
    // Add prediction noise to simulate uncertainty
    std::vector<float> predData = prediction->getData();
    for (size_t i = 0; i < predData.size(); ++i) {
        // Add small noise based on prediction confidence
        float noise = (pImpl->confidence - 0.5f) * 0.1f;
        predData[i] += noise;
    }
    prediction->setData(predData);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sqrt(sumError / predData.size());
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error magnitude
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
    }
}

void PredictionSystem::recordSensoryState(const std::vector<float>& sensoryState,
                                        SimulationStep currentStep) {
    // Convert to SensoryInput
    SensoryInput input;
    input.setData(sensoryState);
    
    // Make prediction
    auto prediction = predictNextState(input);
    if (prediction) {
        updatePredictions(*prediction, input);
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
    // Simple training: update internal state based on observation
    auto prediction = predictNextState(observation);
    if (prediction) {
        updatePredictions(*prediction, observation);
    }
}

} // namespace nlm
