#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Sequence learning state
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    // Pattern neurons (one per unique sensory pattern cluster)
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Recent states for sequence learning
    std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
    
    // Action-consequence learning
    std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
    std::vector<float> actionRewards;
    
    Impl() : brain(nullptr) {}
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
    
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    
    // Keep only recent states
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.erase(recentSensoryStates_.begin());
        stateTimestamps_.erase(stateTimestamps_.begin());
    }
    
    // Learn sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching learned sequence
    const auto& currentState = recentSensoryStates_.back();
    
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
        predictionError_ = 1.0f - predictionError_;  // Convert to error
    }
    
    errorHistory_.push_back(predictionError_);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
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
    
    for (size_t i = 0; i < recentActions_.size(); ++i) {
        if (recentActions_[i].first == action) {
            float sim = computeSimilarity(currentState, recentActions_[i].second);
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
    
    auto currentPred = recentSensoryStates_.empty() ? 
                       std::vector<float>() : recentSensoryStates_.back();
    
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
    if (recentSensoryStates_.empty()) return;
    
    recentActions_.push_back({action, recentSensoryStates_.back()});
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
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
            // Update strength
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
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

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void NeuralPrediction::applyPredictionErrorToNeurons(float error) {
    if (!brain_ || error <= 0.0f) return;
    
    // Apply prediction error to relevant neural populations
    // Error modulates synaptic efficacy and neural firing rates
    for (const auto& region : brain_->getRegions()) {
        for (auto* neuron : region->getAllNeurons()) {
            // Apply error-modulated plasticity to prediction-related synapses
            if (isPredictionNeuron(neuron->getId())) {
                // Error strengthens synapses for better prediction
                float plasticity = error * 0.1f;
                neuron->injectCurrent(plasticity);
                
                // Modulate synaptic weights based on error
                auto synapses = region->getSynapsesFrom(neuron->getId());
                for (auto* syn : synapses) {
                    if (syn->getDestinationNeuron() != neuron->getId()) {
                        float weight = syn->getWeight();
                        float delta = plasticity * (1.0f - std::abs(weight));
                        syn->addToWeight(delta);
                    }
                }
            }
        }
    }
}

bool NeuralPrediction::isPredictionNeuron(NeuronId neuronId) const {
    for (const auto& id : predictionNeurons_) {
        if (id == neuronId) return true;
    }
    return false;
}

ActionType NeuralPrediction::selectActionBasedOnPredictions(const std::vector<float>& currentState) {
    if (!brain_ || recentActions_.empty()) return ActionType::Wait;
    
    // Find most likely action based on current state
    float bestScore = 0.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& actionRecord : recentActions_) {
        float similarity = computeSimilarity(currentState, actionRecord.second);
        
        // Weight by error history
        float errorWeight = 1.0f;
        if (!errorHistory_.empty()) {
            errorWeight = errorHistory_.back();
        }
        
        float score = similarity * errorWeight;
        if (score > bestScore) {
            bestScore = score;
            bestAction = actionRecord.first;
        }
    }
    
    return bestAction;
}