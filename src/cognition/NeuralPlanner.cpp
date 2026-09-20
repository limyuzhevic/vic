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

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralPlanner initialized");
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
        // Note: SelfModel reference kept for backward compatibility
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> result;
    
    // Simplified action set - reduce combinatorial explosion
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // Limit depth to prevent excessive sequences
    size_t effectiveDepth = std::min(depth, static_cast<size_t>(3));
    
    std::vector<ActionType> current;
    
    // Recursive generation
    std::function<void(size_t)> generate = [&](size_t remaining) {
        if (remaining == 0) {
            result.push_back(current);
            return;
        }
        
        for (ActionType action : baseActions) {
            current.push_back(action);
            generate(remaining - 1);
            current.pop_back();
        }
    };
    
    generate(effectiveDepth);
    
    // Limit total sequences to maintain efficiency
    if (result.size() > 50) {
        result.resize(50);
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
    return currentState;  // Simplified prediction
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