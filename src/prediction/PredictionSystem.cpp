#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Forward model: state -> next state
    std::vector<std::pair<std::vector<float>, NeuronId>> forwardModel;
    
    // Inverse model: state -> action
    std::vector<std::pair<std::vector<float>, ActionType>> inverseModel;
    
    // Pattern neurons for sensory representations
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Temporal sequence memory for predictive coding
    std::vector<std::vector<float>> temporalSequenceMemory;
    std::vector<float> sequenceTimestamps;
    
    // Learning weights
    std::vector<std::pair<NeuronId, float>> forwardWeights;
    std::vector<std::pair<NeuronId, float>> inverseWeights;
    
    // Prediction error signal for neuromodulation
    float predictionErrorSignal;
    float predictionErrorLearningRate;
    
    // Attention modulation from prediction error
    float attentionModulation;
    
    Impl() : brain(nullptr), predictionErrorSignal(0.0f), 
             predictionErrorLearningRate(0.01f), attentionModulation(0.0f) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , predictionErrorSignal_(0.0f)
    , brain_(nullptr)
    , predictionHorizon_(1)
    , temporalMemorySize_(10)
    , errorSensitivity_(1.0f)
    , surpriseThreshold_(0.3f)
    , predictiveCodingEnabled_(true)
    , forwardModelEnabled_(true)
    , inverseModelEnabled_(true)
{
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("NeuralPrediction initialized with forward and inverse models");
}

float NeuralPrediction::processSensoryState(const SensoryInput& currentState) {
    if (!predictiveCodingEnabled_ || !brain_) {
        return 0.0f;
    }
    
    // Generate prediction using forward model
    auto predicted = computeForwardPrediction(currentState);
    
    if (!predicted) {
        predictionError_ = 0.0f;
        predictionConfidence_ = 0.0f;
        return predictionError_;
    }
    
    // Compute prediction error
    predictionError_ = computePredictionError(*predicted, currentState);
    
    // Update confidence based on error
    if (predictionError_ < 0.1f) {
        predictionConfidence_ = 1.0f - predictionError_;
    } else {
        predictionConfidence_ = 0.1f;
    }
    
    // Update error history
    errorHistory_.push_back(predictionError_);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    // Update prediction models with error
    updateModels(predictionError_, currentState);
    
    // Update prediction error signal for neuromodulation
    updateErrorSignal(predictionError_);
    
    // Integrate with working memory for pattern completion
    integrateWithWorkingMemory(*predicted, predictionConfidence_);
    
    // Modulate attention based on prediction error
    modulateAttentionWithPredictionError(predictionError_);
    
    // Apply prediction error to enhance learning in other systems
    if (pImpl->brain) {
        applyPredictionErrorToLearning(predictionError_, pImpl->brain);
    }
    
    return predictionError_;
}

std::unique_ptr<SensoryInput> NeuralPrediction::predictNextState(const SensoryInput& currentState) {
    if (!forwardModelEnabled_ || !brain_) {
        return currentState.clone();
    }
    
    return computeForwardPrediction(currentState);
}

void NeuralPrediction::updateModels(float predictionError, 
                                   const SensoryInput& actualState) {
    if (!brain_) return;
    
    const auto& currentData = actualState.getData();
    
    // If error is low, reinforce forward model associations
    if (predictionError < 0.1f) {
        // Learn forward model from successful prediction
        learnForwardModel(actualState, actualState);  // In real scenario, would use predicted state
    } else {
        // High error = unexpected state, strengthen inverse model
        if (inverseModelEnabled_) {
            // Learn inverse model for surprising states
            // This would be based on actual actions taken
        }
    }
    
    // Update pattern neurons with new information
    NeuronId patternNeuron = findMatchingPatternNeuron(currentData);
    if (patternNeuron != INVALID_NEURON_ID) {
        updatePatternRepresentation(patternNeuron, currentData);
    }
}

