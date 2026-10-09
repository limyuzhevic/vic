#include "MotorSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain, Neuromodulator* neuromodulator,
                            PredictionSystem* predictionSystem, SelfModel* selfModel) {
    pImpl->brain_ = brain;
    pImpl->neuromodulator_ = neuromodulator;
    pImpl->predictionSystem_ = predictionSystem;
    pImpl->selfModel_ = selfModel;
    
    // Initialize motor regions if brain is available
    if (brain_) {
        pImpl->motorCortexRegions_.clear();
        pImpl->basalGangliaRegions_.clear();
        pImpl->cerebellumRegions_.clear();
        
        // Try to find motor-related regions
        for (auto regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                const std::string& name = region->getName();
                if (name.find("Motor") != std::string::npos || 
                    name.find("Cortex") != std::string::npos) {
                    pImpl->motorCortexRegions_.push_back(regionId);
                } else if (name.find("Basal") != std::string::npos || 
                          name.find("Ganglia") != std::string::npos) {
                    pImpl->basalGangliaRegions_.push_back(regionId);
                } else if (name.find("Cerebellum") != std::string::npos) {
                    pImpl->cerebellumRegions_.push_back(regionId);
                }
            }
        }
        
        // If specific regions not found, create them (placeholder)
        if (pImpl->motorCortexRegions_.empty()) {
            pImpl->motorCortexRegions_.push_back(brain_->addRegion("MotorCortex"));
        }
        if (pImpl->basalGangliaRegions_.empty()) {
            pImpl->basalGangliaRegions_.push_back(brain_->addRegion("BasalGanglia"));
        }
        if (pImpl->cerebellumRegions_.empty()) {
            pImpl->cerebellumRegions_.push_back(brain_->addRegion("Cerebellum"));
        }
    }
    
    NLM_LOG_INFO("MotorSystem initialized with brain, neuromodulator, prediction system, and self model");
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Update motor cortical spreading activation
    updateMotorCorticalRepresentation(motorRegion);
    
    // Get currently represented actions
    std::vector<ActionType> candidates = getActivelyRepresentedActions();
    if (candidates.empty()) {
        candidates = pImpl->availableActions_;
    }
    
    // Get current state from self model if available
    std::vector<float> currentState;
    if (pImpl->selfModel_ && pImpl->brain_) {
        currentState = pImpl->selfModel_->getCurrentGoal();
        if (currentState.empty()) {
            // Use a default state
            currentState = std::vector<float>(27, 0.0f);
        }
    } else {
        // Default state
        currentState = std::vector<float>(27, 0.0f);
    }
    
    // Apply actor-critic action selection
    ActionType selectedAction = selectActionBasedOnActorCritic(
        candidates, currentState, 
        (pImpl->neuromodulator_ ? pImpl->neuromodulator_->getExplorationBias() : 0.0f)
    );
    
    // Apply basal ganglia selection (GO/NO-GO pathways)
    GO_NG_SIGNAL signal = applyBasalGangliaSelection(
        candidates, 
        pImpl->actorCritic_.actionValues
    );
    
    ActionType finalAction = selectedAction;
    if (signal == GO_NG_SIGNAL::NO_GO && selectedAction != ActionType::Wait) {
        finalAction = ActionType::Wait;
    }
    
    // Create and record the action
    auto action = std::make_unique<Action>(finalAction);
    recordActionOutcome(finalAction, currentState, 0.0f, 0.0f); // Will be updated with actual reward
    pImpl->actionHistory_.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions_;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actorCritic_.actionPreferences[action] = preference;
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory_;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory_) {
        delete action;
    }
    pImpl->actionHistory_.clear();
}

ActionSequence MotorSystem::planActionSequence(const std::vector<ActionType>& proposedActions,
                                             float timePerAction) {
    return pImpl->planActionSequence(proposedActions, timePerAction);
}

