#include "NeuralPredictionModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralPredictionModel::Impl {
    Brain* brain;
    
    // Forward model weights (prediction connections)
    // Maps from current neural activity to predicted next state
    std::vector<std::vector<float>> forwardWeights;
    
    // Recurrence connections for maintaining predictions over time
    std::vector<std::vector<float>> recurrenceWeights;
    
    // Current prediction (neural activity pattern)
    std::vector<float> currentPrediction;
    
    // Previous prediction (for computing error)
    std::vector<float> previousPrediction;
    
    // Prediction confidence based on neural activity stability
    float predictionConfidence;
    
    // Reward history for prediction updates
    std::vector<float> rewardHistory;
    
    // STDP eligibility traces for prediction weights
    std::vector<std::vector<float>> eligibilityTraces;
    
    // Neural memory references
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    
    // Neural populations for prediction
    std::vector<PopulationId> predictionPopulations;
    
    Impl() : brain(nullptr), predictionConfidence(0.5f), workingMemory(nullptr), episodicMemory(nullptr) {}
};

NeuralPredictionModel::NeuralPredictionModel()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralPredictionModel::~NeuralPredictionModel() = default;

void NeuralPredictionModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Get memory systems
    pImpl->workingMemory = brain->getWorkingMemory();
    pImpl->episodicMemory = brain->getEpisodicMemory();
    
    NLM_LOG_INFO("NeuralPredictionModel initialized");
}

void NeuralPredictionModel::setupForwardConnections(const std::vector<float>& currentPattern) {
    if (!brain_ || currentPattern.empty()) return;
    
    // Create forward model weights based on neural population activity
    // Use working memory patterns to inform predictions
    
    // Get current working memory content
    std::vector<float> workingMemoryContent = pImpl->workingMemory->retrieve();
    
    // Determine prediction dimensionality from working memory
    size_t predDim = std::max(currentPattern.size(), workingMemoryContent.size());
    
    // Initialize forward weights
    pImpl->forwardWeights.clear();
    pImpl->forwardWeights.resize(predDim, std::vector<float>(predDim, 0.0f));
    
    // Set up forward connections based on neural patterns
    for (size_t i = 0; i < predDim; ++i) {
        float sourceActivity = (i < currentPattern.size()) ? currentPattern[i] : 0.0f;
        float memoryActivity = (i < workingMemoryContent.size()) ? workingMemoryContent[i] : 0.0f;
        
        // Encode source activity into prediction
        for (size_t j = 0; j < predDim; ++j) {
            // Forward weight from source to prediction
            pImpl->forwardWeights[i][j] = sourceActivity * memoryActivity * 0.1f;
            
            // Also include recurrence connections
            pImpl->recurrenceWeights[i][j] = 0.0f;  // Will be updated during training
        }
    }
}

std::vector<float> NeuralPredictionModel::predictNextState(const std::vector<float>& currentPattern) const {
    std::vector<float> prediction(currentPattern.size(), 0.0f);
    
    if (currentPattern.empty() || !brain_) {
        return prediction;
    }
    
    // Get current working memory to bias prediction
    std::vector<float> workingMemoryContent = pImpl->workingMemory->retrieve();
    
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        float currentActivity = currentPattern[i];
        float memoryActivity = (i < workingMemoryContent.size()) ? workingMemoryContent[i] : 0.0f;
        
        // Get input from current neural activity
        float input = currentActivity * 0.7f + memoryActivity * 0.3f;
        
        // Apply forward model (simplified neural computation)
        // In real implementation, this would involve actual neural population dynamics
        prediction[i] = input * 0.8f + (i > 0 ? prediction[i-1] * 0.2f : 0.0f);
        
        // Clip to valid range
        prediction[i] = std::max(0.0f, std::min(1.0f, prediction[i]));
    }
    
    return prediction;
}

