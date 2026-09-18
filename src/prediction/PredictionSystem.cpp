#include "PredictionSystem.hpp"
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
    float predictionErrorSignal;
    float predictionConfidence;
    std::vector<float> predictionHistory;
    
    // Integration with brain systems
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             predictionErrorSignal(0.0f), predictionConfidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 6: Implement real prediction using NLM's neural substrate
    // Use concept formation and neural patterns to predict next sensory state
    
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem: Brain not set, returning current state");
        return currentState.clone();
    }
    
    // Get current sensory patterns from brain
    auto* conceptFormation = pImpl->brain->getConceptFormation();
    if (!conceptFormation) {
        NLM_LOG_WARNING("PredictionSystem: ConceptFormation not available, returning current state");
        return currentState.clone();
    }
    
    // Use most common concept prototype as prediction
    size_t conceptCount = conceptFormation->getConceptCount();
    if (conceptCount == 0) {
        NLM_LOG_WARNING("PredictionSystem: No concepts learned, returning current state");
        return currentState.clone();
    }
    
    // Get the most stable concept
    size_t mostStableConcept = 0;
    float maxStability = 0.0f;
    for (size_t i = 1; i <= conceptCount; ++i) {
        float stability = conceptFormation->getConceptStability(i);
        if (stability > maxStability) {
            maxStability = stability;
            mostStableConcept = i;
        }
    }
    
    // Create predicted sensory input based on concept prototype
    const std::vector<float>& prototype = conceptFormation->getConceptPrototype(mostStableConcept);
    
    // Create new sensory input with predicted values
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // Add some noise to make prediction realistic but not deterministic
    float noise = pImpl->predictionError * 0.1f;  // Error affects prediction uncertainty
    for (size_t i = 0; i < prototype.size() && i < currentState.getData().size(); ++i) {
        float value = prototype[i];
        
        // Add prediction-specific noise
        float variation = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * noise;
        value += variation;
        
        // Apply concept stability to prediction confidence
        float confidenceFactor = 0.8f + 0.2f * maxStability;  // Higher stability = more confidence
        value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * (1.0f - confidenceFactor);
        
        predictedInput->addValue(value);
    }
    
    // Update prediction confidence
    pImpl->predictionConfidence = maxStability * 0.8f + 0.2f;
    
    NLM_LOG_INFO("PredictionSystem: Generated prediction using concept " + 
                 std::to_string(mostStableConcept) + " with confidence " + 
                 std::to_string(pImpl->predictionConfidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 6: Implement real prediction error computation using neural dynamics
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using neural computation
        float sumSquaredError = 0.0f;
        float maxAbsoluteError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
            maxAbsoluteError = std::max(maxAbsoluteError, std::abs(diff));
        }
        
        // Convert to prediction error signal (normalized)
        pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
        pImpl->predictionErrorSignal = maxAbsoluteError * 10.0f;  // Scale for neural effect
        
        // Update prediction history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        pImpl->predictionHistory.push_back(pImpl->predictionErrorSignal);
        
        // Update confidence based on prediction accuracy
        if (pImpl->predictionError < 0.1f) {
            pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + 0.05f);
        } else if (pImpl->predictionError > 0.5f) {
            pImpl->predictionConfidence = std::max(0.1f, pImpl->predictionConfidence - 0.1f);
        }
        
        NLM_LOG_INFO("PredictionSystem: Error=" + std::to_string(pImpl->predictionError) + 
                     ", Signal=" + std::to_string(pImpl->predictionErrorSignal) +
                     ", Confidence=" + std::to_string(pImpl->predictionConfidence));
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
    pImpl->predictionHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // PHASE 6: Train prediction model using neural substrate
    // Update internal predictions based on observed sensory input
    
    // Use concept formation to learn from observations
    if (!pImpl->brain) return;
    
    auto* conceptFormation = pImpl->brain->getConceptFormation();
    if (!conceptFormation) return;
    
    // Extract features from sensory observation
    const auto& data = observation.getData();
    std::vector<float> features = data;
    
    // Simple feature extraction for concept formation
    float reward = 0.0f;  // TODO: Get actual reward from environment
    
    // Let concept formation learn from this observation
    size_t conceptId = conceptFormation->presentExperience(
        features, features, reward, 0);  // Using same pattern as features
    
    if (conceptId > 0) {
        NLM_LOG_INFO("PredictionSystem: Trained on observation, formed concept " + std::to_string(conceptId));
    }
}

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain integration");
}

} // namespace nlm
