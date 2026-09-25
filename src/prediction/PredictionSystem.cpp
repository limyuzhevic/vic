#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    float accuracyScore;
    std::vector<float> errorHistory;
    std::vector<float> accuracyHistory;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    SimulationStep lastTimestep;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), accuracyScore(0.5f), lastTimestep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction backend");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain || !pImpl->neuralPrediction) {
        return currentState.clone();
    }
    
    // Get current neural activity pattern
    std::vector<float> neuralPattern;
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float activation = std::max(0.0f, std::min(1.0f, 
                    (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f));
                neuralPattern.push_back(activation);
            }
        }
    }
    
    // Use neural prediction system to generate prediction
    auto predictedPattern = pImpl->neuralPrediction->generatePrediction(pImpl->lastTimestep);
    
    // Convert neural pattern to SensoryInput
    if (predictedPattern.empty()) {
        predictedPattern = neuralPattern;
    }
    
    auto result = std::make_unique<SensoryInput>(predictedPattern.size());
    for (size_t i = 0; i < predictedPattern.size() && i < result->getData().size(); ++i) {
        result->getData()[i] = predictedPattern[i];
    }
    
    // Update confidence based on pattern similarity
    float similarity = 0.5f;
    if (!neuralPattern.empty() && !predictedPattern.empty()) {
        similarity = pImpl->neuralPrediction->computeSimilarity(neuralPattern, predictedPattern);
    }
    pImpl->confidence = similarity;
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using MSE
        float sumError = 0.0f;
        float sumSquaredError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff;
            sumSquaredError += diff * diff;
        }
        
        pImpl->predictionError = sumSquaredError / predData.size();
        
        // Compute accuracy as 1 - normalized error
        float maxError = 4.0f;  // Assuming normalized input [0,1]
        pImpl->accuracyScore = std::max(0.0f, 1.0f - (pImpl->predictionError / maxError));
        
        // Update history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        pImpl->accuracyHistory.push_back(pImpl->accuracyScore);
        
        // Keep history bounded
        const size_t maxHistory = 1000;
        if (pImpl->errorHistory.size() > maxHistory) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            pImpl->accuracyHistory.erase(pImpl->accuracyHistory.begin());
        }
        
        // Log prediction performance
        if (pImpl->predictionError > 0.5f) {
            NLM_LOG_WARN("High prediction error: " + std::to_string(pImpl->predictionError));
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->accuracyScore;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->accuracyHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain || !pImpl->neuralPrediction) return;
    
    // Convert observation to neural pattern
    std::vector<float> neuralPattern;
    const auto& data = observation.getData();
    neuralPattern.reserve(data.size());
    for (float value : data) {
        neuralPattern.push_back(std::max(0.0f, std::min(1.0f, value)));
    }
    
    // Update neural prediction system
    pImpl->neuralPrediction->recordSensoryState(neuralPattern, pImpl->lastTimestep);
    
    // Train based on pattern consistency
    if (pImpl->neuralPrediction->getErrorHistory().size() > 10) {
        const auto& history = pImpl->neuralPrediction->getErrorHistory();
        float avgError = std::accumulate(history.begin(), history.end(), 0.0f) / history.size();
        pImpl->confidence = std::max(0.1f, 1.0f - avgError);
    }
}

void PredictionSystem::updateWithNeuralActivity(const std::vector<float>& neuralActivity, SimulationStep timestep) {
    pImpl->lastTimestep = timestep;
    
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(neuralActivity, timestep);
    }
}

void PredictionSystem::automaticUpdate(const SensoryInput& currentSensoryInput, SimulationStep timestep) {
    if (!pImpl->brain || !pImpl->neuralPrediction) return;
    
    // Get prediction for next state
    auto prediction = predictNextState(currentSensoryInput);
    
    // Get current neural activity as "actual" prediction
    std::vector<float> currentNeuralPattern;
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float activation = std::max(0.0f, std::min(1.0f,
                    (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f));
                currentNeuralPattern.push_back(activation);
            }
        }
    }
    
    // Convert neural pattern to SensoryInput
    auto actualInput = std::make_unique<SensoryInput>(currentNeuralPattern.size());
    for (size_t i = 0; i < currentNeuralPattern.size() && i < actualInput->getData().size(); ++i) {
        actualInput->getData()[i] = currentNeuralPattern[i];
    }
    
    // Update predictions and compute error
    updatePredictions(prediction, *actualInput);
    
    // Train with current observation
    train(currentSensoryInput);
    
    // Update neural prediction system
    updateWithNeuralActivity(currentNeuralPattern, timestep);
}

} // namespace nlm