ActionSequence MotorSystem::planGoalDirectedAction(const std::vector<float>& goalState,
                                                 const std::vector<float>& currentState) {
    ActionSequence sequence;
    
    // Simple goal-directed planning based on distance
    std::vector<float> delta(goalState.size());
    for (size_t i = 0; i < goalState.size() && i < currentState.size(); ++i) {
        delta[i] = goalState[i] - currentState[i];
    }
    
    // Determine which actions to take based on goal
    if (delta[0] > 0.5f) {  // Move forward to increase X
        sequence.addAction(ActionType::MoveForward, getOptimalActionDuration(ActionType::MoveForward));
    } else if (delta[0] < -0.5f) {  // Move backward to decrease X
        sequence.addAction(ActionType::MoveBackward, getOptimalActionDuration(ActionType::MoveBackward));
    }
    
    if (delta[1] > 0.5f) {  // Turn right (Y positive)
        sequence.addAction(ActionType::TurnRight, getOptimalActionDuration(ActionType::TurnRight));
    } else if (delta[1] < -0.5f) {  // Turn left (Y negative)
        sequence.addAction(ActionType::TurnLeft, getOptimalActionDuration(ActionType::TurnLeft));
    }
    
    return sequence;
}

ActionType MotorSystem::selectActionBasedOnActorCritic(const std::vector<ActionType>& candidates,
                                                      const std::vector<float>& state,
                                                      float explorationBonus) {
    return pImpl->selectActionBasedOnActorCritic(candidates, state, explorationBonus);
}

ActionType MotorSystem::applyBasalGangliaSelection(const std::vector<ActionType>& candidates,
                                                  const std::vector<float>& actionValues) {
    if (candidates.empty()) return GO_NG_SIGNAL::NO_GO;
    
    // Select action based on action values with noise for exploration
    std::vector<float> scores;
    for (const auto& action : candidates) {
        size_t actionIndex = static_cast<size_t>(action);
        float score = actionValues.empty() ? 
            pImpl->actorCritic_.actionValues.at(action) : 
            actionValues[actionIndex % actionValues.size()];
        
        // Add exploration noise
        std::normal_distribution<float> noiseDist(0.0f, pImpl->explorationRate_);
        score += noiseDist(pImpl->rng_);
        
        scores.push_back(score);
    }
    
    // Select action with highest score
    float maxScore = -1e30f;
    ActionType bestAction = candidates[0];
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (scores[i] > maxScore) {
            maxScore = scores[i];
            bestAction = candidates[i];
        }
    }
    
    // Apply GO/NO-GO threshold
    if (maxScore > 0.1f) {
        return GO_NG_SIGNAL::GO;
    } else if (maxScore < -0.1f) {
        return GO_NG_SIGNAL::NO_GO;
    } else {
        return GO_NG_SIGNAL::QUEUE;
    }
}

ActionType MotorSystem::applyMotorCortexSpreading(const std::vector<ActionType>& candidates,
                                                const NeuralRegion& motorRegion) {
    // Motor cortical spreading activation
    std::vector<float> activationScores(candidates.size(), 0.0f);
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        ActionType action = candidates[i];
        size_t actionIndex = static_cast<size_t>(action);
        
        // Get neural activity from the motor region
        float neuralActivity = 0.0f;
        if (pImpl->brain_) {
            for (const auto& population : motorRegion.getPopulations()) {
                neuralActivity += population->getAverageFiringRate();
            }
        }
        
        // Combine with stored activation
        activationScores[i] = pImpl->neuralState_.motorCorticalActivation[actionIndex] * 0.7f + 
                           neuralActivity * 0.3f;
    }
    
    // Select action with highest activation
    float maxScore = -1e30f;
    ActionType bestAction = ActionType::Wait;
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (activationScores[i] > maxScore) {
            maxScore = activationScores[i];
            bestAction = candidates[i];
        }
    }
    
    return bestAction;
}

void MotorSystem::startSequenceExecution(const ActionSequence& sequence) {
    pImpl->currentSequence_ = sequence;
    pImpl->currentSequence_.isActive = true;
    pImpl->currentSequence_.currentIndex = 0;
    
    if (!pImpl->currentSequence_.timings.empty()) {
        pImpl->currentSequence_.timings[0].isExecuting = true;
    }
    
    NLM_LOG_INFO("Started action sequence execution with " + std::to_string(sequence.actions.size()) + " actions");
}

void MotorSystem::updateSequenceExecution(float dt) {
    if (!pImpl->currentSequence_.isActive) return;
    
    // Advance the sequence
    pImpl->currentSequence_.advanceSequence(dt);
    
    // Update global clock
    pImpl->timingState_.globalClock += dt;
    
    // Check if sequence is complete
    if (pImpl->currentSequence_.isComplete()) {
        pImpl->currentSequence_.isActive = false;
        NLM_LOG_INFO("Action sequence completed");
    }
}

