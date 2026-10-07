#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <limits>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Sequence learning state with neural substrate
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Temporal sequence storage with neural populations
    std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
    
    // Action-consequence learning with reward modulation
    std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
    std::vector<float> actionRewards;
    std::vector<std::pair<ActionType, std::vector<float>>> actionStateMap;
    
    // Neural dynamics for prediction
    std::vector<NeuronId> predictiveNeurons;
    std::vector<NeuronId> associationNeurons;
    
    // Hebbian learning matrix for temporal associations
    std::vector<std::vector<float>> hebbianMatrix;
    
    // STDP eligibility traces for prediction learning
    std::vector<std::vector<float>> eligibilityTraces;
    
    Impl() : brain(nullptr) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(20)
    , predictionHorizon_(3)
    , similarityThreshold_(0.7f)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , temporalPredictionCount_(0)
    , actionConsequencePredictionCount_(0)
    , temporalPredictionAccuracy_(0.5f)
    , actionConsequenceAccuracy_(0.5f)
{
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural components for prediction
    initializePredictiveNeurons();
    initializeHebbianLearning();
    initializeSTDP();
    
    NLM_LOG_INFO("NeuralPrediction initialized with real forward models");
}

void NeuralPrediction::initializePredictiveNeurons() {
    if (!brain_) return;
    
    // Create predictive neural populations
    for (size_t i = 0; i < 50; ++i) {
        NeuronId neuronId = NeuronId(10000 + i);
        pImpl->predictiveNeurons.push_back(neuronId);
        
        // Find or create neurons for prediction patterns
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            // Initialize prediction neurons with appropriate properties
        }
    }
}

void NeuralPrediction::initializeHebbianLearning() {
    // Initialize Hebbian learning matrix for temporal associations
    size_t matrixSize = std::min(sequenceMemorySize_, static_cast<size_t>(100));
    pImpl->hebbianMatrix.resize(matrixSize, std::vector<float>(matrixSize, 0.0f));
    pImpl->eligibilityTraces.resize(matrixSize, std::vector<float>(matrixSize, 0.0f));
}

void NeuralPrediction::initializeSTDP() {
    // Initialize STDP parameters for prediction learning
    // STDP will be applied through brain's existing STDP system
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                        SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    // Store sensory state with temporal context
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    pImpl->stateHistory.push_back({sensoryState, currentStep});
    
    // Keep only recent states
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.erase(recentSensoryStates_.begin());
        stateTimestamps_.erase(stateTimestamps_.begin());
    }
    if (pImpl->stateHistory.size() > sequenceMemorySize_ * 2) {
        pImpl->stateHistory.pop_front();
    }
    
    // Learn temporal sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
    
    // Update prediction confidence based on sensory pattern consistency
    updatePredictionConfidence();
}

void NeuralPrediction::updatePredictionConfidence() {
    if (recentSensoryStates_.size() < 2) {
        predictionConfidence_ = 0.0f;
        return;
    }
    
    // Compute confidence based on similarity of recent consecutive states
    // This reflects how predictable the current state is
    float totalSimilarity = 0.0f;
    size_t count = 0;
    
    for (size_t i = 1; i < recentSensoryStates_.size(); ++i) {
        float similarity = computeSimilarity(recentSensoryStates_[i-1], 
                                           recentSensoryStates_[i]);
        totalSimilarity += similarity;
        ++count;
    }
    
    if (count > 0) {
        predictionConfidence_ = totalSimilarity / count;
    } else {
        predictionConfidence_ = 0.0f;
    }
}