float NeuralPredictionModel::computeNeuralPredictionError(const std::vector<float>& predicted,
                                                        const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 1.0f;  // Max error if dimensions don't match
    }
    
    // Compute prediction error using neural dynamics
    // Combines activity-based error with reward prediction error
    float activityError = 0.0f;
    float rewardError = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        activityError += diff * diff;
        
        // Also compute reward prediction error component
        // Reward prediction uses dopamine signals from brain
        if (pImpl->brain) {
            auto* dopamine = pImpl->brain->getDopamine();
            if (dopamine) {
                float dopamineLevel = dopamine->getLevel();
                rewardError += std::abs(dopamineLevel - predicted[i]);
            }
        }
    }
    
    activityError /= predicted.size();
    rewardError /= predicted.size();
    
    // Combine errors
    float totalError = std::sqrt(activityError) * 0.7f + rewardError * 0.3f;
    
    return std::max(0.0f, std::min(1.0f, totalError));
}

void NeuralPredictionModel::train(const std::vector<float>& currentPattern,
                                 const std::vector<float>& actualPattern) {
    if (!brain_ || currentPattern.empty() || actualPattern.empty()) {
        return;
    }
    
    // Get prediction for current pattern
    std::vector<float> predicted = predictNextState(currentPattern);
    
    // Compute prediction error
    float error = computeNeuralPredictionError(predicted, actualPattern);
    
    // Get current working memory to influence weight updates
    std::vector<float> workingMemory = pImpl->workingMemory->retrieve();
    
    // Update forward weights using STDP-like learning
    if (!pImpl->forwardWeights.empty() && !pImpl->forwardWeights[0].empty()) {
        size_t dim = pImpl->forwardWeights.size();
        
        // Compute weight changes based on prediction error
        for (size_t i = 0; i < dim; ++i) {
            float currentActivity = (i < currentPattern.size()) ? currentPattern[i] : 0.0f;
            float targetActivity = (i < actualPattern.size()) ? actualPattern[i] : 0.0f;
            
            // Calculate desired weight change
            float weightDelta = (targetActivity - predicted[i]) * error * 0.01f;
            
            // Apply weight changes with eligibility traces
            for (size_t j = 0; j < dim; ++j) {
                // Update forward weight
                pImpl->forwardWeights[i][j] += weightDelta * currentActivity;
                
                // Clip weights
                pImpl->forwardWeights[i][j] = std::max(0.0f, std::min(1.0f, pImpl->forwardWeights[i][j]));
                
                // Update eligibility trace
                pImpl->eligibilityTraces[i][j] = pImpl->eligibilityTraces[i][j] * 0.99f + weightDelta * 0.01f;
            }
        }
    }
    
    // Update working memory with successful predictions
    if (error < 0.3f) {  // Low error indicates successful prediction
        float reward = 1.0f - error;
        updateWithReward(reward);
        
        // Store prediction in working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->store(predicted, reward * 0.5f);
        }
    }
    
    // Store in episodic memory if prediction was good
    if (error < 0.2f) {  // Very good prediction
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->brain->getTotalSpikeCount();
            episode.sensoryState = currentPattern;
            episode.resultingSensoryState = actualPattern;
            episode.reward = 1.0f - error;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void NeuralPredictionModel::updateWithReward(float reward) {
    pImpl->rewardHistory.push_back(reward);
    
    // Keep history manageable
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Update prediction confidence based on reward history
    if (!pImpl->rewardHistory.empty()) {
        float avgReward = 0.0f;
        for (float r : pImpl->rewardHistory) avgReward += r;
        avgReward /= pImpl->rewardHistory.size();
        
        pImpl->predictionConfidence = avgReward;
        pImpl->predictionConfidence = std::max(0.1f, std::min(1.0f, pImpl->predictionConfidence));
    }
}

float NeuralPredictionModel::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

const std::vector<float>& NeuralPredictionModel::getCurrentPrediction() const {
    return pImpl->currentPrediction;
}

void NeuralPredictionModel::clear() {
    pImpl->forwardWeights.clear();
    pImpl->recurrenceWeights.clear();
    pImpl->currentPrediction.clear();
    pImpl->previousPrediction.clear();
    pImpl->predictionConfidence = 0.5f;
    pImpl->rewardHistory.clear();
    pImpl->eligibilityTraces.clear();
}

} // namespace nlm
