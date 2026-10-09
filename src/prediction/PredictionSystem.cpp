#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <limits>

namespace nlm {

struct PredictionSystem::Impl {
    // Hierarchical levels
    std::vector<PredictionLevel> levels;
    
    // Configuration
    size_t hierarchyLevels;
    size_t predictionHorizon;
    size_t sequenceMemorySize;
    bool precisionWeightingEnabled;
    bool activeInferenceEnabled;
    bool sensoryPredictionEnabled;
    bool motorPredictionEnabled;
    bool internalPredictionEnabled;
    
    // Brain integration
    Brain* brain;
    std::mt19937 rng;
    
    // State tracking
    std::vector<float> currentSensoryState;
    SimulationStep currentStep;
    std::deque<std::vector<float>> predictionHistory;
    
    // Action prediction
    std::vector<std::vector<float>> actionConsequences;
    std::vector<float> actionRewards;
    
    // Neuromodulation
    std::unordered_map<std::string, Neuromodulator*> neuromodulators;
    
    // Learning state
    float predictionError;
    float predictionConfidence;
    std::vector<std::vector<float>> errorHistory;
    
    Impl() : hierarchyLevels(4), predictionHorizon(10), sequenceMemorySize(50),
             precisionWeightingEnabled(true), activeInferenceEnabled(true),
             sensoryPredictionEnabled(true), motorPredictionEnabled(true), 
             internalPredictionEnabled(true), brain(nullptr), rng(std::random_device{}()),
             currentStep(0), predictionError(0.0f), predictionConfidence(0.5f) {
        initializeLevels();
    }
    
    void initializeLevels() {
        levels.clear();
        levels.reserve(hierarchyLevels);
        
        // Initialize each level with appropriate dimensions
        // Lower levels: sensory features, Higher levels: abstract concepts
        for (size_t i = 0; i < hierarchyLevels; ++i) {
            PredictionLevel level;
            
            // Set dimensions based on hierarchy level
            size_t stateDim = 32 + i * 16;  // Grow with hierarchy level
            if (i == 0) stateDim = 64;     // Bottom level: high-dimensional sensory
            else if (i == hierarchyLevels - 1) stateDim = 16;  // Top level: low-dimensional concepts
            
            level.stateEstimate.resize(stateDim, 0.0f);
            level.prediction.resize(stateDim, 0.0f);
            level.error.resize(stateDim, 0.0f);
            level.sequenceMemory.reserve(sequenceMemorySize);
            
            // Initialize forward weights (to higher level)
            size_t nextLevelDim = (i < hierarchyLevels - 1) ? (32 + (i + 1) * 16) : 16;
            level.forwardWeights.resize(stateDim, std::vector<float>(nextLevelDim, 0.0f));
            level.forwardBiases.resize(nextLevelDim, 0.0f);
            
            // Initialize backward weights (from higher level)
            level.backwardWeights.resize(nextLevelDim, std::vector<float>(stateDim, 0.0f));
            level.backwardBiases.resize(stateDim, 0.0f);
            
            // Initialize weights with random values
            initializeWeightsRandom(level);
            
            levels.push_back(std::move(level));
        }
        
        NLM_LOG_INFO("Initialized prediction system with " + 
                    std::to_string(hierarchyLevels) + " hierarchy levels");
    }
    