ActionType MotorSystem::getCurrentExecutingAction() const {
    return pImpl->currentSequence_.getCurrentAction();
}

bool MotorSystem::isSequenceExecuting() const {
    return pImpl->currentSequence_.isActive && !pImpl->currentSequence_.isComplete();
}

float MotorSystem::getSequenceProgress() const {
    return pImpl->currentSequence_.isActive ? 
        static_cast<float>(pImpl->currentSequence_.currentIndex) / 
        std::max(1.0f, static_cast<float>(pImpl->currentSequence_.actions.size())) : 0.0f;
}

void MotorSystem::learnActionOutcome(ActionType action, const std::vector<float>& state,
                                    float reward, float predictionError) {
    pImpl->learnActionOutcome(action, state, reward, predictionError);
}

void MotorSystem::updateActionValues(ActionType action, float reward, float temporalDifference) {
    pImpl->updateActionValues(action, reward, temporalDifference);
}

void MotorSystem::adaptMotorParameters(float error, float learningRate) {
    if (!pImpl->adaptiveTimingEnabled_) return;
    
    // Adapt motor timing parameters based on error
    float oldOptimal = pImpl->timingState_.optimalDurations[action];
    pImpl->timingState_.optimalDurations[action] = 
        std::clamp(oldOptimal + learningRate * error, 0.1f, 10.0f);
    
    // Update neural integration state
    pImpl->neuralState_.cerebellarForwardModelError[action] = error;
}

std::vector<float> MotorSystem::predictActionConsequence(ActionType action,
                                                         const std::vector<float>& currentState) {
    return pImpl->selfModel_->predictActionConsequence(action, currentState);
}

float MotorSystem::computeMotorPredictionError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual) {
    float error = 0.0f;
    for (size_t i = 0; i < predicted.size() && i < actual.size(); ++i) {
        error += std::abs(predicted[i] - actual[i]);
    }
    return error / std::max(1.0f, static_cast<float>(predicted.size()));
}

Action MotorSystem::compensateForDynamics(ActionType rawAction, const std::vector<float>& state) {
    // Simple dynamics compensation based on state
    Action compensatedAction = Action(rawAction);
    
    // Adjust parameters based on state
    std::vector<float> params = compensatedAction.getParameters();
    
    // Add small adjustment based on state to simulate dynamics compensation
    for (size_t i = 0; i < params.size(); ++i) {
        params[i] += (state[i] * 0.1f - 0.05f); // Small adjustment
    }
    
    compensatedAction.setParameters(params);
    return compensatedAction;
}

Action MotorSystem::applyGainScheduling(ActionType action, float error) {
    Action gainAction = Action(action);
    
    // Adjust action gains based on error
    std::vector<float> params = gainAction.getParameters();
    
    // Increase gains if error is large
    float gainFactor = std::clamp(std::abs(error), 0.0f, 1.0f);
    
    for (size_t i = 0; i < params.size(); ++i) {
        params[i] *= (1.0f + gainFactor * 0.5f);
    }
    
    gainAction.setParameters(params);
    return gainAction;
}

Action MotorSystem::enforceMotorConstraints(ActionType action, const std::vector<float>& state) {
    // Apply motor constraints
    Action constrainedAction = Action(action);
    
    // Simple constraint enforcement based on state
    switch (action) {
        case ActionType::MoveForward:
            if (state[0] > 0.9f) constrainedAction = Action(ActionType::Wait);
            break;
        case ActionType::MoveBackward:
            if (state[0] < -0.9f) constrainedAction = Action(ActionType::Wait);
            break;
        case ActionType::TurnLeft:
            if (state[1] < -0.9f) constrainedAction = Action(ActionType::Wait);
            break;
        case ActionType::TurnRight:
            if (state[1] > 0.9f) constrainedAction = Action(ActionType::Wait);
            break;
        default:
            break;
    }
    
    return constrainedAction;
}

void MotorSystem::recordActionOutcome(ActionType action, const std::vector<float>& state,
                                    float reward, float temporalDifference) {
    learnActionOutcome(action, state, reward, temporalDifference);
}