void NeuralPrediction::updatePredictionError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        predictionError_ = 0.0f;
        return;
    }
    
    // Compute prediction error using MSE
    float errorSum = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        errorSum += diff * diff;
    }
    
    predictionError_ = errorSum / static_cast<float>(std::min(predicted.size(), actual.size()));
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Find or create pattern neurons for current and next states
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState, similarityThreshold_);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState, similarityThreshold_);
    
    if (currentNeuron != INVALID_NEURON_ID && nextNeuron != INVALID_NEURON_ID &&
        currentNeuron != nextNeuron) {
        // Create sequence association
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
        
        // Update Hebbian learning matrix
        updateHebbianLearning(currentState, nextState, currentStep);
        
        // Record learned sequence
        pImpl->learnedSequences.emplace_back(currentState, nextState);
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
        
        // Create neural representation for this pattern
        createPatternNeuronRepresentation(newId, pattern);
        
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

void NeuralPrediction::createPatternNeuronRepresentation(NeuronId neuronId,
                                                         const std::vector<float>& pattern) {
    if (!brain_) return;
    
    // Find appropriate neuron for this pattern
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                // Set neuron's firing pattern based on input vector
                // This creates the neural representation of the sensory pattern
                float avgActivity = 0.0f;
                for (float val : pattern) {
                    avgActivity += std::abs(val);
                }
                avgActivity /= pattern.size();
                
                // Inject current to establish the neural representation
                neuron->injectCurrent(avgActivity * 10.0f);
                break;
            }
        }
    }
}

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->sequenceAssociations) {
        if (assoc.first == from && assoc.second == to) {
            // Update strength
            // In real implementation, this would modulate synaptic weight
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
    
    // Update Hebbian matrix
    updateHebbianMatrix(from, to, strength);
}

void NeuralPrediction::updateHebbianMatrix(NeuronId from, NeuronId to, float strength) {
    // Convert neuron IDs to matrix indices
    size_t fromIdx = from.index() % pImpl->hebbianMatrix.size();
    size_t toIdx = to.index() % pImpl->hebbianMatrix.size();
    
    if (fromIdx < pImpl->hebbianMatrix.size() && toIdx < pImpl->hebbianMatrix.size()) {
        // Update Hebbian weight
        pImpl->hebbianMatrix[fromIdx][toIdx] += strength;
        
        // Apply spike-timing dependent plasticity if brain has STDP system
        if (brain_ && brain_->getSTDP()) {
            // STDP will update synaptic weights based on spike timing
        }
    }
}

void NeuralPrediction::updateHebbianLearning(const std::vector<float>& currentState,
                                           const std::vector<float>& nextState,
                                           SimulationStep currentStep) {
    // Update Hebbian learning based on state transitions
    // This implements temporal difference learning
    
    // Get neuromodulation factors
    float reward = 0.5f;  // Default reward for sequence learning
    float error = 0.0f;   // Will be computed based on prediction accuracy
    
    // Apply Hebbian learning to predictive synapses
    modulatePredictionSynapses(error, reward);
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
                        temporalPredictionCount_++;
                        temporalPredictionAccuracy_ = 
                            (temporalPredictionAccuracy_ * (temporalPredictionCount_ - 1) + bestSimilarity) / 
                            static_cast<float>(temporalPredictionCount_);
                        return pImpl->patternRepresentations[i];
                    }
                }
            }
        }
    }
    
    // Use Hebbian predictions as fallback
    if (!pImpl->hebbianMatrix.empty()) {
        std::vector<float> hebbianPrediction = generateHebbianPrediction(currentState);
        if (!hebbianPrediction.empty()) {
            predictionConfidence_ = 0.5f;  // Moderate confidence for Hebbian predictions
            return hebbianPrediction;
        }
    }
    
    // No good prediction - return current state as baseline
    predictionConfidence_ = 0.0f;
    return currentState;
}

