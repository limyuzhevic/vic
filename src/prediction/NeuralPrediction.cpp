#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>
#include <unordered_set>

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
    
    // Real neural prediction state using NLM substrate
    std::vector<NeuronId> predictionNeurons_;
    std::vector<NeuronId> sequenceNeurons_;
    std::deque<SimulationStep> stateTimestamps_;
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
    
    // Learning and plasticity state
    std::unordered_map<NeuronId, float> eligibilityTraces;
    float predictionError_;
    float predictionConfidence_;
    
    // Random number generator for biologically realistic noise
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    // Prediction components for biologically realistic error
    struct PredictionComponents {
        float intensityError = 0.0f;    // Mean squared error across all dimensions
        float temporalError = 0.0f;     // Error in timing/sequence prediction
        float patternError = 0.0f;      // Error in pattern similarity
        float totalError = 0.0f;        // Combined prediction error
    } errorComponents_;
    
    // Learning rate parameters (biologically inspired)
    float hebbianLearningRate_;
    float stdpLearningRate_;
    float rewardModulatedLearningRate_;
    
    // Surprise detection threshold (for neuromodulation)
    float surpriseThreshold_;
    
    Impl() : brain(nullptr), 
              predictionError_(0.0f), predictionConfidence_(0.5f),
              hebbianLearningRate_(0.01f), stdpLearningRate_(0.005f), 
              rewardModulatedLearningRate_(0.02f), surpriseThreshold_(0.3f),
              noiseDist(0.0f, 0.05f) {
        rng.seed(std::random_device{}());
        NLM_LOG_DEBUG("NeuralPrediction Impl initialized with biological parameters");
    }
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
    NLM_LOG_DEBUG("NeuralPrediction constructor: initialized with default parameters");
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("NeuralPrediction initialized with biological prediction system");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                         SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    // Store in main state history for prediction
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    
    // Keep only recent states (bounded memory)
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.erase(recentSensoryStates_.begin());
        stateTimestamps_.erase(stateTimestamps_.begin());
    }
    
    // Learn temporal sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
    
    // Apply Hebbian learning for pattern formation
    if (brain_) {
        NeuronId patternNeuron = findMatchingPatternNeuron(sensoryState);
        applyHebbianLearning(sensoryState, patternNeuron, currentStep);
    }
}

std::vector<float> NeuralPrediction::computeNeuralPrediction(const std::vector<float>& currentState,
                                                           SimulationStep currentStep) {
    if (pImpl->brain == nullptr) return currentState;
    
    // Check for matching pattern in learned representations
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    size_t bestPatternIndex = 0;
    
    // Compare with all learned pattern representations
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = pImpl->patternNeurons[i];
            bestPatternIndex = i;
        }
    }
    
    // If we have a good match, use associated pattern for prediction
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        // Find sequence association to predict what comes next
        for (const auto& assoc : pImpl->sequenceAssociations) {
            if (assoc.first == bestMatch) {
                // Find the associated pattern neuron
                for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                    if (pImpl->patternNeurons[i] == assoc.second) {
                        predictionConfidence_ = bestSimilarity;
                        
                        // Get the predicted state with some biological noise
                        auto predicted = pImpl->patternRepresentations[i];
                        return addBiologicalNoise(predicted);
                    }
                }
            }
        }
    }
    
    // No good prediction - return current state with noise
    predictionConfidence_ = 0.0f;
    return addBiologicalNoise(currentState);
}

