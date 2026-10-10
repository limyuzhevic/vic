#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(10, 0.0f);  // 10 action types
}

NeuralPlanner::~NeuralPlanner() = default;

NLM_LOG_INFO("NeuralPlanner initialized");

// Initialize advanced planning systems
initializeAdvancedPlanning();

void NeuralPlanner::initializeAdvancedPlanning() {
    NLM_LOG_INFO("Initializing advanced neural planning systems");
    
    // Initialize neuromodulation gating
    neuromodulationGating_[NeuromodulatorType::DOPAMINE] = 1.0f;
    neuromodulationGating_[NeuromodulatorType::ACETYLCHOLINE] = 1.0f;
    neuromodulationGating_[NeuromodulatorType::SEROTONIN] = 1.0f;
    neuromodulationGating_[NeuromodulatorType::NORADRENALINE] = 1.0f;
    neuromodulationGating_[NeuromodulatorType::ENDORPHIN] = 1.0f;
    
    // Initialize working memory integration
    workingMemoryPlanCache_.clear();
    
    // Initialize transfer learning
    transferMemory_.clear();
    
    // Initialize strategy performance tracking
    strategyPerformance_[StrategyType::RandomSearch] = 0.0f;
    strategyPerformance_[StrategyType::Greedy] = 0.0f;
    strategyPerformance_[StrategyType::TreeSearch] = 0.0f;
    strategyPerformance_[StrategyType::MonteCarlo] = 0.0f;
    strategyPerformance_[StrategyType::Evolutionary] = 0.0f;
    strategyPerformance_[StrategyType::CuriosityDriven] = 0.0f;
    strategyPerformance_[StrategyType::Developmental] = 0.0f;
    
    // Initialize hierarchical goals
    hierarchicalGoals_.clear();
    
    NLM_LOG_INFO("Advanced neural planning systems initialized");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                    float targetReward) {
    // Generate possible action sequences
    auto sequences = generateActionSequences(planningDepth_);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    // Evaluate each sequence
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update confidence based on how consistent evaluations are
    if (!sequences.empty()) {
        planningConfidence_ = best.confidence;
    }
    
    // Return first action of best sequence
    if (!best.actions.empty()) {
        // Record success/failure based on whether we achieved target
        recentPlanSuccess_.push_back(best.expectedReward >= targetReward);
        if (recentPlanSuccess_.size() > 10) {
            recentPlanSuccess_.pop_front();
        }
        
        return best.actions[0];
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
        
        // Evaluate this action
        float actionValue = evaluateAction(action, currentState);
        
        // Predict next state
        auto predictedNext = predictNextState(action, currentState);
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
        }
        
        // Get action quality from experience
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            actionValue = actionQuality_[actionIdx] * 0.7f + actionValue * 0.3f;
        }
        
        totalReward += actionValue;
        
        // Reduce confidence if we're uncertain
        confidence *= getSelfModelConfidence(action);
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> result;
    
    // Simple enumeration of action sequences
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // For now, generate sequences of depth actions
    // This could be optimized with pruning
    
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            for (ActionType action : baseActions) {
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
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
    
    // Base value from experience
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
    
    return baseValue * 0.7f + goalBonus * 0.3f;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                  const std::vector<float>& currentState) {
    // Use simple action-consequence prediction
    // In a full implementation, this would use learned forward models
    
    std::vector<float> nextState = currentState;
    
    // Apply expected effect based on action
    // This is a simplified model - real implementation would use experience
    switch (action) {
        case ActionType::MoveForward:
            // Shift visual field forward
            if (nextState.size() > 10) {
                for (size_t i = 10; i < nextState.size(); ++i) {
                    nextState[i-10] = nextState[i] * 0.9f;
                }
            }
            break;
        case ActionType::MoveBackward:
            if (nextState.size() > 10) {
                for (size_t i = nextState.size() - 1; i >= 10; --i) {
                    nextState[i] = nextState[i-10] * 0.9f;
                }
            }
            break;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotate pattern
            // Simplified: just add some noise
            for (auto& v : nextState) {
                v += 0.05f * (v > 0.5f ? -1.0f : 1.0f);
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
        case ActionType::Interact:
            // Interaction changes state significantly
            for (auto& v : nextState) {
                v = v > 0.5f ? 0.8f : 0.2f;
            }
            break;
        case ActionType::Wait:
        default:
            // No change
            break;
    }
    
    return nextState;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                     const std::vector<ActionType>& actualActions,
                                     float actualReward) {
    // Update action quality based on how well plan worked
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    for (size_t i = 0; i < minLen; ++i) {
        size_t actionIdx = static_cast<size_t>(actualActions[i]);
        if (actionIdx < actionQuality_.size()) {
            // If action matched plan, positive update; else negative
            float delta = (actualActions[i] == plannedActions[i]) ? 0.1f : -0.05f;
            delta *= actualReward;
            
            actionQuality_[actionIdx] = std::clamp(
                actionQuality_[actionIdx] + delta, -1.0f, 1.0f);
        }
    }
}

void NeuralPlanner::clearCache() {
    // Clear any planning cache
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return true;  // No data
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) ++successCount;
    }
    
    return successCount > recentPlanSuccess_.size() / 2;
}

