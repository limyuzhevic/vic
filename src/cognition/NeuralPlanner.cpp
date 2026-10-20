// NeuralPlanner implementation
#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralPlanner::Impl {
    float explorationRate;
    float exploitationRate;
    float confidenceThreshold;
    size_t maxSequenceLength;
    
    Impl() : explorationRate(0.3f), exploitationRate(0.7f), confidenceThreshold(0.5f), maxSequenceLength(5) {}
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(static_cast<size_t>(ActionType::Count), 0.5f);
    recentPlanSuccess_.clear();
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("NeuralPlanner initialized");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                    float targetReward) {
    if (!brain_) return ActionType::Wait;
    
    // Generate all possible action sequences
    std::vector<std::vector<ActionType>> sequences = generateActionSequences(planningDepth_);
    
    if (sequences.empty()) return ActionType::Wait;
    
    // Evaluate each sequence
    std::vector<PlanningCandidate> candidates;
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        candidates.push_back(candidate);
    }
    
    // Sort by expected reward
    std::sort(candidates.begin(), candidates.end(),
              [](const PlanningCandidate& a, const PlanningCandidate& b) {
                  return a.expectedReward > b.expectedReward;
              });
    
    if (candidates.empty() || candidates[0].expectedReward < 0) {
        return ActionType::Wait;
    }
    
    // Select action based on planning confidence
    float confidence = candidates[0].confidence;
    planningConfidence_ = (planningConfidence_ * 0.9f) + (confidence * 0.1f);
    
    // Apply exploration/exploitation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    float randomValue = dis(gen);
    if (randomValue < pImpl->explorationRate) {
        // Explore: choose action with highest confidence difference
        float bestConfidence = -1.0f;
        ActionType bestAction = ActionType::Wait;
        
        for (size_t i = 0; i < sequences.size(); ++i) {
            if (i == 0) continue;  // Skip best sequence
            
            float confidenceDiff = candidates[i].confidence - candidates[0].confidence;
            if (confidenceDiff > bestConfidence) {
                bestConfidence = confidenceDiff;
                bestAction = sequences[i][0];
            }
        }
        
        if (bestAction != ActionType::Wait) return bestAction;
    }
    
    // Exploit: choose best action
    return sequences[0][0];
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                const std::vector<float>& startState) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = actions.size();
    
    float totalReward = 0.0f;
    float totalConfidence = 0.0f;
    
    // Simulate action sequence
    std::vector<float> currentState = startState;
    for (ActionType action : actions) {
        // Estimate reward for this action
        float actionReward = evaluateAction(action, currentState);
        
        totalReward += actionReward;
        totalConfidence += actionQuality_[static_cast<size_t>(action)];
        
        // Update state (simplified)
        for (size_t i = 0; i < currentState.size(); ++i) {
            currentState[i] += actionReward * 0.1f;
            currentState[i] = std::clamp(currentState[i], -10.0f, 10.0f);
        }
    }
    
    if (!actions.empty()) {
        candidate.expectedReward = totalReward / actions.size();
        candidate.confidence = totalConfidence / actions.size();
    }
    
    return candidate;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                    const std::vector<ActionType>& actualActions,
                                    float actualReward) {
    // Update action quality based on actual outcomes
    for (ActionType action : plannedActions) {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            // Increase quality if action was chosen
            actionQuality_[actionIdx] = (actionQuality_[actionIdx] * 0.8f) + (0.4f * actualReward);
        }
    }
    
    // Record recent success
    bool planSuccessful = (plannedActions == actualActions) && 
                         (actualReward > 0.5f);
    recentPlanSuccess_.push_back(planSuccessful);
    
    // Keep only recent history
    if (recentPlanSuccess_.size() > 20) {
        recentPlanSuccess_.pop_front();
    }
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return false;
    
    int successful = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) successful++;
    }
    
    return (successful / static_cast<float>(recentPlanSuccess_.size())) > 0.6f;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> sequences;
    
    // Generate all possible sequences of actions
    std::vector<ActionType> actions = {
        ActionType::Wait,
        ActionType::MoveForward,
        ActionType::MoveBackward,
        ActionType::TurnLeft,
        ActionType::TurnRight
    };
    
    if (depth == 0) return sequences;
    
    // Generate sequences recursively
    std::function<void(std::vector<ActionType>, size_t)> generate = [&](std::vector<ActionType> current, size_t remaining) {
        if (remaining == 0) {
            sequences.push_back(current);
            return;
        }
        
        for (ActionType action : actions) {
            std::vector<ActionType> newSequence = current;
            newSequence.push_back(action);
            generate(newSequence, remaining - 1);
        }
    };
    
    generate({}, depth);
    
    // Limit to reasonable number
    if (sequences.size() > 1000) {
        sequences.resize(1000);
    }
    
    return sequences;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    // Evaluate action quality based on state
    size_t actionIdx = static_cast<size_t>(action);
    
    float baseQuality = actionQuality_[actionIdx];
    
    // Modulate based on state (simple heuristic)
    float stateMod = 0.0f;
    for (size_t i = 0; i < state.size(); ++i) {
        stateMod += std::abs(state[i]);
    }
    stateMod /= state.size();
    
    // Actions are better in medium activity states
    float statePreference = 1.0f - std::abs(stateMod - 0.5f) * 2.0f;
    
    return baseQuality * (0.7f + statePreference * 0.3f);
}

} // namespace nlm