float MotorSystem::getActionQuality(ActionType action) const {
    return pImpl->getActionQuality(action);
}

ActionType MotorSystem::getBestActionBasedOnQuality(const std::vector<ActionType>& candidates) const {
    if (candidates.empty()) return ActionType::Wait;
    
    ActionType bestAction = candidates[0];
    float bestQuality = getActionQuality(bestAction);
    
    for (size_t i = 1; i < candidates.size(); ++i) {
        float quality = getActionQuality(candidates[i]);
        if (quality > bestQuality) {
            bestQuality = quality;
            bestAction = candidates[i];
        }
    }
    
    return bestAction;
}

void MotorSystem::updateMotorCorticalRepresentation(const NeuralRegion& motorRegion) {
    // Update motor cortical spreading activation
    for (size_t i = 0; i < pImpl->neuralState_.motorCorticalActivation.size(); ++i) {
        // Get neural activity from motor region
        float neuralActivity = 0.0f;
        for (const auto& population : motorRegion.getPopulations()) {
            neuralActivity += population->getAverageFiringRate() * 0.01f;
        }
        
        // Update activation with decay and input
        pImpl->neuralState_.motorCorticalActivation[i] *= 0.95f;  // Decay
        pImpl->neuralState_.motorCorticalActivation[i] += neuralActivity * 0.05f;  // Input
        pImpl->neuralState_.motorCorticalActivation[i] = 
            std::clamp(pImpl->neuralState_.motorCorticalActivation[i], 0.0f, 1.0f);
    }
}

float MotorSystem::computeActionActivation(ActionType action, const NeuralRegion& motorRegion) const {
    size_t actionIndex = static_cast<size_t>(action);
    float neuralActivity = 0.0f;
    
    for (const auto& population : motorRegion.getPopulations()) {
        neuralActivity += population->getAverageFiringRate();
    }
    
    return pImpl->neuralState_.motorCorticalActivation[actionIndex] * 0.7f + 
           neuralActivity * 0.3f;
}

std::vector<ActionType> MotorSystem::getActivelyRepresentedActions() const {
    std::vector<ActionType> activeActions;
    
    for (size_t i = 0; i < pImpl->neuralState_.motorCorticalActivation.size(); ++i) {
        if (pImpl->neuralState_.motorCorticalActivation[i] > 0.1f) {
            // Convert index back to ActionType
            try {
                ActionType action = static_cast<ActionType>(i);
                activeActions.push_back(action);
            } catch (...) {
                // Invalid action type, skip
            }
        }
    }
    
    return activeActions;
}

void MotorSystem::learnTemporalSequence(const std::vector<ActionType>& actionSequence,
                                       const std::vector<float>& timingPattern,
                                       float reward) {
    if (!pImpl->sequenceLearningEnabled_) return;
    
    // Record the sequence and its timing
    pImpl->sequenceHistory_.push_back(Action(new ActionType(actionSequence[0])));  // Simplified
    
    // Learn timing patterns
    for (size_t i = 0; i < actionSequence.size() && i < timingPattern.size(); ++i) {
        pImpl->timingState_.optimalDurations[actionSequence[i]] = timingPattern[i];
    }
    
    // Update action values based on sequence reward
    float sequenceValue = reward * static_cast<float>(actionSequence.size());
    for (const auto& action : actionSequence) {
        pImpl->actorCritic_.actionValues[action] += pImpl->actorCriticLearningRate_ * sequenceValue;
    }
}

void MotorSystem::refineActionTiming(ActionType action, float optimalDuration) {
    pImpl->timingState_.optimalDurations[action] = optimalDuration;
}

float MotorSystem::getOptimalActionDuration(ActionType action) const {
    auto it = pImpl->timingState_.optimalDurations.find(action);
    if (it != pImpl->timingState_.optimalDurations.end()) {
        return it->second;
    }
    
    // Default durations based on action type
    switch (action) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
            return 1.0f;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            return 0.5f;
        case ActionType::Look:
        case ActionType::LookUp:
        case ActionType::LookDown:
            return 0.2f;
        case ActionType::Interact:
        case ActionType::Eat:
        case ActionType::Drink:
            return 1.5f;
        default:
            return 0.5f;
    }
}

