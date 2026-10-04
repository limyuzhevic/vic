#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Forward model parameters
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Prediction cache
    std::vector<std::vector<float>> predictedStates;
    std::vector<float> predictedRewards;
    
    // Temporal predictions
    std::vector<SimulationStep> predictionTimestamps;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->errorHistory.reserve(10000);
    pImpl->predictedStates.reserve(1000);
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(
    const SensoryInput& currentState,
    const class Action& action) {
    
    if (!brain_) {
        return currentState.clone();
    }
    
    // In a real implementation, this would use neural forward models
    // to predict sensory consequences of actions
    // For now, return a modified version of current state
    
    auto predicted = currentState.clone();
    const auto& currentData = currentState.getData();
    auto& predictedData = predicted->getData();
    
    // Simple prediction: add some action-dependent modulation
    float actionModulation = 0.0f;
    
    switch (action.getType()) {
        case ActionType::MoveForward:
            actionModulation = 0.2f;
            break;
        case ActionType::TurnLeft:
            actionModulation = -0.1f;
            break;
        case ActionType::TurnRight:
            actionModulation = 0.1f;
            break;
        case ActionType::Wait:
            actionModulation = 0.0f;
            break;
        default:
            actionModulation = 0.05f;
            break;
    }
    
    // Apply modulation to prediction
    for (size_t i = 0; i < predictedData.size() && i < currentData.size(); ++i) {
        predictedData[i] = currentData[i] + actionModulation;
    }
    
    // Store prediction for learning
    if (pImpl->predictedStates.size() >= pImpl->predictedStates.capacity()) {
        pImpl->predictedStates.erase(pImpl->predictedStates.begin());
    }
    
    pImpl->predictedStates.push_back(predictedData);
    pImpl->predictedRewards.push_back(getExpectedReward(predicted));
    pImpl->predictionTimestamps.push_back(0); // Will be updated by brain step
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                         const SensoryInput& actual) {
    // Compute prediction error between predicted and actual sensory input
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() != actualData.size() || predictedData.empty()) {
        pImpl->predictionError = 1.0f;  // Maximum error
    } else {
        // Compute mean squared error
        float sumError = 0.0f;
        for (size_t i = 0; i < predictedData.size(); ++i) {
            float diff = predictedData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predictedData.size();
        
        // Add to error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 10000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    }
    
    // Trigger learning when prediction error is high
    if (pImpl->predictionError > 0.1f) {
        // Prediction error is high - time to learn
        train(actual);
    }
}

void PredictionSystem::processSensoryInput(const SensoryInput& input) {
    if (!brain_) return;
    
    // Store current observation
    auto& currentState = brain_->getCurrentSensoryState();
    
    // If we have a stored prediction for current timestep, compare
    if (!pImpl->predictedStates.empty()) {
        // Update the most recent prediction
        auto predicted = pImpl->predictedStates.back();
        if (!predicted.empty()) {
            // Convert back to SensoryInput for comparison
            // This is simplified - real implementation would handle types
            float error = pImpl->predictionError;  // From previous update
            
            // Use prediction error to drive attention
            if (auto* attention = brain_->getAttention()) {
                // High prediction error -> increase attention to current input
                if (error > 0.2f) {
                    // Create a temporary action to trigger attention mechanism
                    auto action = std::make_unique<Action>(ActionType::Wait);
                    attention->applyBottomUpSalience(
                        *reinterpret_cast<NeuronId*>(&input.getData().data()),  // Simplified
                        error * 2.0f);
                }
            }
            
            // Use prediction error to modulate dopamine
            if (auto* dopamine = brain_->getDopamine()) {
                // Positive prediction error -> increase dopamine (learning signal)
                dopamine->setLevel(error * 0.5f);
            }
        }
    }
    
    // Also provide prediction error to curiosity system
    if (auto* curiosity = brain_->getCuriosity()) {
        // Novelty/uncertainty from prediction error drives curiosity
        curiosity->setLevel(pImpl->predictionError);
    }
}

float PredictionSystem::getExpectedReward(const SensoryInput& predictedState) const {
    // Simple expected reward calculation based on prediction confidence
    // Higher confidence = higher expected reward
    float reward = pImpl->confidence * 0.5f;
    
    // Add some action-dependent reward
    return reward;
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
    pImpl->predictedRewards.clear();
    pImpl->predictionTimestamps.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using observed sensory input
    // In a real implementation, this would update forward models
    // For now, just log the training
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Simulate learning: store the observation as a new example
    // This would update internal forward models in a real implementation
    
    NLM_LOG_DEBUG(std::string("PredictionSystem trained on observation with ") +
                  std::to_string(data.size()) + " dimensions, error=") +
                  std::to_string(pImpl->predictionError));
}

} // namespace nlm