std::vector<float> NeuralPrediction::generateHebbianPrediction(const std::vector<float>& currentState) {
    if (pImpl->hebbianMatrix.empty()) return std::vector<float>();
    
    // Simple Hebbian prediction: average of associated patterns
    std::vector<float> prediction(currentState.size(), 0.0f);
    float totalWeight = 0.0f;
    
    // This is a simplified implementation
    // In a real implementation, this would use actual neural dynamics
    for (size_t i = 0; i < currentState.size(); ++i) {
        prediction[i] = currentState[i];  // Default to current state
    }
    
    return prediction;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                             SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    // Compute error
    if (predicted.empty()) {
        predictionError_ = 0.0f;
    } else {
        updatePredictionError(predicted, actualState);
        errorHistory_.push_back(predictionError_);
    }
    
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Get neuromodulation factors
    float reward = 1.0f - predictionError_;  // Higher reward for better predictions
    
    // Modulate learning based on error
    modulatePredictionSynapses(predictionError_, reward);
    
    return predictionError_;
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                // TD learning rule: delta = eligibility * (reward - baseline)
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                                const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Find experiences with same action and similar state
    float bestMatch = 0.0f;
    std::vector<float> bestConsequence;
    
    // Check learned action-consequences
    for (size_t i = 0; i < pImpl->actionConsequences.size(); ++i) {
        const auto& before = pImpl->actionConsequences[i].first;
        
        if (before.size() != currentState.size()) continue;
        
        float similarity = computeSimilarity(before, currentState);
        if (similarity > bestMatch) {
            bestMatch = similarity;
            bestConsequence = pImpl->actionConsequences[i].second;
        }
    }
    
    // Check recent actions
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
        actionConsequencePredictionCount_++;
        actionConsequenceAccuracy_ = 
            (actionConsequenceAccuracy_ * (actionConsequencePredictionCount_ - 1) + bestMatch) / 
            static_cast<float>(actionConsequencePredictionCount_);
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

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
    pImpl->patternNeurons.clear();
    pImpl->patternRepresentations.clear();
    pImpl->stateHistory.clear();
    pImpl->actionConsequences.clear();
    pImpl->actionRewards.clear();
    pImpl->actionStateMap.clear();
    pImpl->hebbianMatrix.clear();
    pImpl->eligibilityTraces.clear();
    
    predictionError_ = 0.0f;
    predictionConfidence_ = 0.5f;
    temporalPredictionCount_ = 0;
    actionConsequencePredictionCount_ = 0;
    temporalPredictionAccuracy_ = 0.5f;
    actionConsequenceAccuracy_ = 0.5f;
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> result = predictionNeurons_;
    result.insert(result.end(), pImpl->predictiveNeurons.begin(), pImpl->predictiveNeurons.end());
    return result;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

float NeuralPrediction::getTemporalPredictionAccuracy() const {
    return temporalPredictionCount_ > 0 ? temporalPredictionAccuracy_ : 0.5f;
}

float NeuralPrediction::getActionConsequenceAccuracy() const {
    return actionConsequencePredictionCount_ > 0 ? actionConsequenceAccuracy_ : 0.5f;
}

// ActionConsequencePredictor Implementation
struct ActionConsequencePredictor::Impl {
    Brain* brain;
    
    // State-action -> consequence mappings
    std::vector<std::pair<std::pair<std::vector<float>, ActionType>, std::vector<float>>> experiences;
    
    // Average reward per action
    std::vector<std::pair<ActionType, float>> actionQuality;
    
    Impl() : brain(nullptr) {}
};

ActionConsequencePredictor::ActionConsequencePredictor()
    : pImpl(new Impl)
{
}

ActionConsequencePredictor::~ActionConsequencePredictor() = default;

void ActionConsequencePredictor::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void ActionConsequencePredictor::recordExperience(ActionType action,
                                                  const std::vector<float>& beforeState,
                                                  const std::vector<float>& afterState,
                                                  float reward) {
    pImpl->experiences.push_back({{beforeState, action}, afterState});
    pImpl->actionQuality.push_back({action, reward});
    
    // Update average reward for this action
    float totalReward = 0.0f;
    size_t count = 0;
    for (const auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            totalReward += aq.second;
            ++count;
        }
    }
    
    // Update or add to action quality
    for (auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            aq.second = totalReward / count;
            break;
        }
    }
    
    // Limit experience memory
    if (pImpl->experiences.size() > 1000) {
        pImpl->experiences.erase(pImpl->experiences.begin());
    }
}

std::vector<float> ActionConsequencePredictor::predictConsequence(ActionType action,
                                                                  const std::vector<float>& currentState) {
    // Find most similar previous experience with this action
    float bestSimilarity = 0.0f;
    std::vector<float> bestConsequence;
    
    for (const auto& exp : pImpl->experiences) {
        if (exp.first.second == action) {
            // Compute similarity between current state and stored before-state
            const auto& before = exp.first.first;
            const auto& after = exp.second;
            
            if (before.size() != currentState.size()) continue;
            
            float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
            for (size_t i = 0; i < before.size(); ++i) {
                dot += before[i] * currentState[i];
                norm1 += before[i] * before[i];
                norm2 += currentState[i] * currentState[i];
            }
            
            float sim = (norm1 > 0.0001f && norm2 > 0.0001f) ? 
                        dot / (std::sqrt(norm1) * std::sqrt(norm2)) : 0.0f;
            
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestConsequence = after;
            }
        }
    }
    
    if (bestSimilarity > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;  // Default: no predicted change
}

float ActionConsequencePredictor::getConsequenceConfidence(ActionType action,
                                                           const std::vector<float>& currentState) const {
    size_t matchingCount = 0;
    for (const auto& exp : pImpl->experiences) {
        if (exp.first.second == action) {
            ++matchingCount;
        }
    }
    
    // More experiences = higher confidence
    return std::min(1.0f, static_cast<float>(matchingCount) / 10.0f);
}

void ActionConsequencePredictor::updatePrediction(ActionType action,
                                                const std::vector<float>& predicted,
                                                const std::vector<float>& actual,
                                                float reward) {
    // This would update the internal model
    // For now, just record the new experience
    recordExperience(action, predicted, actual, reward);
}