ActionType MotorSystem::selectStateDependentAction(const std::vector<float>& state) const {
    // Simple state-dependent action selection
    if (state.size() >= 3) {
        if (state[0] > 0.5f && state[1] > -0.2f && state[1] < 0.2f) {
            return ActionType::MoveForward;
        } else if (state[1] > 0.5f) {
            return ActionType::TurnRight;
        } else if (state[1] < -0.5f) {
            return ActionType::TurnLeft;
        } else if (state[0] < -0.5f) {
            return ActionType::MoveBackward;
        }
    }
    
    return ActionType::Wait;
}

ActionType MotorSystem::adaptToContext(const std::vector<float>& context,
                                    const std::vector<float>& currentState) {
    // Context-dependent adaptation
    float contextSimilarity = 0.0f;
    
    for (size_t i = 0; i < context.size() && i < currentState.size(); ++i) {
        contextSimilarity += std::abs(context[i] - currentState[i]);
    }
    
    contextSimilarity /= std::max(1.0f, static_cast<float>(context.size()));
    
    // Choose action based on context similarity
    if (contextSimilarity < 0.3f) {
        return ActionType::MoveForward;  // High similarity, continue with forward
    } else if (contextSimilarity < 0.7f) {
        return ActionType::Interact;  // Medium similarity, try interaction
    } else {
        return ActionType::Wait;  // Low similarity, wait and reassess
    }
}

float MotorSystem::getMotorConfidence() const {
    float totalActivation = 0.0f;
    for (float activation : pImpl->neuralState_.motorCorticalActivation) {
        totalActivation += activation;
    }
    return std::clamp(totalActivation / static_cast<float>(pImpl->neuralState_.motorCorticalActivation.size()), 0.0f, 1.0f);
}

float MotorSystem::getPlanningConfidence() const {
    // Calculate confidence based on prediction error and action quality
    float confidence = 0.0f;
    
    for (const auto& actionQuality : pImpl->learningState_.actionVariances) {
        confidence += 1.0f - actionQuality.second;  // Lower variance = higher confidence
    }
    
    confidence /= std::max(1.0f, static_cast<float>(pImpl->learningState_.actionVariances.size()));
    return confidence;
}

size_t MotorSystem::getSequenceLength() const {
    return pImpl->currentSequence_.actions.size();
}

float MotorSystem::getCurrentActionValue() const {
    ActionType currentAction = getCurrentExecutingAction();
    if (currentAction == ActionType::Wait) return 0.0f;
    
    auto it = pImpl->actorCritic_.actionValues.find(currentAction);
    if (it != pImpl->actorCritic_.actionValues.end()) {
        return it->second;
    }
    
    return 0.0f;
}

bool MotorSystem::isLearningFromError() const {
    return pImpl->isLearningFromError_;
}

void MotorSystem::reset() {
    clearHistory();
    pImpl->currentSequence_ = ActionSequence();
    pImpl->actorCritic_ = ActorCriticNetworks();
    pImpl->learningState_.reset();
    pImpl->neuralState_ = NeuralIntegrationState();
    pImpl->timingState_ = MotorTimingState();
    pImpl->isSequenceExecuting_ = false;
    pImpl->isLearningFromError_ = false;
    
    // Reset action preferences
    for (auto& action : pImpl->availableActions_) {
        pImpl->actorCritic_.actionValues[action] = 0.0f;
        pImpl->actorCritic_.actionPreferences[action] = 0.0f;
    }
    
    NLM_LOG_INFO("MotorSystem reset completed");
}

void MotorSystem::integrateWithMotorCortex(const NeuralRegion& motorRegion) {
    // Integrate with motor cortex regions
    updateMotorCorticalRepresentation(motorRegion);
}

void MotorSystem::integrateWithBasalGanglia(GO_NG_SIGNAL signal, ActionType action) {
    // Integrate with basal ganglia
    size_t actionIndex = static_cast<size_t>(action);
    
    if (signal == GO_NG_SIGNAL::GO) {
        pImpl->neuralState_.basalGangliaGoSignals[actionIndex] = 1.0f;
        pImpl->neuralState_.basalGangliaNoGoSignals[actionIndex] = 0.0f;
    } else if (signal == GO_NG_SIGNAL::NO_GO) {
        pImpl->neuralState_.basalGangliaGoSignals[actionIndex] = 0.0f;
        pImpl->neuralState_.basalGangliaNoGoSignals[actionIndex] = 1.0f;
    }
    
    // Update action selection based on basal ganglia signals
    if (signal == GO_NG_SIGNAL::GO) {
        pImpl->actorCritic_.actionPreferences[action] += 0.5f;
    } else if (signal == GO_NG_SIGNAL::NO_GO) {
        pImpl->actorCritic_.actionPreferences[action] -= 0.5f;
    }
}