void NeuralPrediction::learnForwardModel(const SensoryInput& currentState,
                                        const SensoryInput& nextState) {
    const auto& currentData = currentState.getData();
    const auto& nextData = nextState.getData();
    
    // Find or create pattern neuron for current state
    NeuronId currentNeuron = findMatchingPatternNeuron(currentData);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextData);
    
    if (currentNeuron != INVALID_NEURON_ID && nextNeuron != INVALID_NEURON_ID) {
        // Create or strengthen forward model association
        createForwardModelAssociation(currentNeuron, nextNeuron, 0.8f);
    }
}

void NeuralPrediction::learnInverseModel(const SensoryInput& sensoryState,
                                       ActionType action) {
    const auto& stateData = sensoryState.getData();
    
    // Find pattern neuron for this state
    NeuronId patternNeuron = findMatchingPatternNeuron(stateData);
    
    if (patternNeuron != INVALID_NEURON_ID) {
        // Create or strengthen inverse model association
        createInverseModelAssociation(patternNeuron, action, 0.7f);
    }
}

std::unique_ptr<SensoryInput> NeuralPrediction::computeForwardPrediction(
    const SensoryInput& currentState) const {
    if (!brain_) return currentState.clone();
    
    const auto& currentData = currentState.getData();
    
    // Find most similar state pattern we've learned
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentData, pImpl->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = pImpl->patternNeurons[i];
        }
    }
    
    // If we have a good match, predict what comes next based on learned sequences
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        // Find what we associate with this pattern in forward model
        for (const auto& assoc : pImpl->forwardModel) {
            if (assoc.first == bestMatch) {
                // Found association - return the associated pattern
                for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                    if (pImpl->patternNeurons[i] == assoc.second) {
                        // Create new sensory input with predicted data
                        auto predicted = std::make_unique<SensoryInput>();
                        // In real implementation, would set the data from patternRepresentations
                        return predicted;
                    }
                }
            }
        }
    }
    
    // No good prediction - return current state as baseline
    return currentState.clone();
}

ActionType NeuralPrediction::computeInversePrediction(
    const SensoryInput& sensoryState) const {
    if (!inverseModelEnabled_ || !brain_) {
        return ActionType::Wait;  // Default action
    }
    
    const auto& stateData = sensoryState.getData();
    
    // Find pattern neuron for this state
    NeuronId patternNeuron = INVALID_NEURON_ID;
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float sim = computeSimilarity(stateData, pImpl->patternRepresentations[i]);
        if (sim > 0.7f) {
            patternNeuron = pImpl->patternNeurons[i];
            break;
        }
    }
    
    if (patternNeuron == INVALID_NEURON_ID) {
        return ActionType::Wait;
    }
    
    // Find associated action in inverse model
    for (const auto& assoc : pImpl->inverseModel) {
        if (assoc.first == patternNeuron) {
            return assoc.second;
        }
    }
    
    return ActionType::Wait;
}

void NeuralPrediction::updateErrorSignal(float error, float learningRate) {
    pImpl->predictionErrorSignal = std::min(1.0f, error * pImpl->predictionErrorLearningRate);
    
    // Apply to neuromodulatory systems
    if (pImpl->brain) {
        // Apply prediction error signal to dopamine system
        if (pImpl->brain->getPredictionError()) {
            float modulation = pImpl->predictionErrorSignal * 2.0f - 1.0f;
            pImpl->brain->getPredictionError()->setPredictionError(modulation);
        }
        
        // Apply to acetylcholine system for attention
        if (pImpl->brain->getAcetylcholine()) {
            pImpl->brain->getAcetylcholine()->setLevel(pImpl->predictionErrorSignal);
        }
        
        // Apply to norepinephrine for salience
        if (pImpl->brain->getNorepinephrine()) {
            pImpl->brain->getNorepinephrine()->setLevel(pImpl->predictionErrorSignal * 0.5f);
        }
    }
}

void NeuralPrediction::integrateWithWorkingMemory(const SensoryInput& predictedState,
                                                 float predictionConfidence) {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) {
        return;
    }
    
    // Store prediction in working memory if confidence is high
    if (predictionConfidence > 0.5f) {
        const auto& predictedData = predictedState.getData();
        
        // Get or create working memory neurons for prediction
        std::vector<NeuronId> memoryNeurons = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
        
        if (!memoryNeurons.empty()) {
            // Store prediction in first available memory neuron
            pImpl->brain->getWorkingMemory()->storeToNeuron(
                memoryNeurons[0], predictionConfidence);
        }
    }
}