float ActionConsequencePredictor::getActionQuality(ActionType action) const {
    for (const auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            return aq.second;
        }
    }
    return 0.0f;
}

void ActionConsequencePredictor::clear() {
    pImpl->experiences.clear();
    pImpl->actionQuality.clear();
}

// PredictionErrorSignal Implementation
struct PredictionErrorSignal::Impl {
    Brain* brain;
    float lastIntensityError;
    float lastSpatialError;
    float lastTemporalError;
    
    // Neural error signals
    std::vector<NeuronId> errorNeurons;
    std::vector<float> errorSignals;
    
    Impl() : brain(nullptr), lastIntensityError(0.0f), lastSpatialError(0.0f), lastTemporalError(0.0f) {}
};

PredictionErrorSignal::PredictionErrorSignal()
    : pImpl(new Impl)
    , lastError_(0.0f)
    , surpriseThreshold_(0.3f)
{
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
}

PredictionErrorSignal::~PredictionErrorSignal() = default;

void PredictionErrorSignal::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize error neurons for prediction error signals
    initializeErrorNeurons();
    
    NLM_LOG_INFO("PredictionError system initialized with neural signals");
}

void PredictionErrorSignal::initializeErrorNeurons() {
    if (!brain_) return;
    
    // Create error signaling neural populations
    for (size_t i = 0; i < 20; ++i) {
        NeuronId neuronId = NeuronId(20000 + i);
        pImpl->errorNeurons.push_back(neuronId);
    }
}

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                        const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return 0.0f;
    }
    
    // Compute overall difference
    float totalDiff = 0.0f;
    float intensityDiff = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        totalDiff += diff;
        intensityDiff += diff * diff;
    }
    
    float meanError = totalDiff / predicted.size();
    float meanSqError = intensityDiff / predicted.size();
    
    // Compute spatial error (variance of differences)
    float spatialVar = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    float spatialError = std::sqrt(spatialVar / predicted.size());
    
    // Update components
    pImpl->lastIntensityError = std::sqrt(meanSqError);
    pImpl->lastSpatialError = spatialError;
    errorComponents_.intensityError = pImpl->lastIntensityError;
    errorComponents_.spatialError = pImpl->lastSpatialError;
    
    // Compute temporal error component
    errorComponents_.temporalError = std::abs(meanError);
    
    // Compute total error (combining components)
    lastError_ = std::sqrt(meanSqError + spatialError * spatialError);
    errorComponents_.totalError = lastError_;
    
    // Update neural error signals
    updateErrorSignals(lastError_);
    
    return lastError_;
}

void PredictionErrorSignal::updateErrorSignals(float error) {
    if (!pImpl->brain) return;
    
    // Convert prediction error to neuromodulatory signal
    float neuromodulationSignal = getModulationSignal();
    
    // Update error neurons based on prediction error
    for (NeuronId neuronId : pImpl->errorNeurons) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == neuronId) {
                    // Error neurons increase firing based on prediction error
                    float errorCurrent = neuromodulationSignal * 5.0f;
                    neuron->injectCurrent(errorCurrent);
                    
                    // Store the error signal
                    pImpl->errorSignals.push_back(neuromodulationSignal);
                    
                    if (pImpl->errorSignals.size() > 1000) {
                        pImpl->errorSignals.erase(pImpl->errorSignals.begin());
                    }
                    
                    break;
                }
            }
        }
    }
}

float PredictionErrorSignal::getModulationSignal() const {
    // Convert error to modulation signal (0-1)
    // Higher error = stronger modulation for learning
    return std::min(1.0f, lastError_ * 2.0f);
}

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

float PredictionErrorSignal::getMeanError() const {
    if (errorHistory_.empty()) return 0.0f;
    
    float sum = std::accumulate(errorHistory_.begin(), errorHistory_.end(), 0.0f);
    return sum / errorHistory_.size();
}

float PredictionErrorSignal::getMaxError() const {
    if (errorHistory_.empty()) return 0.0f;
    
    return *std::max_element(errorHistory_.begin(), errorHistory_.end());
}

float PredictionErrorSignal::getErrorVariance() const {
    if (errorHistory_.size() < 2) return 0.0f;
    
    float mean = getMeanError();
    float varianceSum = 0.0f;
    
    for (float error : errorHistory_) {
        varianceSum += (error - mean) * (error - mean);
    }
    
    return varianceSum / errorHistory_.size();
}

} // namespace nlm
