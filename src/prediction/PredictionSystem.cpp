#include "PredictionSystem.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<float> neuralPattern;
    std::vector<float> predictionWeights;
    std::vector<float> errorWeights;
    
    // Integration with other systems
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 2 IMPLEMENTATION: Real neural prediction
    // Use neural patterns from the brain to predict future state
    
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem: Brain not set, returning placeholder prediction");
        return currentState.clone();
    }
    
    // Get current neural activity patterns
    std::vector<float> currentPattern;
    
    // Try to get working memory content first (most immediate prediction)
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        currentPattern = workingMemory->retrieve();
    }
    
    // If working memory is empty, get average neural activity
    if (currentPattern.empty() || currentPattern.size() < 10) {
        // Extract neural activity patterns from the brain
        currentPattern.clear();
        
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                float avgActivity = pop->getAverageFiringRate();
                currentPattern.push_back(avgActivity);
            }
        }
        
        // If still empty, use internal signals
        if (currentPattern.empty()) {
            InternalSignals internalSignals;
            for (size_t i = 0; i < 100; ++i) {
                internalSignals.addSignal(0.5f + 0.3f * sin(i * 0.1f));
            }
            currentPattern = internalSignals.getData();
        }
    }
    
    // Create prediction based on neural pattern
    pImpl->neuralPattern = currentPattern;
    
    // Apply neural prediction dynamics
    // Prediction shifts based on current neural state
    std::vector<float> predictedPattern = currentPattern;
    
    // Add temporal dynamics (simple integration)
    for (size_t i = 0; i < predictedPattern.size(); ++i) {
        // Predict slight change based on current activity
        float change = (predictedPattern[i] * 0.1f) - (0.05f * (i % 10) / 10.0f);
        predictedPattern[i] += change;
        
        // Clamp to reasonable range
        predictedPattern[i] = std::max(0.0f, std::min(1.0f, predictedPattern[i]));
    }
    
    // Create InternalSignals as the predicted state
    auto predictedInput = std::make_unique<InternalSignals>();
    predictedInput->setData(predictedPattern);
    
    // Store for learning
    pImpl->predictionWeights = currentPattern;
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                        const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // Calculate error between predicted and actual neural states
    
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
        
        // Update error weights based on prediction error
        if (!pImpl->errorWeights.empty() && pImpl->errorWeights.size() == predData.size()) {
            for (size_t i = 0; i < predData.size(); ++i) {
                float error = predData[i] - actualData[i];
                pImpl->errorWeights[i] = std::min(1.0f, pImpl->errorWeights[i] + 0.1f * std::abs(error));
            }
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
    pImpl->errorWeights.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // PHASE 2: Real neural training of prediction system
    // Learn from actual observations to improve predictions
    
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem: Brain not set, cannot train");
        return;
    }
    
    const auto& obsData = observation.getData();
    if (obsData.empty()) return;
    
    // Get current neural state
    std::vector<float> currentNeuralState;
    
    // Try multiple sources for neural state
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        currentNeuralState = workingMemory->retrieve();
    }
    
    if (currentNeuralState.empty() || currentNeuralState.size() < 10) {
        // Extract from brain's neural populations
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                float activity = pop->getAverageFiringRate();
                currentNeuralState.push_back(activity);
            }
        }
    }
    
    if (currentNeuralState.empty()) return;
    
    // Ensure consistent dimensions
    if (currentNeuralState.size() != obsData.size()) {
        // Resample observation to match neural state size
        std::vector<float> resizedObs(obsData.size() * currentNeuralState.size() / obsData.size());
        if (!resizedObs.empty()) {
            for (size_t i = 0; i < currentNeuralState.size(); ++i) {
                size_t idx = i % resizedObs.size();
                currentNeuralState[i] = (currentNeuralState[i] + resizedObs[idx]) / 2.0f;
            }
        }
    }
    
    // Update prediction weights using Hebbian-like learning
    // Stronger connections when prediction is accurate
    float accuracy = 1.0f - std::min(1.0f, pImpl->predictionError * 10.0f);
    
    if (pImpl->predictionWeights.empty()) {
        pImpl->predictionWeights = currentNeuralState;
    } else {
        // Hebbian update: weight increases with correlation
        for (size_t i = 0; i < std::min(currentNeuralState.size(), pImpl->predictionWeights.size()); ++i) {
            float correlation = currentNeuralState[i] * pImpl->predictionWeights[i];
            pImpl->predictionWeights[i] += 0.01f * accuracy * correlation;
            
            // Clamp weights
            pImpl->predictionWeights[i] = std::max(0.0f, std::min(1.0f, pImpl->predictionWeights[i]));
        }
    }
    
    // Update confidence based on prediction accuracy
    pImpl->confidence = (pImpl->confidence * 0.9f) + (accuracy * 0.1f);
    
    // Also train episodic memory through prediction errors
    if (auto* episodicMemory = pImpl->brain->getEpisodicMemory()) {
        // Store the observation as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0; // Would need simulation step
        episode.sensoryState = obsData;
        episode.resultingSensoryState = currentNeuralState;
        episode.reward = accuracy * 0.5f;  // Positive reward for good predictions
        
        episodicMemory->storeEpisode(episode);
    }
    
    NLM_LOG_INFO("PredictionSystem: Trained with accuracy " + std::to_string(accuracy));
}

} // namespace nlm
