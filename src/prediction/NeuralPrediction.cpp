#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Temporal sequence learning with neural dynamics
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    std::vector<float> associationStrengths;
    
    // Pattern recognition using neural population activity
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    std::vector<float> patternConfidences;
    std::vector<float> patternActivationTimes;
    
    // Temporal dynamics for prediction
    std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
    std::vector<float> temporalPredictions;
    
    // Action-consequence learning with neural credit assignment
    std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
    std::vector<float> actionRewards;
    std::vector<NeuronId> actionRelatedNeurons;
    
    // Working memory integration
    std::vector<NeuronId> workingMemoryNeurons;
    std::vector<std::vector<float>> workingMemoryActivity;
    
    // Neuromodulation integration
    std::vector<NeuronId> dopamineNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(10)
    , predictionHorizon_(3)  // Predict multiple steps
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , tdLearningRate_(0.01f)
    , rewardPredictionEnabled_(true)
    , neuromodulationEnabled_(true)
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
    
    // Store in recent states
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    
    // Keep only recent states for working memory efficiency
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
    
    // Store in working memory for persistence
    if (brain_ && neuromodulationEnabled_) {
        for (size_t i = 0; i < sensoryState.size(); ++i) {
            if (sensoryState[i] > 0.1f) {
                // Find or create working memory neuron for this feature
                NeuronId wmNeuron = findMatchingPatternNeuron(sensoryState, 0.5f);
                if (wmNeuron != INVALID_NEURON_ID) {
                    workingMemoryNeurons_.push_back(wmNeuron);
                    workingMemoryActivity_.push_back(sensoryState);
                }
            }
        }
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching learned sequence
    const auto& currentState = recentSensoryStates_.back();
    
    // Find most similar pattern we've seen using neural dynamics
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    // Check all learned pattern neurons
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = pImpl->patternNeurons[i];
        }
    }
    
    // If we have a good match, use pattern completion for prediction
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        // Use neural attractor dynamics for pattern completion
        std::vector<float> completedPattern = patternCompletion(currentState, 0.7f);
        
        if (!completedPattern.empty()) {
            predictionConfidence_ = bestSimilarity;
            
            // Find the neuron that represents this completed pattern
            NeuronId patternNeuron = findMatchingPatternNeuron(completedPattern, 0.7f);
            
            // Predict next state based on sequence associations
            for (size_t i = 0; i < pImpl->sequenceAssociations.size(); ++i) {
                if (pImpl->sequenceAssociations[i].first == bestMatch) {
                    NeuronId nextNeuron = pImpl->sequenceAssociations[i].second;
                    
                    // Find representation for the predicted next state
                    for (size_t j = 0; j < pImpl->patternNeurons.size(); ++j) {
                        if (pImpl->patternNeurons[j] == nextNeuron) {
                            predictionConfidence_ = bestSimilarity * pImpl->associationStrengths[i];
                            return pImpl->patternRepresentations[j];
                        }
                    }
                }
            }
        }
    }
    
    // Use temporal dynamics prediction if no pattern match
    if (pImpl->temporalPredictions.size() > 1 && bestSimilarity > 0.5f) {
        size_t predIndex = std::min(static_cast<size_t>(bestSimilarity * 10.0f), pImpl->temporalPredictions.size() - 1);
        predictionConfidence_ = 0.5f + bestSimilarity * 0.5f;
        return pImpl->temporalPredictions[predIndex];
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
    for (size_t i = 0; i < pImpl->sequenceAssociations.size(); ++i) {
        if (pImpl->sequenceAssociations[i].first == from && pImpl->sequenceAssociations[i].second == to) {
            // Update strength
            pImpl->associationStrengths[i] = strength;
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
    pImpl->associationStrengths.push_back(strength);
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

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
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