void NeuralPrediction::modulateAttentionWithPredictionError(float error) {
    pImpl->attentionModulation = std::min(1.0f, error * 2.0f);
    
    // Apply attention modulation if attention system exists
    if (pImpl->brain && pImpl->brain->getAttention()) {
        if (error > 0.5f) {
            // High prediction error increases attention saliency
            auto winners = pImpl->brain->getAttention()->getWinners();
            if (!winners.empty()) {
                // Boost attention to current processing
                pImpl->brain->getAttention()->applyBottomUpSalience(
                    winners[0], pImpl->attentionModulation);
            }
        }
    }
}

void NeuralPrediction::applyPredictionErrorToLearning(float error, Brain* brain) {
    // Apply prediction error to development system for adaptive plasticity
    if (brain->getDevelopmentSystem()) {
        // High prediction error increases plasticity for learning
        if (error > 0.3f) {
            // This would apply to developmental stage and plasticity modulation
            // In a real implementation, this would modify developmental parameters
        }
    }
    
    // Apply to episodic memory for error-based learning
    if (brain->getEpisodicMemory()) {
        // Store prediction error as learning signal in memory
        // This would be integrated into memory consolidation
    }
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    temporalSequenceMemory_.clear();
    sequenceTimestamps_.clear();
    pImpl->forwardModel.clear();
    pImpl->inverseModel.clear();
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> result;
    result.insert(result.end(), pImpl->patternNeurons.begin(), pImpl->patternNeurons.end());
    result.insert(result.end(), forwardModelNeurons_.begin(), forwardModelNeurons_.end());
    return result;
}

std::vector<NeuronId> NeuralPrediction::getForwardModelNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->forwardModel) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

std::vector<NeuronId> NeuralPrediction::getInverseModelNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->inverseModel) {
        result.push_back(assoc.first);
    }
    return result;
}

float NeuralPrediction::getPredictionErrorSignal() const {
    return predictionErrorSignal_;
}

// Helper methods
float NeuralPrediction::computePredictionError(const SensoryInput& predicted,
                                              const SensoryInput& actual) const {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return 1.0f;
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    
    return std::sqrt(sumError / predData.size());
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                    float similarityThreshold) {
    // Check if we already have a similar pattern
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(pattern, pImpl->patternRepresentations[i]);
        if (sim >= similarityThreshold) {
            return pImpl->patternNeurons[i];
        }
    }
    
    // Create new pattern neuron
    if (brain_) {
        NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 10000);
        pImpl->patternNeurons.push_back(newId);
        pImpl->patternRepresentations.push_back(pattern);
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

void NeuralPrediction::updatePatternRepresentation(NeuronId neuronId,
                                                  const std::vector<float>& pattern) {
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        if (pImpl->patternNeurons[i] == neuronId) {
            // Update with running average
            float alpha = 0.1f;  // Learning rate
            for (size_t j = 0; j < pattern.size() && j < pImpl->patternRepresentations[i].size(); ++j) {
                pImpl->patternRepresentations[i][j] = 
                    (1.0f - alpha) * pImpl->patternRepresentations[i][j] + 
                    alpha * pattern[j];
            }
            break;
        }
    }
}

void NeuralPrediction::createForwardModelAssociation(NeuronId from, NeuronId to, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->forwardModel) {
        if (assoc.first == from && assoc.second == to) {
            return;
        }
    }
    
    // Create new association
    pImpl->forwardModel.emplace_back(from, to);
    pImpl->forwardWeights.emplace_back(to, strength);
}

void NeuralPrediction::createInverseModelAssociation(NeuronId pattern, ActionType action, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->inverseModel) {
        if (assoc.first == pattern && assoc.second == action) {
            return;
        }
    }
    
    // Create new association
    pImpl->inverseModel.emplace_back(pattern, action);
    pImpl->inverseWeights.emplace_back(pattern, strength);
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
