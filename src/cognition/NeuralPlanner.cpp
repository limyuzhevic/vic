#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <limits>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    // Neural network components for planning
    std::mt19937 rng;
    std::vector<float> neuralState;
    std::vector<std::vector<float>> predictionCache;
    std::vector<float> actionHistory;
    std::vector<float> rewardHistory;
    
    Impl() : brain(nullptr), rng(std::random_device()()) {
        neuralState.reserve(100);
        predictionCache.reserve(100);
        actionHistory.reserve(1000);
        rewardHistory.reserve(1000);
    }
};

// Helper functions
namespace {
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
        float normA = std::inner_product(a.begin(), a.end(), a.begin(), 0.0f);
        float normB = std::inner_product(b.begin(), b.end(), b.begin(), 0.0f);
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
}

// SelfModel Implementation
struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

// NeuralPlanner Implementation

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(10, 0.0f);  // 10 action types
    // Initialize learning parameters
    learningRate_ = 0.1f;
    discountFactor_ = 0.95f;
    explorationFactor_ = 0.1f;
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural state
    pImpl->neuralState.resize(20, 0.5f);  // Default neural state
    
    NLM_LOG_INFO("NeuralPlanner initialized with depth " + std::to_string(planningDepth_));
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                     float targetReward) {
    // Neural simulation: generate multiple candidate action sequences
    auto sequences = generateActionSequences(planningDepth_);
    
    if (sequences.empty()) {
        return ActionType::Wait;
    }
    
    // Evaluate sequences using neural predictive dynamics
    PlanningCandidate best = evaluateSequence(sequences[0], currentState);
    
    // Neural selection based on expected reward and confidence
    float bestScore = -std::numeric_limits<float>::max();
    
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        
        // Combined score considering reward, confidence, and novelty
        float novelty = 0.0f;
        if (wasRecentPlanSuccessful()) {
            // Reduce novelty if we've been successful recently
            novelty = 0.5f;
        }
        
        float score = candidate.expectedReward * candidate.confidence * (1.0f + novelty);
        
        if (score > bestScore) {
            bestScore = score;
            best = candidate;
        }
    }
    
    // Update confidence based on evaluation consistency
    planningConfidence_ = best.confidence;
    
    // Record decision for learning
    if (!best.actions.empty()) {
        recentPlanSuccess_.push_back(best.expectedReward >= targetReward);
        if (recentPlanSuccess_.size() > 10) {
            recentPlanSuccess_.pop_front();
        }
        
        // Store in neural history for learning
        ActionType chosenAction = best.actions[0];
        size_t actionIdx = static_cast<size_t>(chosenAction);
        if (actionIdx < pImpl->actionHistory.size()) {
            pImpl->actionHistory[actionIdx] += 1.0f;
        }
        
        return chosenAction;
    }
    
    // Default: wait if no good option
    return ActionType::Wait;
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                 const std::vector<float>& startState) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = actions.size();
    
    std::vector<float> currentState = startState;
    float totalReward = 0.0f;
    float confidence = 1.0f;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType action = actions[i];
        
        // Evaluate this action using neural evaluation
        float actionValue = evaluateAction(action, currentState);
        
        // Predict next state using forward model
        auto predictedNext = predictNextState(action, currentState);
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
        }
        
        // Get action quality from experience
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            // Blend learned quality with current evaluation
            float learnedQuality = actionQuality_[actionIdx];
            actionValue = learnedQuality * 0.6f + actionValue * 0.4f;
        }
        
        totalReward += actionValue;
        
        // Reduce confidence based on prediction uncertainty
        confidence *= getSelfModelConfidence(action);
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> result;
    
    // Base actions with preferred ordering based on learning
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::Interact,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::MoveBackward, ActionType::Wait
    };
    
    // Neural-style generation with stochasticity
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            // Randomize action selection based on learning history
            std::vector<ActionType> shuffled = baseActions;
            std::shuffle(shuffled.begin(), shuffled.end(), pImpl->rng);
            
            for (ActionType action : shuffled) {
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
                
                if (result.size() >= 50) break;  // Limit search space
            }
        };
    
    std::vector<ActionType> current;
    generate(current, depth);
    
    // Limit number of sequences
    if (result.size() > 100) {
        result.resize(100);
    }
    
    return result;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Base value from learned quality
    float baseValue = actionQuality_[actionIdx];
    
    // Bonus for approaching goal (if goal is defined)
    float goalBonus = 0.0f;
    if (!currentGoal_.empty() && state.size() == currentGoal_.size()) {
        float stateGoalSim = 0.0f, stateNorm = 0.0f, goalNorm = 0.0f;
        for (size_t i = 0; i < state.size(); ++i) {
            stateGoalSim += state[i] * currentGoal_[i];
            stateNorm += state[i] * state[i];
            goalNorm += currentGoal_[i] * currentGoal_[i];
        }
        if (stateNorm > 0.0001f && goalNorm > 0.0001f) {
            goalBonus = stateGoalSim / (std::sqrt(stateNorm) * std::sqrt(goalNorm));
        }
    }
    
    return baseValue * 0.6f + goalBonus * 0.4f;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                  const std::vector<float>& currentState) {
    // Use simple action-consequence prediction with neural dynamics
    std::vector<float> nextState = currentState;
    
    // Simple predictive dynamics model
    switch (action) {
        case ActionType::MoveForward:
            // Visual forward movement
            if (nextState.size() > 10) {
                for (size_t i = 10; i < nextState.size(); ++i) {
                    nextState[i-10] = nextState[i] * 0.85f;
                    nextState[i] *= 0.9f;
                }
            }
            break;
            
        case ActionType::MoveBackward:
            if (nextState.size() > 10) {
                for (size_t i = nextState.size() - 1; i >= 10; --i) {
                    nextState[i] = nextState[i-10] * 0.85f;
                }
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotation effect
            float turnFactor = (action == ActionType::TurnLeft) ? -0.1f : 0.1f;
            for (auto& v : nextState) {
                v += turnFactor * (v > 0.5f ? -1.0f : 1.0f);
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
            
        case ActionType::Interact:
            // Interaction creates significant state changes
            for (auto& v : nextState) {
                v = v > 0.5f ? 0.85f : 0.15f;
            }
            break;
            
        case ActionType::Wait:
        default:
            // Some natural decay
            for (auto& v : nextState) {
                v = v * 0.98f + 0.01f;
            }
            break;
    }
    
    return nextState;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                    const std::vector<ActionType>& actualActions,
                                    float actualReward) {
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    // Learn from the difference between planned and actual
    for (size_t i = 0; i < minLen; ++i) {
        ActionType planned = plannedActions[i];
        ActionType actual = actualActions[i];
        
        // Update action quality based on success/failure
        size_t actionIdx = static_cast<size_t>(actual);
        if (actionIdx < actionQuality_.size()) {
            // Positive reinforcement if action matches plan
            float delta = 0.0f;
            if (planned == actual) {
                // Good match, increase quality
                delta = learningRate_ * actualReward * 0.1f;
            } else {
                // Bad match, decrease quality
                delta = -learningRate_ * actualReward * 0.05f;
            }
            
            // Apply learning with decay
            actionQuality_[actionIdx] = actionQuality_[actionIdx] * 0.9f + delta;
            actionQuality_[actionIdx] = std::clamp(actionQuality_[actionIdx], -1.0f, 1.0f);
            
            // Store reward for future learning
            pImpl->rewardHistory.push_back(actualReward);
            if (pImpl->rewardHistory.size() > 100) {
                pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
            }
        }
    }
}

void NeuralPlanner::clearCache() {
    // Clear planning cache and neural state
    pImpl->neuralState.assign(pImpl->neuralState.size(), 0.0f);
    pImpl->predictionCache.clear();
    pImpl->actionHistory.assign(pImpl->actionHistory.size(), 0.0f);
    pImpl->rewardHistory.clear();
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return true;  // No data means we assume success
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) ++successCount;
    }
    
    return successCount > recentPlanSuccess_.size() / 2;
}

