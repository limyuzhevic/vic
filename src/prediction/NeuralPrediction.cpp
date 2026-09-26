#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Temporal sequence memory
    std::vector<std::vector<float>> sequenceMemory;
    std::vector<SimulationStep> sequenceTimestamps;
    
    // Pattern neurons
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    
    // Sequence associations
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    // Action-consequence associations
    std::vector<std::tuple<NeuronId, ActionType, std::vector<float>>> actionConsequences;
    
    Impl() : brain(nullptr) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(std::make_unique<Impl>())
    , sequenceMemorySize_(100)
    , predictionHorizon_(10)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.0f)
{
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralPrediction system initialized");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    if (!brain_) return;
    
    // Record state for temporal prediction
    if (sensoryState.size() > 0) {
        pImpl->sequenceMemory.push_back(sensoryState);
        pImpl->sequenceTimestamps.push_back(currentStep);
        
        // Maintain memory size
        if (pImpl->sequenceMemory.size() > sequenceMemorySize_) {
            pImpl->sequenceMemory.erase(pImpl->sequenceMemory.begin());
            pImpl->sequenceTimestamps.erase(pImpl->sequenceTimestamps.begin());
        }
        
        // Learn temporal sequence
        if (pImpl->sequenceMemory.size() >= 2) {
            learnTemporalSequence(sensoryState, pImpl->sequenceMemory[pImpl->sequenceMemory.size() - 2], currentStep);
        }
        
        // Update recent states for prediction
        recentSensoryStates_.push_back(sensoryState);
        if (recentSensoryStates_.size() > 10) {
            recentSensoryStates_.erase(recentSensoryStates_.begin());
        }
        stateTimestamps_.push_back(currentStep);
        if (stateTimestamps_.size() > 10) {
            stateTimestamps_.erase(stateTimestamps_.begin());
        }
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    std::vector<float> prediction;
    
    if (!brain_ || recentSensoryStates_.empty()) {
        return prediction;
    }
    
    // Simple prediction: repeat last state (placeholder for real neural prediction)
    prediction = recentSensoryStates_.back();
    
    // In real implementation, this would:
    // 1. Activate pattern neurons representing the last state
    // 2. Use sequence associations to predict next state
    // 3. Integrate prediction error
    
    return prediction;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState, 
                                           SimulationStep currentStep) {
    float prediction = 0.0f;
    float actual = 0.0f;
    
    if (!recentSensoryStates_.empty()) {
        prediction = recentSensoryStates_.back()[0];  // Use first element as proxy
    }
    
    if (!actualState.empty()) {
        actual = actualState[0];
    }
    
    predictionError_ = std::abs(actual - prediction);
    float error = predictionError_;
    
    // Update confidence based on error
    predictionConfidence_ = std::max(0.0f, 1.0f - predictionError_);
    
    // Record error history
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 100) {
        errorHistory_.pop_front();
    }
    
    // Modulate prediction synapses based on error
    if (temporalPredictionEnabled_) {
        modulatePredictionSynapses(error, actual);
    }
    
    return error;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                            const std::vector<float>& currentState) {
    std::vector<float> predictedConsequence;
    
    if (!brain_ || !actionConsequenceEnabled_) {
        return predictedConsequence;
    }
    
    // Look up action consequence from learned associations
    for (const auto& assoc : pImpl->actionConsequences) {
        NeuronId patternNeuron = std::get<0>(assoc);
        ActionType learnedAction = std::get<1>(assoc);
        std::vector<float> consequence = std::get<2>(assoc);
        
        if (learnedAction == action) {
            // Find matching pattern neuron
            for (const auto& pn : pImpl->patternNeurons) {
                if (pn.first == patternNeuron && 
                    computeSimilarity(currentState, pn.second) > 0.7f) {
                    predictedConsequence = consequence;
                    return predictedConsequence;
                }
            }
        }
    }
    
    // If no learned consequence, predict similar to current state
    predictedConsequence = currentState;
    return predictedConsequence;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    // Record action for action-consequence learning
    std::vector<float> actionPattern;
    
    if (!recentSensoryStates_.empty()) {
        actionPattern = recentSensoryStates_.back();
    }
    
    if (!actionPattern.empty()) {
        pImpl->actionConsequences.emplace_back(
            findPatternNeuron(actionPattern), action, actionPattern
        );
    }
    
    recentActions_.push_back({action, actionPattern});
    if (recentActions_.size() > 50) {
        recentActions_.pop_front();
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& previousState,
                                            SimulationStep currentStep) {
    if (!brain_) return;
    
    // Find or create pattern neurons for these states
    NeuronId fromNeuron = findMatchingPatternNeuron(previousState);
    NeuronId toNeuron = findMatchingPatternNeuron(currentState);
    
    if (fromNeuron != INVALID_NEURON_ID && toNeuron != INVALID_NEURON_ID) {
        // Create sequence association
        createSequenceAssociation(fromNeuron, toNeuron, 0.5f);
    }
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                   float similarityThreshold) {
    // Check if we already have a similar pattern neuron
    for (const auto& pn : pImpl->patternNeurons) {
        if (computeSimilarity(pattern, pn.second) > similarityThreshold) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId = static_cast<NeuronId>(pImpl->patternNeurons.size() + 20000);
    pImpl->patternNeurons.push_back({newId, pattern});
    return newId;
}

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    // Check if association already exists
    for (const auto& assoc : pImpl->sequenceAssociations) {
        if (std::get<0>(assoc) == from && std::get<1>(assoc) == to) {
            return;
        }
    }
    
    pImpl->sequenceAssociations.push_back({from, to});
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // In a real implementation, this would:
    // 1. Find prediction neurons
    // 2. Adjust synaptic weights based on prediction error
    // 3. Use dopamine as learning signal
    
    // Placeholder: do nothing
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> neurons;
    for (const auto& pn : pImpl->patternNeurons) {
        neurons.push_back(pn.first);
    }
    return neurons;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> neurons;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        neurons.push_back(std::get<0>(assoc));
        neurons.push_back(std::get<1>(assoc));
    }
    return neurons;
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->sequenceMemory.clear();
    pImpl->sequenceTimestamps.clear();
    pImpl->patternNeurons.clear();
    pImpl->sequenceAssociations.clear();
    pImpl->actionConsequences.clear();
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                     size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    if (!brain_ || recentSensoryStates_.empty()) {
        return predictions;
    }
    
    // Simple multi-step prediction (placeholder for real neural dynamics)
    std::vector<float> currentState = recentSensoryStates_.back();
    predictions.push_back(currentState);
    
    for (size_t step = 0; step < numSteps; ++step) {
        // Predict next state (placeholder implementation)
        std::vector<float> predictedState = currentState;
        predictions.push_back(predictedState);
        currentState = predictedState;
    }
    
    return predictions;
}