// SelfModel Implementation
struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
{
    actionEffects_.resize(10);  // 10 action types
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized");
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

float SelfModel::computeSelfGenerated Likeness(const std::vector<float>& beforeState,
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
    float bestValue = -1000.0f;
    
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
            
            // Adjust confidence based on error
            effect.confidence *= (1.0f - error * 0.1f);
            effect.confidence = std::max(0.1f, effect.confidence);
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
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// SocialLearning Implementation
struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

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
    NLM_LOG_INFO("SocialLearning initialized");
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
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
// Advanced Planning Methods Implementation

std::vector<ActionType> NeuralPlanner::planBackward(const std::vector<float>& goal,
                                                    const std::vector<float>& startState,
                                                    const PlanningContext& context) {
    NLM_LOG_INFO("Performing backward planning from goal to actions");
    
    std::vector<ActionType> plannedActions;
    
    // Backward search from goal state
    std::vector<std::vector<ActionType>> actionSequences;
    
    // Generate sequences in reverse (backward from goal)
    std::function<void(std::vector<ActionType>&, size_t, const std::vector<float>&)> backwardSearch = 
        [&](std::vector<ActionType>& current, size_t depth, const std::vector<float>& currentGoal) {
            if (depth == 0) {
                actionSequences.push_back(current);
                return;
            }
            
            for (ActionType action : getAllActions()) {
                current.push_back(action);
                
                // Simulate backward effect
                std::vector<float> newGoal = currentGoal;
                applyBackwardEffect(action, newGoal);
                
                backwardSearch(current, depth - 1, newGoal);
                current.pop_back();
            }
        };
    
    std::vector<ActionType> current;
    backwardSearch(current, planningDepth_, goal);
    
    // Evaluate all backward-planned sequences
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    for (const auto& sequence : actionSequences) {
        // Reverse sequence for forward execution
        std::vector<ActionType> forwardSequence = sequence;
        std::reverse(forwardSequence.begin(), forwardSequence.end());
        
        PlanningCandidate candidate = evaluateSequence(forwardSequence, startState);
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    if (!best.actions.empty()) {
        plannedActions = best.actions;
        NLM_LOG_INFO("Backward planning successful, found %d actions", plannedActions.size());
    }
    
    return plannedActions;
}

std::vector<float> NeuralPlanner::predictForward(const std::vector<ActionType>& actions,
                                                  const std::vector<float>& currentState,
                                                  const PlanningContext& context) const {
    NLM_LOG_INFO("Predicting forward consequences of action sequence");
    
    std::vector<float> predictedState = currentState;
    std::vector<float> predictionErrors;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType action = actions[i];
        
        // Get forward prediction from SelfModel
        std::vector<float> nextState = predictNextState(action, predictedState);
        
        // Apply neuromodulation gating to prediction confidence
        float predictionConfidence = 1.0f;
        if (brain_) {
            predictionConfidence = getNeuromodulationGating(NeuromodulatorType::ACETYLCHOLINE);
        }
        
        predictionConfidence *= getSelfModelConfidence(action);
        predictionConfidence *= context.neuromodulationLevel;
        
        // Incorporate curiosity effects
        if (context.curiosityLevel > 0.0f) {
            predictionConfidence *= (1.0f + context.curiosityLevel * 0.5f);
        }
        
        // Apply prediction error feedback
        if (context.predictionError > 0.0f) {
            predictionConfidence *= (1.0f - context.predictionError * 0.3f);
        }
        
        // Scale prediction by confidence
        for (size_t j = 0; j < nextState.size(); ++j) {
            nextState[j] = predictedState[j] * predictionConfidence + 
                          nextState[j] * (1.0f - predictionConfidence);
        }
        
        predictedState = nextState;
        predictionErrors.push_back(computePredictionError(currentState, predictedState, actions));
    }
    
    // Record prediction history for learning
    if (brain_) {
        recordPredictionHistory(actions, predictedState, predictionErrors, context);
    }
    
    return predictedState;
}

PlanningCandidate NeuralPlanner::planWithWorkingMemory(const std::vector<float>& currentState,
                                                      const PlanningContext& context) {
    NLM_LOG_INFO("Planning with working memory integration");
    
    PlanningCandidate candidate;
    candidate.actions.clear();
    candidate.predictedStates.clear();
    candidate.expectedReward = 0.0f;
    candidate.confidence = 0.0f;
    
    // Integrate with actual working memory if available
    if (brain_ && brain_->getWorkingMemory()) {
        auto workingMemory = brain_->getWorkingMemory();
        
        // Get current working memory content
        std::vector<float> wmContent = workingMemory->retrieve();
        
        // Create planning context based on working memory
        PlanningContext wmContext = context;
        wmContext.currentState = wmContent;
        
        // Generate alternative action sequences
        std::vector<std::vector<ActionType>> alternativePlans;
        
        // Plan with working memory support
        auto sequences = generateActionSequences(planningDepth_);
        
        // Select sequences compatible with working memory
        for (const auto& sequence : sequences) {
            if (isCompatibleWithWorkingMemory(sequence, wmContent)) {
                alternativePlans.push_back(sequence);
            }
        }
        
        // Evaluate all compatible sequences
        for (const auto& sequence : alternativePlans) {
            PlanningCandidate planCandidate = evaluateSequence(sequence, wmContent);
            
            // Boost reward if plan maintains working memory content
            float wmRetentionBonus = computeWorkingMemoryRetentionBonus(sequence, wmContent);
            planCandidate.expectedReward += wmRetentionBonus;
            
            if (planCandidate.expectedReward > candidate.expectedReward) {
                candidate = planCandidate;
            }
        }
        
        // Store plan in working memory
        if (!candidate.actions.empty()) {
            storePlanInWorkingMemory(candidate.actions, context);
        }
    }
    
    return candidate;
}

void NeuralPlanner::incorporatePredictionError(const std::vector<ActionType>& actions,
                                              float predictionError,
                                              const PlanningContext& context) {
    NLM_LOG_INFO("Incorporating prediction error into planning");
    
    if (predictionError > 0.1f) {
        // Significant prediction error - need to adjust planning
        
        // Update action quality based on error
        for (size_t i = 0; i < std::min(actions.size(), actionQuality_.size()); ++i) {
            size_t actionIdx = static_cast<size_t>(actions[i]);
            if (actionIdx < actionQuality_.size()) {
                // Reduce confidence in actions that led to prediction error
                actionQuality_[actionIdx] *= (1.0f - predictionError * 0.2f);
            }
        }
        
        // Adjust planning depth based on error
        if (predictionError > 0.5f) {
            // High error - increase exploration
            planningDepth_ = std::min(planningDepth_ + 2, 10u);
        }
        
        // Update prediction error history
        trialErrorStats_.predictionErrors.push_back(predictionError);
        if (trialErrorStats_.predictionErrors.size() > 100) {
            trialErrorStats_.predictionErrors.erase(trialErrorStats_.predictionErrors.begin());
        }
        
        // Apply trial-and-error learning
        applyTrialErrorLearning(actions, 0.0f, context);
    }
}

float NeuralPlanner::getDevelopmentalPlanningGain(DevelopmentalStage stage) const {
    // Different developmental stages benefit from different planning approaches
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: simple random search
            return 0.3f;
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: rapid learning from prediction errors
            return 1.5f;
        case DevelopmentalStage::Maturation:
            // Maturation: developing strategic planning
            return 0.8f;
        case DevelopmentalStage::Adult:
            // Adult: balanced planning
            return 1.0f;
        case DevelopmentalStage::Aging:
            // Aging: simplified planning (preserve function)
            return 0.6f;
        default:
            return 1.0f;
    }
}

std::vector<ActionType> NeuralPlanner::generateCuriosityDrivenActions(
    const std::vector<float>& currentState,
    const PlanningContext& context) const {
    NLM_LOG_INFO("Generating curiosity-driven action sequences");
    
    std::vector<ActionType> curiosityActions;
    
    // Generate random actions for exploration
    auto allActions = getAllActions();
    std::vector<ActionType> randomActions;
    
    // Select actions that are under-explored
    std::unordered_map<size_t, size_t> actionUsageCount;
    for (ActionType action : allActions) {
        actionUsageCount[static_cast<size_t>(action)] = 0;
    }
    
    // Check recent usage (simplified)
    for (size_t i = 0; i < 50 && i < recentPlanSuccess_.size(); ++i) {
        // Check if action was used in recent plans
        // This is a simplified version
    }
    
    // Select least-used actions
    std::vector<std::pair<size_t, size_t>> actionUsagePairs;
    for (const auto& pair : actionUsageCount) {
        actionUsagePairs.push_back(pair);
    }
    std::sort(actionUsagePairs.begin(), actionUsagePairs.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // Take top actions for curiosity-driven exploration
    size_t numCuriosityActions = std::min(static_cast<size_t>(3), actionUsagePairs.size());
    for (size_t i = 0; i < numCuriosityActions; ++i) {
        curiosityActions.push_back(static_cast<ActionType>(actionUsagePairs[i].first));
    }
    
    return curiosityActions;
}

PlanningCandidate NeuralPlanner::planWithRL(const std::vector<float>& currentState,
                                           const PlanningContext& context) {
    NLM_LOG_INFO("Planning with reinforcement learning algorithms");
    
    PlanningCandidate candidate;
    candidate.actions.clear();
    candidate.predictedStates.clear();
    candidate.expectedReward = 0.0f;
    candidate.confidence = 0.0f;
    candidate.depth = 0;
    
    // Monte Carlo Tree Search (simplified)
    std::vector<std::vector<ActionType>> candidates = generateActionSequences(planningDepth_);
    
    // Filter candidates based on action quality
    std::vector<std::vector<ActionType>> filteredCandidates;
    for (const auto& sequence : candidates) {
        bool isGood = true;
        float sequenceValue = 0.0f;
        
        for (ActionType action : sequence) {
            size_t actionIdx = static_cast<size_t>(action);
            if (actionIdx < actionQuality_.size() && actionQuality_[actionIdx] < -0.5f) {
                isGood = false;
                break;
            }
            
            sequenceValue += actionQuality_[actionIdx] > 0.0f ? 1.0f : -1.0f;
        }
        
        if (isGood && sequenceValue > 0.0f) {
            filteredCandidates.push_back(sequence);
        }
    }
    
    // Select best candidate using epsilon-greedy strategy
    float explorationRate = trialErrorStats_.explorationRate;
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    for (const auto& sequence : filteredCandidates) {
        PlanningCandidate rlCandidate = evaluateSequence(sequence, currentState);
        
        // Add exploration bonus for curiosity-driven planning
        if (context.curiosityLevel > 0.0f && context.developmentStage == DevelopmentalStage::Maturation) {
            rlCandidate.expectedReward += context.curiosityLevel * 0.5f;
        }
        
        if (rlCandidate.expectedReward > best.expectedReward) {
            best = rlCandidate;
        }
    }
    
    // Fallback to random action if no good candidates found
    if (best.actions.empty()) {
        auto curiosityActions = generateCuriosityDrivenActions(currentState, context);
        if (!curiosityActions.empty()) {
            candidate.actions = { curiosityActions[0] };
            candidate.expectedReward = actionQuality_[static_cast<size_t>(curiosityActions[0])];
        }
    } else {
        candidate = best;
    }
    
    return candidate;
}

float NeuralPlanner::getNeuromodulationGating(NeuromodulatorType type) const {
    auto it = neuromodulationGating_.find(type);
    if (it != neuromodulationGating_.end()) {
        return it->second;
    }
    return 1.0f;
}

void NeuralPlanner::applyNeuromodulationGating(PlanningContext& context) const {
    // Apply neuromodulation effects to planning
    for (const auto& pair : neuromodulationGating_) {
        switch (pair.first) {
            case NeuromodulatorType::DOPAMINE:
                // Dopamine increases planning confidence and reward seeking
                context.rewardEstimate *= pair.second * 1.2f;
                context.curiosityLevel *= pair.second * 1.1f;
                break;
            case NeuromodulatorType::ACETYLCHOLINE:
                // Acetylcholine increases prediction confidence
                context.curiosityLevel *= pair.second;
                break;
            case NeuromodulatorType::SEROTONIN:
                // Serotonin decreases exploration, increases stability
                context.curiosityLevel *= std::max(0.1f, pair.second * 0.8f);
                break;
            case NeuromodulatorType::NORADRENALINE:
                // Noradrenaline increases alertness and attention
                context.curiosityLevel *= pair.second * 1.1f;
                break;
            case NeuromodulatorType::ENDORPHIN:
                // Endorphin reduces stress in planning
                context.curiosityLevel *= pair.second;
                break;
        }
    }
}

void NeuralPlanner::applyTrialErrorLearning(const std::vector<ActionType>& actions,
                                           float reward,
                                           const PlanningContext& context) {
    NLM_LOG_INFO("Applying trial-and-error learning to refine action sequences");
    
    // Update action sequence in trial-and-error stats
    trialErrorStats_.actionSequence = actions;
    trialErrorStats_.rewards.push_back(reward);
    
    // Calculate learning rate based on prediction error
    float predictionError = context.predictionError;
    float effectiveLearningRate = trialErrorStats_.learningRate * (1.0f + predictionError);
    
    // Update action quality using trial-and-error results
    for (size_t i = 0; i < std::min(actions.size(), actionQuality_.size()); ++i) {
        size_t actionIdx = static_cast<size_t>(actions[i]);
        if (actionIdx < actionQuality_.size()) {
            // Update based on recent reward history
            float recentReward = 0.0f;
            size_t rewardStart = std::max(0LL, static_cast<long>(trialErrorStats_.rewards.size()) - 10);
            for (size_t j = rewardStart; j < trialErrorStats_.rewards.size(); ++j) {
                recentReward += trialErrorStats_.rewards[j];
            }
            if (!trialErrorStats_.rewards.empty()) {
                recentReward /= (trialErrorStats_.rewards.size() - rewardStart);
            }
            
            // Update action quality
            float delta = effectiveLearningRate * (recentReward - actionQuality_[actionIdx]);
            actionQuality_[actionIdx] = std::clamp(actionQuality_[actionIdx] + delta, -1.0f, 1.0f);
        }
    }
    
    // Update exploration rate
    trialErrorStats_.explorationRate = std::min(0.5f, trialErrorStats_.explorationRate + 0.01f);
}

float NeuralPlanner::evaluateTransferLearning(const std::vector<ActionType>& actions,
                                             const std::vector<float>& startState,
                                             const std::string& taskId) const {
    // Check transfer memory for similar tasks
    float transferBonus = 0.0f;
    
    for (const auto& transfer : transferMemory_) {
        if (transfer.taskSignature == taskId) {
            // Exact task match - full transfer
            transferBonus = transfer.successProbability;
            break;
        }
        
        // Similar task - partial transfer
        if (areTasksSimilar(taskId, transfer.taskSignature)) {
            transferBonus = transfer.successProbability * 0.7f;
        }
    }
    
    return transferBonus;
}

float NeuralPlanner::evaluateHierarchicalGoal(const HierarchicalGoal& goal,
                                            const std::vector<float>& currentState) const {
    // Evaluate hierarchical goal based on abstraction level
    float goalSimilarity = 0.0f;
    
    if (!currentGoal_.empty() && goal.goalVector.size() == currentGoal_.size()) {
        for (size_t i = 0; i < goal.goalVector.size(); ++i) {
            goalSimilarity += goal.goalVector[i] * currentGoal_[i];
        }
    }
    
    // Apply abstraction level weighting
    float abstractionFactor = 1.0f - goal.abstractionLevel;
    float effectiveSimilarity = goalSimilarity * abstractionFactor;
    
    // Boost goal value for higher priority
    float priorityBoost = static_cast<float>(goal.priority) * 0.1f;
    
    return effectiveSimilarity + priorityBoost;
}

StrategyType NeuralPlanner::selectStrategy(const PlanningContext& context) const {
    // Select best strategy based on performance and context
    StrategyType selectedStrategy = StrategyType::Developmental;
    
    if (!strategyPerformance_.empty()) {
        // Find best performing strategy
        float bestPerformance = -1.0f;
        for (const auto& pair : strategyPerformance_) {
            if (pair.second > bestPerformance) {
                bestPerformance = pair.second;
                selectedStrategy = pair.first;
            }
        }
    }
    
    // Adjust based on developmental stage
    if (context.developmentStage == DevelopmentalStage::CriticalPeriod) {
        selectedStrategy = StrategyType::MonteCarlo;
    } else if (context.developmentStage == DevelopmentalStage::Maturation) {
        selectedStrategy = StrategyType::Evolutionary;
    }
    
    // Adjust based on curiosity level
    if (context.curiosityLevel > 0.7f) {
        selectedStrategy = StrategyType::CuriosityDriven;
    }
    
    return selectedStrategy;
}

void NeuralPlanner::switchStrategy(StrategyType newStrategy, PlanningContext& context) {
    NLM_LOG_INFO("Switching planning strategy to: %d", static_cast<int>(newStrategy));
    
    // Record strategy switch
    context.currentStrategy = newStrategy;
    strategyHistory_.push_back(newStrategy);
    
    // Keep history bounded
    if (strategyHistory_.size() > 100) {
        strategyHistory_.erase(strategyHistory_.begin());
    }
    
    // Update planning parameters based on strategy
    switch (newStrategy) {
        case StrategyType::RandomSearch:
            planningDepth_ = 1;
            trialErrorStats_.explorationRate = 0.8f;
            break;
        case StrategyType::Greedy:
            planningDepth_ = 2;
            trialErrorStats_.explorationRate = 0.2f;
            break;
        case StrategyType::TreeSearch:
            planningDepth_ = 4;
            trialErrorStats_.explorationRate = 0.3f;
            break;
        case StrategyType::MonteCarlo:
            planningDepth_ = 3;
            trialErrorStats_.explorationRate = 0.5f;
            break;
        case StrategyType::Evolutionary:
            planningDepth_ = 5;
            trialErrorStats_.explorationRate = 0.6f;
            break;
        case StrategyType::CuriosityDriven:
            planningDepth_ = 3;
            trialErrorStats_.explorationRate = 0.9f;
            break;
        case StrategyType::Developmental:
            planningDepth_ = 3;
            trialErrorStats_.explorationRate = 0.4f;
            break;
    }
}

void NeuralPlanner::evaluateStrategyPerformance(StrategyType strategy, float reward,
                                                PlanningContext& context) {
    // Update strategy performance based on reward
    strategyPerformance_[strategy] = (strategyPerformance_[strategy] * 0.9f) + 
                                     (reward * 0.1f);
    
    // Record in working memory for strategy selection
    recordStrategyPerformance(strategy, reward, context);
}

void NeuralPlanner::initializeAdvancedPlanning() {
    // Re-initialize all advanced planning systems
    Neuromodulator::initializeAdvancedPlanning();
    initializeTransferLearning();
    initializeHierarchicalGoals();
    initializeStrategyTracking();
}

void NeuralPlanner::initializeTransferLearning() {
    transferMemory_.clear();
    NLM_LOG_INFO("Transfer learning system initialized");
}

void NeuralPlanner::initializeHierarchicalGoals() {
    hierarchicalGoals_.clear();
    NLM_LOG_INFO("Hierarchical goal system initialized");
}

void NeuralPlanner::initializeStrategyTracking() {
    strategyPerformance_.clear();
    strategyHistory_.clear();
    NLM_LOG_INFO("Strategy tracking system initialized");
}

std::vector<ActionType> NeuralPlanner::getAllActions() const {
    return { ActionType::MoveForward, ActionType::MoveBackward,
             ActionType::TurnLeft, ActionType::TurnRight,
             ActionType::Interact, ActionType::Wait };
}

void NeuralPlanner::applyBackwardEffect(ActionType action, std::vector<float>& goal) const {
    // Apply backward effect to goal state
    switch (action) {
        case ActionType::MoveForward:
            for (auto& v : goal) {
                v = std::max(0.0f, v - 0.1f);
            }
            break;
        case ActionType::MoveBackward:
            for (auto& v : goal) {
                v = std::min(1.0f, v + 0.1f);
            }
            break;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            for (auto& v : goal) {
                v += 0.05f * (v > 0.5f ? -1.0f : 1.0f);
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
        case ActionType::Interact:
            for (auto& v : goal) {
                v = 0.5f;
            }
            break;
        case ActionType::Wait:
        default:
            // No change
            break;
    }
}

bool NeuralPlanner::isCompatibleWithWorkingMemory(const std::vector<ActionType>& actions,
                                                  const std::vector<float>& wmContent) const {
    // Check if action sequence is compatible with working memory content
    for (ActionType action : actions) {
        // Simple compatibility check - can be enhanced
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < 10) {
            // Actions 0-9 are generally compatible
        }
    }
    return true;
}

float NeuralPlanner::computeWorkingMemoryRetentionBonus(const std::vector<ActionType>& actions,
                                                        const std::vector<float>& wmContent) const {
    // Compute bonus for maintaining working memory content
    return 0.0f;  // Simplified for now
}

void NeuralPlanner::storePlanInWorkingMemory(const std::vector<ActionType>& actions,
                                             const PlanningContext& context) {
    // Store plan in working memory for later recall
    NLM_LOG_INFO("Storing plan in working memory for recall");
}

void NeuralPlanner::recordPredictionHistory(const std::vector<ActionType>& actions,
                                           const std::vector<float>& predictedState,
                                           const std::vector<float>& predictionErrors,
                                           const PlanningContext& context) {
    // Record prediction history for learning
    forwardPredictionModels_.push_back(predictedState);
    
    if (forwardPredictionModels_.size() > 100) {
        forwardPredictionModels_.erase(forwardPredictionModels_.begin());
    }
}

float NeuralPlanner::computePredictionError(const std::vector<float>& actual,
                                           const std::vector<float>& predicted,
                                           const std::vector<ActionType>& actions) const {
    if (actual.size() != predicted.size()) return 1.0f;
    
    float error = 0.0f;
    for (size_t i = 0; i < actual.size(); ++i) {
        error += std::abs(actual[i] - predicted[i]);
    }
    
    return error / actual.size();
}

bool NeuralPlanner::areTasksSimilar(const std::string& task1,
                                    const std::string& task2) const {
    // Simple task similarity check
    // In real implementation, this would use more sophisticated similarity metrics
    return (task1.substr(0, 3) == task2.substr(0, 3));
}

void NeuralPlanner::recordStrategyPerformance(StrategyType strategy, float reward,
                                              const PlanningContext& context) {
    // Record strategy performance for analysis
    NLM_LOG_INFO("Strategy %d performance: %.3f (reward: %.3f)",
                 static_cast<int>(strategy), reward, context.rewardEstimate);
}

} // namespace nlm