// SelfModel Implementation

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
{
    actionEffects_.resize(10);  // 10 action types
    learningDecay_ = 0.95f;
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized for self-representation");
}

void SelfModel::recordSelfAction(ActionType action,
                                const std::vector<float>& beforeState,
                                const std::vector<float>& afterState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.observationCount = 1;
    
    // Compute confidence as consistency with previous observations
    float consistency = 0.5f;
    for (const auto& prev : actionEffects_[actionIdx]) {
        if (computeSimilarity(beforeState, prev.beforeState) > 0.8f) {
            consistency = std::max(consistency, 
                computeSimilarity(afterState, prev.afterState));
            effect.observationCount += prev.observationCount;
        }
    }
    effect.confidence = consistency;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update capability level
    float totalConfidence = 0.0f;
    size_t count = 0;
    for (const auto& effects : actionEffects_) {
        for (const auto& e : effects) {
            totalConfidence += e.confidence;
            ++count;
        }
    }
    if (count > 0) {
        capabilityLevel_ = totalConfidence / count;
        // Apply decay
        capabilityLevel_ *= learningDecay_;
        capabilityLevel_ = std::clamp(capabilityLevel_, 0.1f, 1.0f);
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return currentState;
    
    return findMatchingEffect(action, currentState);
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return 0.0f;
    
    float totalConf = 0.0f;
    for (const auto& e : effects) {
        totalConf += e.confidence;
    }
    return totalConf / effects.size();
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                              const std::vector<float>& afterState,
                                              ActionType action) const {
    // If we have a good prediction for this action, it's likely self-generated
    auto predicted = findMatchingEffect(action, beforeState);
    
    if (predicted.empty()) return 0.0f;
    
    float similarity = computeSimilarity(predicted, afterState);
    return similarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    ActionType best = ActionType::Wait;
    float bestValue = -std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        // Check how well this action would work in current state
        auto predicted = findMatchingEffect(static_cast<ActionType>(i), state);
        if (!predicted.empty()) {
            // Value = how much the state changes toward reward
            float value = computeSimilarity(predicted, state);
            if (value > bestValue) {
                bestValue = value;
                best = static_cast<ActionType>(i);
            }
        }
    }
    
    return best;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find and update matching effect
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(effect.beforeState, predicted) > 0.5f) {
            // Update with prediction error
            float error = 1.0f - computeSimilarity(predicted, actual);
            
            // Adjust confidence based on error (learning from error)
            effect.confidence *= (1.0f - error * 0.05f);
            effect.confidence = std::max(0.1f, effect.confidence);
            
            break;  // Update only first matching effect
        }
    }
}

float SelfModel::getBodyAwareness() const {
    return capabilityLevel_;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    capabilityLevel_ = 0.5f;
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                 const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return {};
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return {};
    
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& effect : effects) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSim) {
            bestSim = sim;
            bestPrediction = effect.afterState;
        }
    }
    
    return bestPrediction;
}

float SelfModel::computeSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
    float normA = std::inner_product(a.begin(), a.end(), a.begin(), 0.0f);
    float normB = std::inner_product(b.begin(), b.end(), b.begin(), 0.0f);
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// SocialLearning Implementation

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized for social cognition");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                       const std::vector<float>& observerState,
                                       const std::vector<float>& resultingState) {
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward
    
    observedActions_.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and know its effect
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : observedActions_) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
    float similarity = computeSimilarity(neuralPattern, signalPattern_);
    return similarity > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : observedActions_) {
        // Adjust reward estimate based on interaction outcome
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    observationCount_ = 0;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
    float normA = std::inner_product(a.begin(), a.end(), a.begin(), 0.0f);
    float normB = std::inner_product(b.begin(), b.end(), b.begin(), 0.0f);
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm

} // namespace nlm