// ActionConsequencePredictor implementation
struct ActionConsequencePredictor::Impl {
    Brain* brain;
    
    // Action-consequence database
    std::vector<std::tuple<ActionType, std::vector<float>, std::vector<float>, float>> experiences;
    
    // Reward estimates
    std::vector<std::pair<ActionType, float>> actionQualities;
    
    Impl() : brain(nullptr) {}
};

ActionConsequencePredictor::ActionConsequencePredictor()
    : pImpl(std::make_unique<Impl>()) {
}

ActionConsequencePredictor::~ActionConsequencePredictor() = default;

void ActionConsequencePredictor::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("ActionConsequencePredictor initialized");
}

void ActionConsequencePredictor::recordExperience(ActionType action,
                                                  const std::vector<float>& beforeState,
                                                  const std::vector<float>& afterState,
                                                  float reward) {
    if (!beforeState.empty() && !afterState.empty()) {
        pImpl->experiences.emplace_back(action, beforeState, afterState, reward);
        
        // Update action quality
        bool found = false;
        for (auto& quality : pImpl->actionQualities) {
            if (quality.first == action) {
                quality.second = (quality.second + reward) * 0.5f;
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->actionQualities.emplace_back(action, reward);
        }
    }
}

std::vector<float> ActionConsequencePredictor::predictConsequence(ActionType action,
                                                                 const std::vector<float>& currentState) {
    std::vector<float> predictedConsequence;
    
    // Find best matching experience
    float bestScore = 0.0f;
    
    for (const auto& exp : pImpl->experiences) {
        ActionType learnedAction = std::get<0>(exp);
        std::vector<float> beforeState = std::get<1>(exp);
        std::vector<float> afterState = std::get<2>(exp);
        float reward = std::get<3>(exp);
        
        if (learnedAction == action && !beforeState.empty()) {
            float similarity = 0.0f;
            if (currentState.size() == beforeState.size() && currentState.size() > 0) {
                similarity = 0.0f;
                for (size_t i = 0; i < currentState.size(); ++i) {
                    similarity += std::abs(currentState[i] - beforeState[i]);
                }
                similarity = 1.0f - similarity / currentState.size();
            }
            
            if (similarity > bestScore) {
                bestScore = similarity;
                predictedConsequence = afterState;
            }
        }
    }
    
    return predictedConsequence;
}

float ActionConsequencePredictor::getConsequenceConfidence(ActionType action,
                                                         const std::vector<float>& currentState) const {
    float confidence = 0.0f;
    size_t matchingExperiences = 0;
    
    for (const auto& exp : pImpl->experiences) {
        ActionType learnedAction = std::get<0>(exp);
        std::vector<float> beforeState = std::get<1>(exp);
        
        if (learnedAction == action && !beforeState.empty()) {
            matchingExperiences++;
            
            if (currentState.size() == beforeState.size()) {
                float similarity = 0.0f;
                for (size_t i = 0; i < currentState.size(); ++i) {
                    similarity += std::abs(currentState[i] - beforeState[i]);
                }
                similarity = 1.0f - similarity / currentState.size();
                confidence += similarity;
            }
        }
    }
    
    if (matchingExperiences > 0) {
        confidence /= matchingExperiences;
    }
    
    return confidence;
}

void ActionConsequencePredictor::updatePrediction(ActionType action,
                                                 const std::vector<float>& predicted,
                                                 const std::vector<float>& actual,
                                                 float reward) {
    // Record new experience
    recordExperience(action, predicted, actual, reward);
    
    // Could also update prediction weights based on error
}

float ActionConsequencePredictor::getActionQuality(ActionType action) const {
    for (const auto& quality : pImpl->actionQualities) {
        if (quality.first == action) {
            return quality.second;
        }
    }
    return 0.0f;
}

void ActionConsequencePredictor::clear() {
    pImpl->experiences.clear();
    pImpl->actionQualities.clear();
}

// PredictionErrorSignal implementation
struct PredictionErrorSignal::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr), lastError_(0.0f), surpriseThreshold_(0.5f) {}
};