    void initializeWeightsRandom(PredictionLevel& level) {
        std::normal_distribution<float> weightDist(0.0f, 0.1f);
        std::uniform_real_distribution<float> biasDist(-0.1f, 0.1f);
        std::uniform_real_distribution<float> precisionDist(0.5f, 2.0f);
        
        for (auto& row : level.forwardWeights) {
            for (auto& weight : row) {
                weight = weightDist(rng);
            }
        }
        
        for (auto& bias : level.forwardBiases) {
            bias = biasDist(rng);
        }
        
        for (auto& row : level.backwardWeights) {
            for (auto& weight : row) {
                weight = weightDist(rng);
            }
        }
        
        for (auto& bias : level.backwardBiases) {
            bias = biasDist(rng);
        }
        
        level.precision = precisionDist(rng);
    }
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain, size_t hierarchyLevels) {
    pImpl->brain = brain;
    pImpl->hierarchyLevels = hierarchyLevels;
    pImpl->initializeLevels();
    
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(
    const SensoryInput& currentState, double dt) {
    
    // Store current sensory state
    pImpl->currentSensoryState = currentState.getData();
    pImpl->currentStep++;
    
    // Reset hierarchical predictions
    for (auto& level : pImpl->levels) {
        // Copy current state to lowest level
        if (&level == &pImpl->levels.front()) {
            level.stateEstimate = pImpl->currentSensoryState;
        }
        
        // Compute forward predictions
        computeForwardPredictions(level);
        
        // Perform temporal sequence prediction
        temporalSequencePrediction(level);
    }
    
    // Integrate sensory-to-motor predictions if enabled
    if (pImpl->motorPredictionEnabled) {
        integrateSensoryToMotor(pImpl->levels.back().prediction);
    }
    
    // Store prediction history
    storePredictionHistory();
    
    // For now, return a vision copy with modified data based on prediction
    // In a real implementation, this would create a new SensoryInput with predicted features
    auto prediction = currentState.clone();
    
    // Update prediction confidence based on overall error
    float totalError = 0.0f;
    for (const auto& level : pImpl->levels) {
        float levelError = 0.0f;
        for (float e : level.error) levelError += e * e;
        levelError = std::sqrt(levelError / level.error.size());
        totalError += levelError;
    }
    pImpl->predictionError = totalError / pImpl->levels.size();
    pImpl->predictionConfidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& actualState, double dt) {
    // Convert actual state to vector
    std::vector<float> actualData = actualState.getData();
    
    // Update from lowest to highest level
    for (size_t i = 0; i < pImpl->levels.size(); ++i) {
        auto& level = pImpl->levels[i];
        
        // Compute backward prediction error
        computeBackwardErrors(level);
        
        // Update precision weights if enabled
        if (pImpl->precisionWeightingEnabled) {
            updatePrecisionWeights(level);
        }
        
        // Update state estimate using precision-weighted error
        updateStateEstimate(level, actualData, i == 0);
        
        // Learn temporal sequences
        if (level.sequenceMemory.size() >= 2) {
            learnTemporalSequence(level);
        }
    }
    
    // Update neuromodulation based on prediction errors
    updateNeuromodulation(dt);
    
    NLM_LOG_DEBUG("Updated predictions with actual state");
}

void PredictionSystem::computeForwardPredictions(PredictionLevel& level) {
    // Forward prediction: lower level predicts what higher level should produce
    // level.prediction = f_forward(level.stateEstimate)
    
    // Clear prediction
    level.prediction.assign(level.prediction.size(), 0.0f);
    
    if (level.stateEstimate.empty()) return;
    
    // Compute forward prediction using weighted sum
    for (size_t i = 0; i < level.forwardWeights.size(); ++i) {
        float sum = level.forwardBiases[i];
        for (size_t j = 0; j < level.forwardWeights[i].size(); ++j) {
            sum += level.forwardWeights[i][j] * level.stateEstimate[j];
        }
        level.prediction[i] = sum;
    }
    
    // Apply non-linearity (tanh for bounded output)
    for (float& p : level.prediction) {
        p = std::tanh(p);
    }
}

void PredictionSystem::computeBackwardErrors(PredictionLevel& level) {
    // Backward error: higher level provides error signal to lower level
    // level.error = g_backward(level.prediction - level.stateEstimate)
    
    // Clear error
    level.error.assign(level.error.size(), 0.0f);
    
    if (level.prediction.empty() || level.stateEstimate.empty()) return;
    
    // Compute raw prediction error
    for (size_t i = 0; i < level.prediction.size() && i < level.error.size(); ++i) {
        float diff = level.prediction[i] - level.stateEstimate[i];
        level.error[i] = diff;
    }
    
    // Apply precision weighting if enabled
    if (pImpl->precisionWeightingEnabled) {
        for (size_t i = 0; i < level.error.size(); ++i) {
            level.error[i] *= level.precision;
        }
    }
}

void PredictionSystem::updatePrecisionWeights(PredictionLevel& level) {
    // Update precision based on prediction error statistics
    float errorVariance = 0.0f;
    for (float e : level.error) {
        errorVariance += e * e;
    }
    errorVariance /= level.error.size();
    
    // Precision reflects inverse of uncertainty
    // High precision = high confidence = low variance
    float targetPrecision = 1.0f / (errorVariance + 0.01f);
    
    // Adaptive precision update
    float precisionChange = (targetPrecision - level.precision) * level.precisionLearningRate;
    level.precision += precisionChange;
    
    // Clip precision to reasonable range
    level.precision = std::max(0.1f, std::min(10.0f, level.precision));
}

void PredictionSystem::temporalSequencePrediction(PredictionLevel& level) {
    if (!pImpl->sensoryPredictionEnabled || level.stateHistory.size() < 2) {
        return;
    }
    
    // Look for temporal patterns in state history
    if (level.stateHistory.size() >= 3) {
        // Get recent sequence
        size_t recentStart = std::max(0UL, level.stateHistory.size() - 5);
        
        // Find similar previous states
        std::vector<float> recentState = level.stateHistory.back().first;
        
        for (size_t i = recentStart; i < level.stateHistory.size() - 1; ++i) {
            const auto& historyState = level.stateHistory[i].first;
            float similarity = computeCosineSimilarity(recentState, historyState);
            
            if (similarity > 0.8f) {
                // Found matching sequence
                const auto& nextState = level.stateHistory[i + 1].first;
                level.sequenceMemory.emplace_back(recentState, nextState);
                
                if (level.sequenceMemory.size() > pImpl->sequenceMemorySize) {
                    level.sequenceMemory.erase(level.sequenceMemory.begin());
                }
            }
        }
    }
}

void PredictionSystem::integrateSensoryToMotor(std::vector<float>& prediction) {
    // Integrate sensory predictions with motor planning
    if (pImpl->brain == nullptr) return;
    
    // This would integrate with motor system predictions
    // For now, just apply a simple integration
    for (float& p : prediction) {
        p *= 0.9f;  // Dampen prediction based on motor uncertainty
    }
}

float PredictionSystem::computeCosineSimilarity(const std::vector<float>& a, 
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
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

void PredictionSystem::learnTemporalSequence(PredictionLevel& level) {
    if (level.sequenceMemory.size() < 2) return;
    
    // Learn temporal associations between state patterns
    // This would update forward/backward weights to reinforce successful predictions
    
    // For each learned sequence, update weights
    for (const auto& seq : level.sequenceMemory) {
        const auto& current = seq.first;
        const auto& next = seq.second;
        
        // Update forward weights to predict next state from current
        for (size_t i = 0; i < level.forwardWeights.size(); ++i) {
            for (size_t j = 0; j < current.size() && j < level.forwardWeights[i].size(); ++j) {
                level.forwardWeights[i][j] += 0.01f * (next[i] - level.prediction[i]) * current[j];
            }
        }
    }
}

void PredictionSystem::updateStateEstimate(PredictionLevel& level, 
                                          const std::vector<float>& actualState,
                                          bool isBottomLevel) {
    // Update state estimate using prediction error
    // state = prediction + precision_weighted_error
    
    if (level.error.empty()) return;
    
    for (size_t i = 0; i < level.stateEstimate.size() && i < level.error.size(); ++i) {
        // Precision-weighted state update
        level.stateEstimate[i] += level.precision * level.error[i];
        
        // Clamp to reasonable bounds
        level.stateEstimate[i] = std::max(-1.0f, std::min(1.0f, level.stateEstimate[i]));
    }
}

ActionType PredictionSystem::selectAction(const std::vector<ActionType>& availableActions,
                                        const SensoryInput& currentState) {
    if (!pImpl->activeInferenceEnabled || availableActions.empty()) {
        return availableActions.empty() ? ActionType::NO_ACTION : availableActions[0];
    }
    
    return performActiveInference(availableActions, currentState);
}

ActionType PredictionSystem::performActiveInference(
    const std::vector<ActionType>& availableActions,
    const SensoryInput& currentState) {
    
    std::vector<float> currentStateVec = currentState.getData();
    float bestActionValue = std::numeric_limits<float>::lowest();
    ActionType bestAction = ActionType::NO_ACTION;
    
    for (ActionType action : availableActions) {
        // Predict outcome of this action
        std::vector<float> predictedOutcome = predictActionOutcome(action, currentStateVec);
        
        // Compute expected prediction error
        float expectedError = computeExpectedPredictionError(predictedOutcome, currentStateVec);
        
        // Active inference: choose action that minimizes expected prediction error
        float actionValue = -expectedError;  // Negative error = good action
        
        if (actionValue > bestActionValue) {
            bestActionValue = actionValue;
            bestAction = action;
        }
    }
    
    return bestAction;
}

std::vector<float> PredictionSystem::predictActionOutcome(ActionType action,
                                                           const std::vector<float>& currentState) {
    // Predict sensory consequences of taking an action
    std::vector<float> outcome = currentState;
    
    // Apply action-specific transformation
    switch (action) {
        case ActionType::MOVE_LEFT:
            for (size_t i = 0; i < outcome.size() && i < 3; ++i) {
                outcome[i] -= 0.1f;
            }
            break;
        case ActionType::MOVE_RIGHT:
            for (size_t i = 0; i < outcome.size() && i < 3; ++i) {
                outcome[i] += 0.1f;
            }
            break;
        case ActionType::MOVE_UP:
            for (size_t i = 0; i < outcome.size() && i < 3; ++i) {
                outcome[i] += 0.1f;
            }
            break;
        case ActionType::MOVE_DOWN:
            for (size_t i = 0; i < outcome.size() && i < 3; ++i) {
                outcome[i] -= 0.1f;
            }
            break;
        default:
            break; // No change
    }
    
    // Clamp to valid range
    for (float& o : outcome) {
        o = std::max(-1.0f, std::min(1.0f, o));
    }
    
    return outcome;
}

float PredictionSystem::computeExpectedPredictionError(
    const std::vector<float>& predictedOutcome,
    const std::vector<float>& currentState) const {
    
    // Compute expected prediction error based on outcome uncertainty
    float error = 0.0f;
    
    for (size_t i = 0; i < std::min(predictedOutcome.size(), currentState.size()); ++i) {
        float diff = predictedOutcome[i] - currentState[i];
        error += diff * diff;
    }
    
    if (predictedOutcome.size() > 0) {
        error /= predictedOutcome.size();
    }
    
    return std::sqrt(error);
}

std::vector<float> PredictionSystem::getPredictionErrors() const {
    std::vector<float> errors;
    for (const auto& level : pImpl->levels) {
        float levelError = 0.0f;
        for (float e : level.error) {
            levelError += e * e;
        }
        levelError = std::sqrt(levelError / level.error.size());
        errors.push_back(levelError);
    }
    return errors;
}

std::vector<float> PredictionSystem::getPredictionConfidences() const {
    std::vector<float> confidences;
    for (const auto& level : pImpl->levels) {
        float levelConfidence = 1.0f / (1.0f + level.precision);
        confidences.push_back(levelConfidence);
    }
    return confidences;
}

std::vector<std::vector<float>> PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    for (auto& level : pImpl->levels) {
        level.stateHistory.clear();
        level.sequenceMemory.clear();
    }
    pImpl->predictionHistory.clear();
    pImpl->errorHistory.clear();
    
    NLM_LOG_DEBUG("Prediction history cleared");
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction models using observed data
    std::vector<float> obsData = observation.getData();
    
    // Store observation in lowest level for learning
    if (!pImpl->levels.empty()) {
        pImpl->levels.front().stateHistory.emplace_back(obsData, pImpl->currentStep);
        
        // Keep only recent history
        if (pImpl->levels.front().stateHistory.size() > pImpl->sequenceMemorySize) {
            pImpl->levels.front().stateHistory.erase(
                pImpl->levels.front().stateHistory.begin());
        }
        
        // Learn temporal sequences
        learnTemporalSequence(pImpl->levels.front());
        
        // Update prediction models
        computeForwardPredictions(pImpl->levels.front());
    }
    
    // Store in error history
    pImpl->errorHistory.push_back(getPredictionErrors());
}

std::vector<std::vector<float>> PredictionSystem::getHierarchicalState() const {
    std::vector<std::vector<float>> hierarchicalState;
    hierarchicalState.reserve(pImpl->levels.size());
    
    for (const auto& level : pImpl->levels) {
        hierarchicalState.push_back(level.stateEstimate);
    }
    
    return hierarchicalState;
}

void PredictionSystem::storePredictionHistory() {
    if (!pImpl->levels.empty()) {
        pImpl->predictionHistory.push_back(pImpl->levels.back().prediction);
        if (pImpl->predictionHistory.size() > 1000) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
    }
}

std::vector<std::vector<NeuronId>> PredictionSystem::getPredictionNeurons() const {
    std::vector<std::vector<NeuronId>> allNeurons;
    
    if (pImpl->brain == nullptr) {
        return allNeurons;
    }
    
    for (const auto& level : pImpl->levels) {
        std::vector<NeuronId> levelNeurons;
        
        // Get neurons associated with this prediction level
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (NeuronId neuronId : pop->getNeuronIds()) {
                    // Heuristic: neurons with higher activity are more likely prediction neurons
                    // This would be implemented based on neural activity patterns
                    levelNeurons.push_back(neuronId);
                }
            }
        }
        
        allNeurons.push_back(levelNeurons);
    }
    
