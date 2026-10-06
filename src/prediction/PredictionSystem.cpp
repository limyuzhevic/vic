#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;
    
    // Temporal prediction components
    std::vector<std::vector<float>> predictedStates;
    std::vector<SimulationStep> predictionSteps;
    
    // Action consequence prediction
    std::vector<std::pair<ActionType, std::vector<float>>> actionConsequences;
    std::vector<float> actionPredictions;
    
    // Working memory integration
    std::vector<std::tuple<SimulationStep, std::vector<float>, std::vector<float>, float>> predictionRecords;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    pImpl->brain = nullptr;
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
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
    pImpl->predictedStates.clear();
    pImpl->predictionSteps.clear();
    pImpl->predictionRecords.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, record the observation for future predictions
    if (pImpl->brain) {
        std::vector<float> obsData = observation.getData();
        if (!obsData.empty()) {
            pImpl->predictedStates.push_back(obsData);
            pImpl->predictionSteps.push_back(pImpl->brain->getSimulationClock()->getCurrentStep());
        }
    }
}

std::vector<float> PredictionSystem::generatePrediction(SimulationStep step) {
    // Use NeuralPrediction for advanced predictive coding
    std::vector<float> prediction;
    
    if (pImpl->brain && pImpl->brain->getPredictionSystem() && 
        pImpl->brain->getPredictionSystem()->getPredictionNeurons() != nullptr) {
        // Get neural prediction system
        auto* neuralPred = pImpl->brain->getPredictionSystem()->getPredictionNeurons();
        // TODO: Actually use the neural prediction system
    }
    
    return prediction;
}

std::vector<float> PredictionSystem::getActionPredictions(SimulationStep step) const {
    return pImpl->actionPredictions;
}

void PredictionSystem::updateWithPredictions(
    float error,
    float confidence,
    float explorationBias,
    float actionBias,
    SimulationStep step
) {
    // Update internal state based on prediction information
    pImpl->predictionError = error;
    pImpl->confidence = confidence;
    
    // Update action predictions based on biases
    pImpl->actionPredictions.clear();
    
    // Generate action predictions based on confidence and bias
    size_t numActions = 6;  // Assuming 6 action types
    pImpl->actionPredictions.resize(numActions);
    
    std::mt19937 rng(42); // Fixed seed for consistency
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < numActions; ++i) {
        // Base prediction based on confidence
        float basePrediction = dist(rng) * confidence;
        
        // Modify based on exploration vs exploitation
        if (i < 2) {  // Higher actions get more exploration bias
            basePrediction = basePrediction * (1.0f - explorationBias) + 
                            dist(rng) * explorationBias * 0.5f;
        }
        
        // Modify based on action selection bias
        if (i % 2 == 0) {  // Even-indexed actions get positive bias
            basePrediction *= (1.0f + actionBias);
        } else {  // Odd-indexed actions get negative bias
            basePrediction *= (1.0f - actionBias * 0.5f);
        }
        
        // Clamp to valid range
        pImpl->actionPredictions[i] = std::max(0.0f, std::min(1.0f, basePrediction));
    }
}

void PredictionSystem::storePrediction(
    SimulationStep step,
    const std::vector<float>& predicted,
    const std::vector<float>& actual,
    float error
) {
    // Store prediction for working memory integration
    pImpl->predictionRecords.emplace_back(step, predicted, actual, error);
    
    // Keep only recent records
    if (pImpl->predictionRecords.size() > 1000) {
        pImpl->predictionRecords.erase(pImpl->predictionRecords.begin());
    }
}

} // namespace nlm