void MotorSystem::integrateWithCerebellum(float predictionError, float timingError) {
    // Integrate with cerebellar forward model
    float totalError = predictionError + timingError;
    
    // Update cerebellar forward model error for all actions
    for (size_t i = 0; i < pImpl->neuralState_.cerebellarForwardModelError.size(); ++i) {
        pImpl->neuralState_.cerebellarForwardModelError[i] = totalError;
    }
    
    // Adapt timing based on error
    if (timingError != 0.0f) {
        for (auto& pair : pImpl->timingState_.optimalDurations) {
            pair.second += pImpl->actorCriticLearningRate_ * timingError;
        }
    }
}

void MotorSystem::integrateWithMotorNeurons(const Action& action) {
    // Integrate with motor neurons
    size_t actionIndex = static_cast<size_t>(action.getType());
    
    // Update motor neuron pool activity
    pImpl->neuralState_.motorNeuronPoolActivity[actionIndex] = 1.0f;
    
    // Decay other motor neuron activities
    for (size_t i = 0; i < pImpl->neuralState_.motorNeuronPoolActivity.size(); ++i) {
        if (i != actionIndex) {
            pImpl->neuralState_.motorNeuronPoolActivity[i] *= 0.9f;
        }
    }
}

void MotorSystem::integrateWithRewardSystem(float reward, float predictionError) {
    // Integrate with reward system
    if (reward != 0.0f) {
        pImpl->isLearningFromError_ = true;
        
        // Update actor-critic networks with reward prediction
        for (auto& pair : pImpl->actorCritic_.actionValues) {
            pair.second += pImpl->actorCriticLearningRate_ * reward;
        }
    }
    
    if (predictionError != 0.0f) {
        // Update prediction error signals
        for (size_t i = 0; i < pImpl->neuralState_.cerebellarForwardModelError.size(); ++i) {
            pImpl->neuralState_.cerebellarForwardModelError[i] = predictionError;
        }
    }
}

} // namespace nlm

// Implementation details for MotorSystem
MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::updateSequenceExecution(float dt) {
    if (!pImpl->currentSequence_.isActive) return;
    
    // Advance the sequence
    pImpl->currentSequence_.advanceSequence(dt);
    
    // Update global clock
    pImpl->timingState_.globalClock += dt;
    
    // Check if sequence is complete
    if (pImpl->currentSequence_.isComplete()) {
        pImpl->currentSequence_.isActive = false;
        NLM_LOG_INFO("Action sequence completed");
    }
}

ActionType MotorSystem::applyBasalGangliaSelection(const std::vector<ActionType>& candidates,
                                                  const std::vector<float>& actionValues) {
    if (candidates.empty()) return GO_NG_SIGNAL::NO_GO;
    
    // Select action based on action values with noise for exploration
    std::vector<float> scores;
    for (const auto& action : candidates) {
        size_t actionIndex = static_cast<size_t>(action);
        float score = actionValues.empty() ? 
            pImpl->actorCritic_.actionValues.at(action) : 
            actionValues[actionIndex % actionValues.size()];
        
        // Add exploration noise
        std::normal_distribution<float> noiseDist(0.0f, pImpl->explorationRate_);
        score += noiseDist(pImpl->rng_);
        
        scores.push_back(score);
    }
    
    // Select action with highest score
    float maxScore = -1e30f;
    ActionType bestAction = candidates[0];
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (scores[i] > maxScore) {
            maxScore = scores[i];
            bestAction = candidates[i];
        }
    }
    
    // Apply GO/NO-GO threshold
    if (maxScore > 0.1f) {
        return GO_NG_SIGNAL::GO;
    } else if (maxScore < -0.1f) {
        return GO_NG_SIGNAL::NO_GO;
    } else {
        return GO_NG_SIGNAL::QUEUE;
    }
}

std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory_;
}