    return allNeurons;
}

void PredictionSystem::updateNeuromodulation(double dt) {
    // Update neuromodulators based on prediction errors
    
    // Calculate overall prediction error
    float totalError = 0.0f;
    for (const auto& level : pImpl->levels) {
        for (float e : level.error) {
            totalError += e * e;
        }
    }
    totalError = std::sqrt(totalError / pImpl->levels.size());
    
    // Update each neuromodulator
    for (auto& pair : pImpl->neuromodulators) {
        Neuromodulator* modulator = pair.second;
        
        if (modulator) {
            // Update neuromodulator level based on prediction error
            float currentLevel = modulator->getLevel();
            
            // Neuromodulators typically increase with prediction error
            // (e.g., dopamine for unexpected outcomes)
            float newLevel = currentLevel + totalError * 0.01f * dt;
            
            // Clip to reasonable range
            newLevel = std::max(0.0f, std::min(1.0f, newLevel));
            modulator->setLevel(newLevel);
        }
    }
}

void PredictionSystem::setNeuromodulator(Neuromodulator* modulator, const std::string& name) {
    if (modulator) {
        pImpl->neuromodulators[name] = modulator;
        NLM_LOG_INFO("Set neuromodulator: " + name);
    }
}

void PredictionSystem::setPredictionHorizon(size_t steps) {
    pImpl->predictionHorizon_ = steps;
}

void PredictionSystem::setSequenceMemorySize(size_t size) {
    pImpl->sequenceMemorySize_ = size;
}

void PredictionSystem::setPrecisionWeighting(bool enable) {
    pImpl->precisionWeightingEnabled_ = enable;
}

void PredictionSystem::setActiveInference(bool enable) {
    pImpl->activeInferenceEnabled_ = enable;
}

void PredictionSystem::enableSensoryPrediction(bool enable) {
    pImpl->sensoryPredictionEnabled_ = enable;
}

void PredictionSystem::enableMotorPrediction(bool enable) {
    pImpl->motorPredictionEnabled_ = enable;
}

void PredictionSystem::enableInternalPrediction(bool enable) {
    pImpl->internalPredictionEnabled_ = enable;
}

} // namespace nlm