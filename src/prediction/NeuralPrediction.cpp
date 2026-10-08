#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralPrediction::Impl {
    // Forward model parameters
    std::vector<std::vector<float>> forwardModelWeights;      // Action-to-sensory mappings
    std::vector<std::vector<float>> inverseModelWeights;      // Sensory-to-action mappings
    std::vector<float> predictionGains;                       // Prediction precision weights
    
    // Hierarchical prediction components
    std::vector<std::vector<float>> levelPredictions;          // Predictions at each level
    std::vector<std::vector<float>> levelErrors;               // Errors at each level
    std::vector<float> levelPrecision;                        // Precision of each level
    std::vector<float> levelConfidence;                       // Confidence of each level
    
    // Temporal prediction state
    std::deque<std::vector<float>> temporalPredictions;        // Predicted sequence
    std::deque<float> temporalErrors;                         // Temporal prediction errors
    std::vector<std::vector<float>> temporalSequences;         // Learned temporal patterns
    
    // Action prediction components
    std::vector<std::vector<float>> actionPredictions;         // Action predictions
    std::vector<float> actionValues;                          // Action values/rewards
    std::vector<float> actionConfidence;                      // Action confidence
    
    // Belief state for Bayesian updating
    std::vector<float> beliefState;                           // Current belief distribution
    std::vector<float> predictionPosterior;                   // Updated belief after prediction
    
    // Neuromodulation and learning
    std::vector<float> rewardPredictionErrors;                // RPE for each level
    std::vector<float> precisionWeights;                      // Precision-based learning rates
    float neuromodulationSignal;                               // Integrated error signal
    float explorationBonus;                                   // Exploration incentive
    
    // Free energy and surprise detection
    std::vector<float> freeEnergy;                            // Free energy at each level
    std::vector<float> surprise;                              // Surprise detection
    float predictionErrorThreshold;                           // Threshold for surprise
    
    // Synaptic prediction effects
    std::vector<float> predictionEligibilityTraces;           // Synaptic eligibility traces
    std::vector<float> predictionWeightChanges;               // Accumulated weight changes
    std::vector<float> predictionSynapticStrength;            // Strength of prediction pathways
    
    // Context and goal dependency
    std::string currentContext;                               // Current behavioral context
    std::vector<float> goalVector;                            // Goal-dependent predictions
    std::vector<std::vector<float>> contextualPredictions;   // Context-aware predictions
    
    Impl() : predictionError(0.0f), predictionConfidence(0.5f),
             neuromodulationSignal(0.0f), explorationBonus(0.0f),
             predictionErrorThreshold(0.3f) {
        
        // Initialize hierarchical prediction system (4 levels)
        int numLevels = 4;
        
        // Allocate memory for hierarchical components
        forwardModelWeights.resize(numLevels);
        inverseModelWeights.resize(numLevels);
        predictionGains.resize(numLevels, 1.0f);
        levelPredictions.resize(numLevels);
        levelErrors.resize(numLevels);
        levelPrecision.resize(numLevels, 1.0f);
        levelConfidence.resize(numLevels, 0.5f);
        actionPredictions.resize(numLevels);
        actionValues.resize(numLevels, 0.0f);
        actionConfidence.resize(numLevels, 0.5f);
        rewardPredictionErrors.resize(numLevels, 0.0f);
        precisionWeights.resize(numLevels, 1.0f);
        freeEnergy.resize(numLevels, 0.0f);
        surprise.resize(numLevels, 0.0f);
        predictionEligibilityTraces.resize(numLevels, 0.0f);
        predictionWeightChanges.resize(numLevels, 0.0f);
        predictionSynapticStrength.resize(numLevels, 0.1f);
        contextualPredictions.resize(numLevels);
        
        // Initialize neural sequence memory
        for (int level = 0; level < numLevels; ++level) {
            forwardModelWeights[level].resize(3, 0.0f);  // 3 sensory dimensions typical
            inverseModelWeights[level].resize(3, 0.0f);  // 3 action dimensions typical
            levelPredictions[level].resize(3, 0.0f);
            levelErrors[level].resize(3, 0.0f);
            actionPredictions[level].resize(3, 0.0f);
            contextualPredictions[level].resize(3, 0.0f);
            
            // Initialize random weights for forward/inverse models
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
            
            for (size_t i = 0; i < forwardModelWeights[level].size(); ++i) {
                forwardModelWeights[level][i] = dist(gen);
                inverseModelWeights[level][i] = dist(gen);
            }
        }
        
        // Initialize belief state
        beliefState.resize(3, 1.0f / 3.0f);  // Uniform initial belief
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
    , predictionConfidence_(0.5f) {
    
    NLM_LOG_INFO("NeuralPrediction: Initialized with hierarchical predictive coding");
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("NeuralPrediction: Initialized with hierarchical predictive coding");
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
    
    // Learn temporal sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    // Implement hierarchical predictive coding with forward models
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    const auto& currentState = recentSensoryStates_.back();
    
    // Multi-level prediction computation
    std::vector<float> multiLevelPrediction(currentState.size(), 0.0f);
    float totalPrecision = 0.0f;
    
    for (size_t level = 0; level < pImpl->levelPredictions.size(); ++level) {
        // Get prediction at this level using forward model
        std::vector<float> levelPrediction = computeLevelPrediction(currentState, level);
        
        // Update prediction confidence based on precision
        float precision = pImpl->levelPrecision[level];
        float confidence = calculateLevelConfidence(levelPrediction, currentState);
        
        pImpl->levelConfidence[level] = confidence;
        
        // Apply precision weighting
        float precisionWeight = precision * confidence;
        
        // Integrate prediction into multi-level result
        for (size_t i = 0; i < currentState.size() && i < levelPrediction.size(); ++i) {
            multiLevelPrediction[i] += levelPrediction[i] * precisionWeight;
        }
        
        totalPrecision += precisionWeight;
    }
    
    // Normalize by total precision
    if (totalPrecision > 0.0f) {
        for (size_t i = 0; i < multiLevelPrediction.size(); ++i) {
            multiLevelPrediction[i] /= totalPrecision;
        }
    }
    
    // Store prediction for temporal sequence
    pImpl->temporalPredictions.push_back(multiLevelPrediction);
    if (pImpl->temporalPredictions.size() > predictionHorizon_) {
        pImpl->temporalPredictions.pop_front();
    }
    
    // Compute overall prediction error
    computePredictionError(multiLevelPrediction, currentState);
    
    // Update belief state using Bayesian updating
    updateBeliefState(multiLevelPrediction);
    
    // Generate final prediction based on highest confidence level
    size_t bestLevel = std::distance(pImpl->levelConfidence.begin(),
        std::max_element(pImpl->levelConfidence.begin(), pImpl->levelConfidence.end()));
    
    predictionConfidence_ = pImpl->levelConfidence[bestLevel];
    
    NLM_LOG_DEBUG(std::string("NeuralPrediction: Generated multi-level prediction, confidence=") + 
                 std::to_string(predictionConfidence_));
    
    return multiLevelPrediction;
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

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                              SimulationStep currentStep) {
    // Compute prediction error and update forward models
    auto predicted = generatePrediction(currentStep);
    
    if (predicted.empty()) {
        predictionError_ = 0.0f;
        return predictionError_;
    }
    
    // Compute multi-level prediction error
    std::vector<float> levelErrors;
    for (size_t level = 0; level < pImpl->levelPredictions.size(); ++level) {
        float error = computeLevelPredictionError(predicted, actualState, level);
        levelErrors.push_back(error);
        
        // Update forward model weights using this error
        updateForwardModel(predicted, actualState, level, error);
    }
    
    // Compute integrated prediction error with precision weighting
    float integratedError = computeIntegratedPredictionError(levelErrors);
    predictionError_ = integratedError;
    
    // Store error for learning and analysis
    pImpl->temporalErrors.push_back(integratedError);
    if (pImpl->temporalErrors.size() > 1000) {
        pImpl->temporalErrors.pop_front();
    }
    
    // Apply prediction error modulation to synaptic learning
    applyPredictionErrorModulation(integratedError, actualState);
    
    // Compute reward prediction error if reward is in the state
    float reward = 0.0f;
    if (!actualState.empty()) {
        reward = actualState[0]; // First dimension often represents reward
        updateRewardPredictionError(reward, predicted, actualState);
    }
    
    // Check for prediction error spikes (surprise)
    checkPredictionErrorSurprise(integratedError);
    
    // Update action predictions based on prediction error
    updateActionPredictions(actualState, reward);
    
    // Update belief state using Bayesian principles
    updateBeliefState(actualState);
    
    // Apply free energy minimization
    applyFreeEnergyMinimization(predicted, actualState);
    
    // Record actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    NLM_LOG_DEBUG(std::string("NeuralPrediction: Updated with error=") + std::to_string(integratedError));
    
    return predictionError_;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Use inverse model to predict sensory consequences of action
    std::vector<float> predictedConsequence(currentState.size(), 0.0f);
    float totalPrecision = 0.0f;
    
    for (size_t level = 0; level < pImpl->inverseModelWeights.size(); ++level) {
        // Get action consequences from inverse model
        std::vector<float> levelConsequence = predictActionConsequenceLevel(action, currentState, level);
        
        float precision = pImpl->levelPrecision[level];
        float confidence = calculateActionConfidence(action, currentState, level);
        
        // Apply precision-weighted combination
        float weight = precision * confidence;
        totalPrecision += weight;
        
        for (size_t i = 0; i < currentState.size() && i < levelConsequence.size(); ++i) {
            predictedConsequence[i] += levelConsequence[i] * weight;
        }
    }
    
    if (totalPrecision > 0.0f) {
        for (size_t i = 0; i < predictedConsequence.size(); ++i) {
            predictedConsequence[i] /= totalPrecision;
        }
    }
    
    // Check for prediction confidence
    float confidence = calculateActionConfidence(action, currentState, 0);
    NLM_LOG_DEBUG(std::string("NeuralPrediction: Predicted consequence for action ") + 
                 std::to_string(static_cast<int>(action)) + 
                 std::string(", confidence=") + std::to_string(confidence));
    
    return predictedConsequence;
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                      size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    if (recentSensoryStates_.empty()) {
        return predictions;
    }
    
    auto currentPred = recentSensoryStates_.back();
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (currentPred.empty()) break;
        
        predictions.push_back(currentPred);
        
        // Generate next prediction using forward model
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
    
    // Update action prediction models
    updateActionPredictionModels(action, recentSensoryStates_.back(), step);
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Learn temporal associations using forward and inverse models
    
    // Find matching pattern neurons for current and next states
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron == INVALID_NEURON_ID || nextNeuron == INVALID_NEURON_ID) {
        // Create new pattern neurons if needed
        if (brain_) {
            currentNeuron = NeuronId(pImpl->patternNeurons.size() + 10000);
            pImpl->patternNeurons.push_back(currentNeuron);
            pImpl->patternRepresentations.push_back(currentState);
        }
        
        if (brain_) {
            nextNeuron = NeuronId(pImpl->patternNeurons.size() + 10000);
            pImpl->patternNeurons.push_back(nextNeuron);
            pImpl->patternRepresentations.push_back(nextState);
        }
    }
    
    if (currentNeuron != nextNeuron && brain_) {
        // Create sequence association with strength based on prediction error
        float predictionError = computeSimilarity(currentState, nextState);
        float associationStrength = 0.8f * predictionError; // Scale by prediction accuracy
        
        createSequenceAssociation(currentNeuron, nextNeuron, associationStrength);
        
        // Store in temporal sequence memory
        pImpl->temporalSequences.push_back({currentState, nextState});
        if (pImpl->temporalSequences.size() > 100) {
            pImpl->temporalSequences.erase(pImpl->temporalSequences.begin());
        }
    }
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                    float similarityThreshold) {
    // Check if we already have a similar pattern using multiple levels
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(pattern, pImpl->patternRepresentations[i]);
        if (sim >= similarityThreshold) {
            return pImpl->patternNeurons[i];
        }
    }
    
    // Create new pattern neuron if we have a brain reference
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