PredictionErrorSignal::PredictionErrorSignal()
    : pImpl(std::make_unique<Impl>())
    , lastError_(0.0f)
    , surpriseThreshold_(0.5f) {
}

PredictionErrorSignal::~PredictionErrorSignal() = default;

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                         const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return 0.0f;
    }
    
    if (predicted.size() != actual.size()) {
        lastError_ = 1.0f; // Large error for size mismatch
        return lastError_;
    }
    
    // Compute error as RMS difference
    float sum = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sum += diff * diff;
    }
    
    lastError_ = std::sqrt(sum / predicted.size());
    
    // Update error components
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = lastError_;
    
    // Simple decomposition (placeholder)
    errorComponents_.intensityError = std::abs(actual[0] - predicted[0]);
    if (predicted.size() > 1) {
        errorComponents_.spatialError = std::sqrt(
            (actual[1] - predicted[1]) * (actual[1] - predicted[1]));
    }
    
    errorHistory_.push_back(lastError_);
    if (errorHistory_.size() > 100) {
        errorHistory_.pop_front();
    }
    
    return lastError_;
}

float PredictionErrorSignal::getModulationSignal() const {
    // Convert error to neuromodulation signal (0 = no learning needed, 1 = high learning needed)
    float signal = std::min(1.0f, lastError_ / 0.5f);
    return signal;
}

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    lastError_ = error;
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 100) {
        errorHistory_.pop_front();
    }
}

} // namespace nlm