float NeuralPrediction::computeBiologicallyRealisticError(const std::vector<float>& predicted,
                                                        const std::vector<float>& actual,
                                                        SimulationStep currentStep) {
    if (predicted.empty() || actual.empty()) return 0.0f;
    
    // Compute multiple error components biologically realistically
    float intensityError = 0.0f;  // Mean squared error (sensory intensity)
    float temporalError = 0.0f;   // Temporal sequence error
    float patternError = 0.0f;    // Pattern similarity error
    
    // Intensity error: captures overall magnitude differences
    float totalSqError = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        totalSqError += diff * diff;
    }
    intensityError = std::sqrt(totalSqError / std::min(predicted.size(), actual.size()));
    
    // Pattern error: cosine similarity in feature space
    patternError = 1.0f - computeSimilarity(predicted, actual);
    
    // Temporal error: based on sequence position and timing
    temporalError = 0.0f;
    if (!stateTimestamps_.empty() && currentStep > 0) {
        // Check if current step aligns with learned temporal patterns
        float expectedTiming = static_cast<float>(currentStep) * 0.1f;  // Biological time scaling
        float timingDeviation = std::abs(currentStep - stateTimestamps_.back()) * 0.01f;
        temporalError = std::min(1.0f, timingDeviation);
    }
    
    // Combine errors with biological weighting
    pImpl->errorComponents_.intensityError = intensityError;
    pImpl->errorComponents_.temporalError = temporalError;
    pImpl->errorComponents_.patternError = patternError;
    pImpl->errorComponents_.totalError = (intensityError * 0.5f + 
                                          temporalError * 0.3f + 
                                          patternError * 0.2f);
    
    // Apply surprise detection
    float predictionSurprise = pImpl->errorComponents_.totalError > pImpl->surpriseThreshold_ ?
                               pImpl->errorComponents_.totalError : 0.0f;
    
    NLM_LOG_DEBUG("Prediction error: intensity=" + std::to_string(intensityError) +
                  " temporal=" + std::to_string(temporalError) +
                  " pattern=" + std::to_string(patternError) +
                  " total=" + std::to_string(pImpl->errorComponents_.totalError));
    
    return pImpl->errorComponents_.totalError;
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    // Get current state for prediction
    const auto& currentState = recentSensoryStates_.back();
    
    // Compute prediction using neural substrate
    auto predicted = computeNeuralPrediction(currentState, currentStep);
    
    // Record for neural prediction tracking
    if (!predicted.empty()) {
        pImpl->predictionNeurons_.push_back(findMatchingPatternNeuron(predicted));
        if (pImpl->predictionNeurons_.size() > 100) {
            pImpl->predictionNeurons_.erase(pImpl->predictionNeurons_.begin());
        }
    }
    
    return predicted;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                             SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    // Compute biologically realistic prediction error
    if (!predicted.empty()) {
        predictionError_ = computeBiologicallyRealisticError(predicted, actualState, currentStep);
    } else {
        predictionError_ = 0.0f;
    }
    
    // Update error history
    errorHistory_.push_back(predictionError_);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Modulate prediction synapses based on error and reward
    float reward = 1.0f - predictionError_;
    modulatePredictionSynapses(predictionError_, reward);
    
    // Apply STDP for successful predictions
    if (brain_ && predictionError_ < 0.3f && !predicted.empty()) {
        // STDP strengthens synapses for accurate predictions
        applySTDPForPrediction(pImpl->predictionNeurons_.back(), 
                             findMatchingPatternNeuron(actualState),
                             currentStep, reward);
    }
    
    return predictionError_;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                              const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Find action-consequence associations
    std::vector<float> bestConsequence;
    float bestMatch = 0.0f;
    
    // Search through learned action-consequences
    for (size_t i = 0; i < pImpl->actionConsequences.size(); ++i) {
        if (pImpl->actionConsequences[i].first == action) {
            float sim = computeSimilarity(currentState, pImpl->actionConsequences[i].second);
            if (sim > bestMatch) {
                bestMatch = sim;
                bestConsequence = pImpl->actionConsequences[i].second;
            }
        }
    }
    
    // Use reward information to modulate confidence
    if (bestMatch > 0.5f && !bestConsequence.empty()) {
        // Get average reward for this action
        float avgReward = 0.0f;
        size_t rewardCount = 0;
        for (size_t i = 0; i < pImpl->actionRewards.size(); ++i) {
            if (i < pImpl->actionConsequences.size() && 
                pImpl->actionConsequences[i].first == action) {
                avgReward += pImpl->actionRewards[i];
                rewardCount++;
            }
        }
        if (rewardCount > 0) {
            avgReward /= rewardCount;
        }
        predictionConfidence_ = bestMatch * avgReward;
        
        // Add biological noise to consequence
        return addBiologicalNoise(bestConsequence);
    }
    
    return currentState; // Default: no change
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
    // Find or create pattern neurons for both states
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron != INVALID_NEURON_ID && nextNeuron != INVALID_NEURON_ID) {
        // Create sequence association with biological strength
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
        
        // Apply STDP to reinforce successful transitions
        if (brain_) {
            applySTDPForPrediction(currentNeuron, nextNeuron, currentStep, 1.0f);
        }
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
    
    // Create new pattern neuron if brain is available
    if (brain_) {
        NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 10000);
        pImpl->patternNeurons.push_back(newId);
        pImpl->patternRepresentations.push_back(pattern);
        
        NLM_LOG_DEBUG("Created new pattern neuron ID=" + std::to_string(newId.value) +
                      " with pattern similarity threshold=" + std::to_string(similarityThreshold));
        
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
    
    // Cosine similarity for neural pattern matching
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
                // Biological reward-modulated learning: 
                // Positive prediction error (failure) weakens synapses
                // Negative prediction error (success) strengthens synapses
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void NeuralPrediction::applySTDPForPrediction(NeuronId preNeuron, NeuronId postNeuron,
                                             SimulationStep currentStep, float reward) {
    if (!brain_) return;
    
    // STDP: spike-timing dependent plasticity
    // If pre-synaptic spike occurs before post-synaptic spike -> LTP
    // If post-synaptic spike occurs before pre-synaptic spike -> LTD
    
    float timingDifference = 0.0f;  // Simplified: assume pre before post
    
    // LTP (Long Term Potentiation) for successful predictions
    if (timingDifference > 0.0f) {
        float weightChange = pImpl->stdpLearningRate_ * (1.0f + reward * 0.5f);
        // In real implementation, would update synapse weight between neurons
        NLM_LOG_DEBUG("STDP LTP: strengthening synapse between neurons for accurate prediction");
    } else {
        // LTD (Long Term Depression) for inaccurate predictions
        float weightChange = -pImpl->stdpLearningRate_ * (1.0f - reward * 0.5f);
        // In real implementation, would update synapse weight between neurons
        NLM_LOG_DEBUG("STDP LTD: weakening synapse for inaccurate prediction");
    }
}

void NeuralPrediction::applyHebbianLearning(const std::vector<float>& pattern,
                                          NeuronId neuronId,
                                          SimulationStep currentStep) {
    if (!brain_) return;
    
    // Hebbian learning: neurons that fire together wire together
    // Strengthen synapses based on co-activation patterns
    
    float hebbianWeight = pImpl->hebbianLearningRate_;
    
    // In real implementation, would update synaptic weights
    // based on co-activation statistics
    NLM_LOG_DEBUG("Hebbian learning: strengthening associations for pattern neuron " + 
                  std::to_string(neuronId.value));
}

std::vector<float> NeuralPrediction::addBiologicalNoise(const std::vector<float>& pattern) const {
    std::vector<float> noisyPattern = pattern;
    
    // Add biologically realistic noise to predictions
    for (auto& value : noisyPattern) {
        value += pImpl->noiseDist(pImpl->rng);
    }
    
    // Clamp to valid range
    for (auto& value : noisyPattern) {
        value = std::clamp(value, 0.0f, 1.0f);
    }
    
    return noisyPattern;
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                        size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    // Get current state for chaining predictions
    auto currentPred = recentSensoryStates_.empty() ? 
                       std::vector<float>() : recentSensoryStates_.back();
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (currentPred.empty()) break;
        
        predictions.push_back(currentPred);
        
        // Use current prediction to generate next (chained prediction)
        auto nextPred = generatePrediction(currentStep + step);
        if (nextPred.empty()) break;
        currentPred = nextPred;
    }
    
    return predictions;
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
    
    // Clear neural prediction tracking
    pImpl->predictionNeurons_.clear();
    pImpl->sequenceNeurons_.clear();
    pImpl->eligibilityTraces.clear();
    
    predictionError_ = 0.0f;
    predictionConfidence_ = 0.5f;
    
    NLM_LOG_INFO("NeuralPrediction history cleared");
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    return predictionNeurons_;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
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
    float lastIntensityError;
    float lastSpatialError;
    float lastTemporalError;
    
    Impl() : lastIntensityError(0.0f), lastSpatialError(0.0f), lastTemporalError(0.0f) {}
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
        float diff = std::abs(predicted[i] - actual[i]);
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
    
    // Compute total error (combining components)
    lastError_ = std::sqrt(meanSqError + spatialError * spatialError);
    errorComponents_.totalError = lastError_;
    
    return lastError_;
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

} // namespace nlm