// Helper functions for prediction system

float NeuralPrediction::sampleGaussianNoise(float stddev) {
    // Simple Gaussian noise sampling
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, stddev);
    return dist(gen);
}

void NeuralPrediction::computePredictionError(const std::vector<float>& predicted,
                                             const std::vector<float>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        predictionError_ = 0.0f;
        return;
    }
    
    float sumSqError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = predicted[i] - actual[i];
        sumSqError += error * error;
    }
    
    predictionError_ = std::sqrt(sumSqError / predicted.size());
}

void NeuralPrediction::updateBeliefState(const std::vector<float>& observation) {
    // Simple Bayesian belief updating
    if (observation.empty()) return;
    
    // Update belief state based on observation
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < observation.size(); ++i) {
        // Scale observation to belief update
        pImpl->beliefState[i] = 0.9f * pImpl->beliefState[i] + 0.1f * observation[i];
        totalWeight += std::abs(observation[i]);
    }
    
    // Normalize belief state
    if (totalWeight > 0.0f) {
        for (size_t i = 0; i < pImpl->beliefState.size(); ++i) {
            pImpl->beliefState[i] /= totalWeight;
        }
    }
}

float NeuralPrediction::computeLevelPredictionError(const std::vector<float>& predicted,
                                                  const std::vector<float>& actual,
                                                  size_t level) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 0.0f;
    }
    
    // Compute prediction error with precision weighting
    float error = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        error += diff * diff;
    }
    
    float avgError = error / predicted.size();
    return std::sqrt(avgError);
}