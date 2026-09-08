#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Prediction model parameters
    float predictionGain;
    float confidenceDecayRate;
    
    // Current prediction
    std::unique_ptr<SensoryInput> currentPrediction;
    float predictionError;
    float predictionConfidence;
    
    // History
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<std::unique_ptr<SensoryInput>> predictionHistory;
    
    // For temporal sequence prediction
    std::vector<std::unique_ptr<SensoryInput>> recentObservations;
    
    Impl() : brain(nullptr), predictionGain(0.5f), confidenceDecayRate(0.1f), 
             predictionError(0.0f), predictionConfidence(1.0f) {}
};

PredictionSystem::PredictionSystem()
    : pImpl(new Impl)
{
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return nullptr;
    }
    
    // Get current sensory pattern
    auto pattern = currentState.getData();
    
    // Get working memory traces
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    std::vector<float> workingPattern;
    if (workingMemory) {
        workingPattern = workingMemory->getActiveTraces();
    }
    
    // Create prediction based on:
    // 1. Current sensory state
    // 2. Working memory context
    // 3. Recent observation history
    auto prediction = std::make_unique<SensoryInput>();
    
    // If we have working memory, use it as basis for prediction
    if (!workingPattern.empty()) {
        prediction->setData(workingPattern);
    } else {
        prediction->setData(pattern);
    }
    
    // Add small random variation for exploration
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.1f, 0.1f);
    
    auto& predData = prediction->getData();
    for (auto& value : predData) {
        value += dis(gen) * predictionGain;
    }
    
    // Store prediction history
    pImpl->predictionHistory.push_back(std::make_unique<SensoryInput>(*prediction));
    if (pImpl->predictionHistory.size() > 100) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Calculate confidence based on working memory strength
    pImpl->predictionConfidence = 0.5f;
    if (!workingPattern.empty()) {
        // Higher working memory activity = higher prediction confidence
        float totalWorkingActivity = std::accumulate(workingPattern.begin(), workingPattern.end(), 0.0f);
        pImpl->predictionConfidence = std::min(1.0f, totalWorkingActivity * 0.1f);
    }
    
    pImpl->confidenceHistory.push_back(pImpl->predictionConfidence);
    if (pImpl->confidenceHistory.size() > 100) {
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
    }
    
    pImpl->currentPrediction = std::make_unique<SensoryInput>(*prediction);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error
    auto& predData = predicted.getData();
    auto& actualData = actual.getData();
    
    float totalError = 0.0f;
    size_t minSize = std::min(predData.size(), actualData.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float error = actualData[i] - predData[i];
        totalError += std::abs(error);
    }
    
    if (minSize > 0) {
        pImpl->predictionError = totalError / minSize;
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error magnitude
        pImpl->predictionConfidence = std::max(0.0f, 1.0f - pImpl->predictionError * 10.0f);
        
        // Store observation for temporal prediction
        pImpl->recentObservations.push_back(std::make_unique<SensoryInput>(actual));
        if (pImpl->recentObservations.size() > 10) {
            pImpl->recentObservations.erase(pImpl->recentObservations.begin());
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->predictionConfidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->confidenceHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->recentObservations.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    // Store observation
    auto newObservation = std::make_unique<SensoryInput>(observation);
    
    // Use prediction error to modulate plasticity in connected neurons
    if (pImpl->predictionError > 0.1f) {
        auto& obsData = observation.getData();
        
        // Get brain regions for plasticity modulation
        auto regions = pImpl->brain->getRegions();
        for (auto& region : regions) {
            for (auto& synapse : region->getSynapses()) {
                if (synapse->getPlasticityFlags().stdp || synapse->getPlasticityFlags().hebbian) {
                    // Modulate synaptic change based on prediction error
                    float plasticityMod = 1.0f + pImpl->predictionError * 0.5f;
                    
                    // This would integrate with actual plasticity mechanisms
                    // For now, just log the idea
                    if (pImpl->predictionError > 0.5f) {
                        NLM_LOG_INFO("PredictionSystem: High prediction error " + 
                                    std::to_string(pImpl->predictionError) +
                                    " should modulate plasticity");
                    }
                }
            }
        }
    }
    
    // Update recent observations
    pImpl->recentObservations.push_back(std::move(newObservation));
    if (pImpl->recentObservations.size() > 20) {
        pImpl->recentObservations.erase(pImpl->recentObservations.begin());
    }
}

} // namespace nlm
