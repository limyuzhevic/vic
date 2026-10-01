#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Sequence learning state
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    // Pattern neurons (one per unique sensory pattern cluster)
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Recent states for sequence learning (moved to Impl)
    std::deque<std::vector<float>> recentSensoryStates;
    std::deque<SimulationStep> stateTimestamps;
    
    // Action-consequence learning
    std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
    std::vector<float> actionRewards;
    
    // Additional state for action consequences
    std::vector<std::pair<ActionType, std::vector<float>>> recentActions;
    
    // For prediction error calculation
    std::vector<float> errorHistory;
    
    // RNG for sequence generation
    std::random_device rd;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), rng(rd()) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
{
    // Initialize error history
    pImpl->errorHistory.resize(1000, 0.0f);
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("NeuralPrediction initialized");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                         SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    pImpl->recentSensoryStates.push_back(sensoryState);
    pImpl->stateTimestamps.push_back(currentStep);
    
    // Keep only recent states
    if (pImpl->recentSensoryStates.size() > sequenceMemorySize_) {
        pImpl->recentSensoryStates.pop_front();
        pImpl->stateTimestamps.pop_front();
    }
    
    // Learn sequence if we have at least 2 states
    if (pImpl->recentSensoryStates.size() >= 2) {
        learnTemporalSequence(pImpl->recentSensoryStates[pImpl->recentSensoryStates.size() - 2],
                            pImpl->recentSensoryStates.back(),
                            currentStep);
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || pImpl->recentSensoryStates.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching learned sequence
    const auto& currentState = pImpl->recentSensoryStates.back();
    
    // Find most similar pattern we've seen
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = pImpl->patternNeurons[i];
        }
    }
    
    // If we have a good match, predict what comes next
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        // Find what we associate with this pattern
        for (const auto& assoc : pImpl->sequenceAssociations) {
            if (assoc.first == bestMatch) {
                // Found association - return the associated pattern
                for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                    if (pImpl->patternNeurons[i] == assoc.second) {
                        predictionConfidence_ = bestSimilarity;
                        return pImpl->patternRepresentations[i];
                    }
                }
            }
        }
    }
    
    // No good prediction - return current state as baseline
    predictionConfidence_ = 0.0f;
    return currentState;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState, 
                                             SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    // Compute error
    if (predicted.empty()) {
        predictionError_ = 0.0f;
    } else {
        predictionError_ = computeSimilarity(predicted, actualState);
        predictionError_ = 1.0f - predictionError_;  // Convert to error (0 = perfect)
    }
    
    pImpl->errorHistory.push_back(predictionError_);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Modulate learning based on error
    float reward = 1.0f - predictionError_;
    modulatePredictionSynapses(predictionError_, reward);
    
    return predictionError_;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Find experiences with same action and similar state
    float bestMatch = 0.0f;
    std::vector<float> bestConsequence;
    
    for (size_t i = 0; i < pImpl->recentActions.size(); ++i) {
        if (pImpl->recentActions[i].first == action) {
            float sim = computeSimilarity(currentState, pImpl->recentActions[i].second);
            if (sim > bestMatch) {
                bestMatch = sim;
                if (i < pImpl->actionConsequences.size()) {
                    bestConsequence = pImpl->actionConsequences[i].second;
                }
            }
        }
    }
    
    if (bestMatch > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;  // Default: no change
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                        size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    std::vector<float> currentPred = pImpl->recentSensoryStates.empty() ? 
                                       std::vector<float>() : pImpl->recentSensoryStates.back();
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (currentPred.empty()) break;
        
        predictions.push_back(currentPred);
        
        // Use current prediction to generate next
        // (simple approach - in reality this would use chained predictions)
        auto nextPred = generatePrediction(currentStep + step);
        if (nextPred.empty()) break;
        currentPred = nextPred;
    }
    
    return predictions;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    if (pImpl->recentSensoryStates.empty()) return;
    
    pImpl->recentActions.push_back({action, pImpl->recentSensoryStates.back()});
    if (pImpl->recentActions.size() > 100) {
        pImpl->recentActions.erase(pImpl->recentActions.begin());
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Find or create pattern neuron for current state
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron != nextNeuron) {
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
    }
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

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    if (from == INVALID_NEURON_ID || to == INVALID_NEURON_ID) return;
    
    // Check if association already exists
    for (auto& assoc : pImpl->sequenceAssociations) {
        if (assoc.first == from && assoc.second == to) {
            // Association already exists
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a, 
                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / std::sqrt(normA * normB);
    }
    
    return 0.0f;
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Real implementation would modulate synaptic weights based on prediction error
    // This is a placeholder for the actual neural substrate implementation
    
    // Stronger errors lead to more weight changes
    float modulation = error * reward;  // Could be positive or negative
    
    if (std::abs(modulation) > 0.1f) {
        NLM_LOG_INFO("Prediction error modulation: error=" + std::to_string(error) +
                    " reward=" + std::to_string(reward) +
                    " modulation=" + std::to_string(modulation));
    }
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    return pImpl->patternNeurons;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> sequenceNeurons;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        sequenceNeurons.push_back(assoc.first);
        sequenceNeurons.push_back(assoc.second);
    }
    return sequenceNeurons;
}

void NeuralPrediction::clearHistory() {
    pImpl->recentSensoryStates.clear();
    pImpl->stateTimestamps.clear();
    pImpl->recentActions.clear();
    pImpl->errorHistory.clear();
    pImpl->patternNeurons.clear();
    pImpl->patternRepresentations.clear();
    pImpl->sequenceAssociations.clear();
    NLM_LOG_INFO("NeuralPrediction history cleared");
}

} // namespace nlm
