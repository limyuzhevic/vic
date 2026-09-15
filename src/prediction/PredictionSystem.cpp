#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    float predictionErrorAccumulator;
    std::vector<float> errorHistory;
    Brain* brain;  // Reference to brain for integration
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionErrorAccumulator(0.0f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::update(const Brain& brain) {
    // Update prediction system with current brain state
    // This is the key integration point for Phase 6
    pImpl->brain = const_cast<Brain*>(&brain);  // Store reference
    
    // Compute prediction error from various sources
    float totalError = 0.0f;
    int errorSources = 0;
    
    // Error from working memory changes (pattern mismatch)
    if (brain.getWorkingMemory()) {
        auto workingMem = brain.getWorkingMemory();
        auto neurons = workingMem->getMemoryNeurons();
        
        for (const auto& neuron : neurons) {
            float activation = workingMem->getNeuronActivation(neuron);
            // Deviation from expected activation (0.5 is neutral)
            totalError += std::abs(activation - 0.5f);
            errorSources++;
        }
    }
    
    // Error from spike patterns and activity
    totalError += brain.getPendingSpikeEventCount() * 0.01f;
    errorSources++;
    
    // Error from developmental stage (stable developmental state = low error)
    if (brain.getDevelopmentSystem()) {
        DevelopmentalStage stage = brain.getDevelopmentSystem()->getDevelopmentalStage();
        // Higher developmental stability = lower prediction error
        totalError += (static_cast<int>(stage) / 10.0f) * 0.5f;
        errorSources++;
    }
    
    // Calculate average prediction error
    pImpl->predictionErrorAccumulator = totalError / (errorSources > 0 ? errorSources : 1);
    pImpl->predictionError = pImpl->predictionErrorAccumulator;
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Update confidence based on prediction stability
    // Lower error = higher confidence
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    
    // Update neuromodulation systems based on prediction error
    if (pImpl->brain) {
        if (pImpl->predictionError > 0.2f) {
            // High prediction error triggers curiosity
            if (pImpl->brain->getCuriosity()) {
                pImpl->brain->getCuriosity()->update(pImpl->predictionError * 0.5f);
            }
            
            // High prediction error also increases novelty detection
            if (pImpl->brain->getNovelty()) {
                pImpl->brain->getNovelty()->update(pImpl->predictionError * 0.3f);
            }
        }
        
        // Reward prediction error for dopamine
        if (pImpl->brain->getPredictionErrorSignal()) {
            pImpl->brain->getPredictionErrorSignal()->update(pImpl->predictionError);
        }
    }
}

void PredictionSystem::updatePredictionError(float error) {
    // External update of prediction error
    if (pImpl) {
        pImpl->predictionError = std::max(0.0f, std::min(1.0f, error));
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error
        pImpl->confidence = 1.0f - pImpl->predictionError;
        
        // Update brain neuromodulation if available
        if (pImpl->brain) {
            if (pImpl->predictionError > 0.1f && pImpl->brain->getCuriosity()) {
                pImpl->brain->getCuriosity()->update(pImpl->predictionError * 0.2f);
            }
        }
    }
}

void PredictionSystem::updateConfidence(float confidence) {
    // Update prediction confidence based on concept formation stability
    if (pImpl) {
        pImpl->confidence = std::max(0.0f, std::min(1.0f, confidence));
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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
    // TODO PHASE 2: Train prediction model
    // PLACEHOLDER: In full implementation, this would train the prediction model
    // using neural patterns from working memory and episodic memory
    NLM_LOG_DEBUG("PredictionSystem: Train called with observation size " + std::to_string(observation.getData().size()));
}

} // namespace nlm
