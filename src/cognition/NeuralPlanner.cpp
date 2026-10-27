#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    // Planning neural networks
    std::vector<std::vector<float>> planningWeights;
    std::vector<std::vector<NeuronId>> planningNeurons;
    
    // Active planning buffer
    std::vector<PlanningCandidate> activePlans;
    
    // Planning confidence adjustments
    std::vector<float> planningConfidence;
    
    // Goal representation in neural space
    std::vector<float> goalVector;
    
    // Planning for different time horizons
    std::vector<size_t> actionSequences;
    
    Impl() : brain(nullptr), planningDepth_(3) {
        planningWeights.reserve(100);
        planningNeurons.reserve(100);
        planningConfidence.reserve(100);
    }
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(10, 0.0f);  // 10 action types
    recentPlanSuccess_.reserve(50);
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize planning neural networks
    initializePlanningNetworks();
    
    NLM_LOG_INFO("NeuralPlanner initialized with neural planning dynamics");
}

void NeuralPlanner::initializePlanningNetworks() {
    if (!brain_) return;
    
    // Create dedicated planning regions in brain
    pImpl->planningNeurons.clear();
    pImpl->planningWeights.clear();
    pImpl->planningConfidence.clear();
    
    // Create 2 dedicated planning regions
    for (int i = 0; i < 2; ++i) {
        RegionId regionId = brain_->addRegion("Planning_" + std::to_string(i));
        std::vector<NeuronId> regionNeurons;
        
        // Initialize planning neurons in this region
        for (int j = 0; j < 50; ++j) {  // 50 neurons per planning region
            NeuronId neuronId(regionId.index() * 1000 + j + 100);
            regionNeurons.push_back(neuronId);
        }
        
        pImpl->planningNeurons.push_back(regionNeurons);
        
        // Initialize planning weights for this region
        std::vector<float> regionWeights(50, 0.0f);
        pImpl->planningWeights.push_back(regionWeights);
        pImpl->planningConfidence.push_back(0.5f);
    }
    
    // Initialize action quality based on neuromodulation
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        for (size_t i = 0; i < actionQuality_.size(); ++i) {
            // Dopamine level influences action value
            actionQuality_[i] = dopamine->getLevel() * 0.5f;
        }
    }
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                    float targetReward) {
    if (!brain_) return ActionType::Wait;
    
    // Get working memory for current context
    auto* workingMem = brain_->getWorkingMemory();
    if (!workingMem) {
        return ActionType::Wait;
    }
    
    // Generate planning candidates using neural dynamics
    pImpl->activePlans.clear();
    generateNeuralPlans(currentState, targetReward);
    
    // Evaluate plans using prediction system
    evaluatePlanningCandidates(currentState);
    
    // Select best action based on neural activation
    ActionType bestAction = selectBestAction(currentState);
    
    // Record plan success based on achieved reward
    if (bestAction != ActionType::Wait) {
        recentPlanSuccess_.push_back(true); // Will be updated when actual outcome known
    } else {
        recentPlanSuccess_.push_back(false);
    }
    
    if (recentPlanSuccess_.size() > 20) {
        recentPlanSuccess_.pop_front();
    }
    
    NLM_LOG_INFO("Planning completed: selected " + std::to_string(static_cast<size_t>(bestAction)) + 
                 ", confidence=" + std::to_string(planningConfidence_));
    
    return bestAction;
}

void NeuralPlanner::generateNeuralPlans(const std::vector<float>& currentState,
                                       float targetReward) {
    if (!brain_ || currentState.empty()) return;
    
    // Get episodic memory for relevant experiences
    auto* episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        // Get relevant episodes for planning
        auto relevantEpisodes = episodicMem->getRecentEpisodes(5);
        
        for (const auto* episode : relevantEpisodes) {
            // Create planning candidate from successful episode pattern
            PlanningCandidate candidate;
            
            // Extract action sequence from episode
            std::vector<ActionType> actions;
            for (const auto* ep : relevantEpisodes) {
                if (ep->action != ActionType::Wait) {
                    actions.push_back(ep->action);
                }
            }
            
            candidate.actions = actions;
            candidate.expectedReward = episode->reward;
            candidate.confidence = 1.0f / (1.0f + episode->age);  // More recent = higher confidence
            candidate.depth = actions.size();
            
            // Generate predicted states based on episode
            std::vector<std::vector<float>> predictedStates;
            for (const auto& epAct : relevantEpisodes) {
                if (!epAct->sensoryState.empty()) {
                    predictedStates.push_back(epAct->sensoryState);
                }
            }
            
            candidate.predictedStates = predictedStates;
            
            // Add to active plans
            pImpl->activePlans.push_back(candidate);
        }
    }
    
    // Generate additional plans using neural prediction
    generatePredictivePlans(currentState, targetReward);
}

void NeuralPlanner::generatePredictivePlans(const std::vector<float>& currentState,
                                           float targetReward) {
    if (!brain_ || currentState.empty()) return;
    
    // Get prediction system for forward model
    auto* predSys = brain_->getPredictionSystem();
    if (!predSys) return;
    
    // Generate action sequences using neural dynamics
    std::vector<std::vector<ActionType>> actionSequences;
    
    // Simple action sequences for now (would use neural dynamics in full implementation)
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // Generate sequences of different depths
    for (size_t depth = 1; depth <= planningDepth_; ++depth) {
        std::function<void(std::vector<ActionType>&, size_t)> generate = 
            [&](std::vector<ActionType>& current, size_t remaining) {
                if (remaining == 0) {
                    actionSequences.push_back(current);
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
    }
    
    // Limit number of sequences
    if (actionSequences.size() > 100) {
        actionSequences.resize(100);
    }
    
    // Create planning candidates from sequences
    for (const auto& sequence : actionSequences) {
        PlanningCandidate candidate;
        candidate.actions = sequence;
        candidate.depth = sequence.size();
        
        // Evaluate sequence using neural prediction
        evaluateSequenceNeural(candidate, currentState, targetReward);
        
        // Add to active plans
        pImpl->activePlans.push_back(candidate);
    }
}

void NeuralPlanner::evaluatePlanningCandidates(const std::vector<float>& currentState) {
    if (pImpl->activePlans.empty()) return;
    
    float bestReward = -1000.0f;
    
    for (auto& candidate : pImpl->activePlans) {
        // Neural evaluation
        float neuralValue = evaluateCandidateNeural(candidate, currentState);
        
        // Combine with prediction system if available
        float predValue = 0.0f;
        if (brain_->getPredictionSystem()) {
            predValue = neuralValue * 0.7f + candidate.expectedReward * 0.3f;
        } else {
            predValue = neuralValue;
        }
        
        // Update confidence based on consistency with working memory
        float wmConsistency = computeWorkingMemoryConsistency(candidate);
        candidate.confidence = predValue * 0.7f + wmConsistency * 0.3f;
        
        // Update expected reward
        candidate.expectedReward = predValue;
        
        // Update planning confidence
        planningConfidence_ = std::max(planningConfidence_, candidate.confidence);
        
        if (candidate.expectedReward > bestReward) {
            bestReward = candidate.expectedReward;
        }
    }
}

ActionType NeuralPlanner::selectBestAction(const std::vector<float>& currentState) {
    if (pImpl->activePlans.empty()) return ActionType::Wait;
    
    // Find best plan using neural competition
    PlanningCandidate bestPlan;
    bestPlan.expectedReward = -1000.0f;
    
    for (const auto& candidate : pImpl->activePlans) {
        if (candidate.expectedReward > bestPlan.expectedReward) {
            bestPlan = candidate;
        }
    }
    
    if (bestPlan.actions.empty()) {
        return ActionType::Wait;
    }
    
    // Select best action (first in sequence for now)
    ActionType bestAction = bestPlan.actions[0];
    
    // Apply neuromodulatory influence
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        
        // Dopamine increases value of actions leading to reward
        if (bestAction != ActionType::Wait) {
            // Apply action selection bias
            size_t actionIdx = static_cast<size_t>(bestAction);
            if (actionIdx < actionQuality_.size()) {
                actionQuality_[actionIdx] += dopamineLevel * 0.1f;
                actionQuality_[actionIdx] = std::clamp(actionQuality_[actionIdx], -1.0f, 1.0f);
            }
        }
    }
    
    // Update planning confidence based on action value
    planningConfidence_ = bestPlan.confidence;
    
    return bestAction;
}

void NeuralPlanner::evaluateSequenceNeural(PlanningCandidate& candidate,
                                          const std::vector<float>& startState,
                                          float targetReward) {
    if (!brain_) return;
    
    std::vector<float> currentState = startState;
    float totalReward = 0.0f;
    float confidence = 1.0f;
    
    for (size_t i = 0; i < candidate.actions.size() && i < planningDepth_; ++i) {
        ActionType action = candidate.actions[i];
        
        // Evaluate action using neural networks
        float actionValue = evaluateActionNeural(action, currentState);
        
        // Predict next state using prediction system
        auto* predSys = brain_->getPredictionSystem();
        if (predSys) {
            // In real implementation, would use prediction system to predict next state
            // For now, use simple dynamics
            currentState = predictNextStateNeural(action, currentState);
        }
        
        // Add goal-directed component
        float goalValue = computeGoalValue(currentState, targetReward);
        
        // Combine values
        float combinedValue = actionValue * 0.6f + goalValue * 0.4f;
        
        totalReward += combinedValue;
        
        // Update confidence based on neuromodulators
        float confidenceModifier = 1.0f;
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) {
            confidenceModifier += curiosity->getLevel() * 0.1f;
        }
        
        confidence *= confidenceModifier;
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    candidate.predictedStates.push_back(currentState);
}

float NeuralPlanner::evaluateActionNeural(ActionType action,
                                         const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Get action quality from experience
    float baseValue = actionQuality_[actionIdx];
    
    // Get neural representation of action from planning networks
    float neuralRepresentation = 0.0f;
    if (!pImpl->planningWeights.empty()) {
        // Use planning weights to compute neural value
        for (size_t i = 0; i < std::min(state.size(), pImpl->planningWeights[0].size()); ++i) {
            neuralRepresentation += state[i] * pImpl->planningWeights[0][i];
        }
    }
    
    // Combine base value with neural representation
    return baseValue * 0.5f + neuralRepresentation * 0.5f;
}

std::vector<float> NeuralPlanner::predictNextStateNeural(ActionType action,
                                                       const std::vector<float>& currentState) {
    // Enhanced forward model using prediction system
    if (!brain_) return currentState;
    
    std::vector<float> nextState = currentState;
    
    // Apply action-specific dynamics
    switch (action) {
        case ActionType::MoveForward:
            // Shift and dampen sensory input
            if (nextState.size() > 10) {
                for (size_t i = nextState.size() - 1; i > 0; --i) {
                    nextState[i] = nextState[i-1] * 0.9f;
                }
                nextState[0] = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
            }
            break;
        case ActionType::MoveBackward:
            if (nextState.size() > 10) {
                for (size_t i = 1; i < nextState.size(); ++i) {
                    nextState[i] = nextState[i+1] * 0.9f;
                }
                nextState[nextState.size() - 1] = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
            }
            break;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotate pattern with some noise
            for (float& val : nextState) {
                val = std::clamp(val + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f, 0.0f, 1.0f);
            }
            break;
        case ActionType::Interact:
            // Interaction causes significant state change
            for (float& val : nextState) {
                val = 0.5f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.6f;
            }
            break;
        case ActionType::Wait:
        default:
            // No change, just add some noise
            for (float& val : nextState) {
                val = std::clamp(val + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f, 0.0f, 1.0f);
            }
            break;
    }
    
    return nextState;
}

float NeuralPlanner::computeGoalValue(const std::vector<float>& state,
                                    float targetReward) {
    if (currentGoal_.empty() || state.size() != currentGoal_.size()) {
        return 0.0f;
    }
    
    // Compute distance to goal using cosine similarity
    float similarity = 0.0f, stateNorm = 0.0f, goalNorm = 0.0f;
    for (size_t i = 0; i < state.size(); ++i) {
        similarity += state[i] * currentGoal_[i];
        stateNorm += state[i] * state[i];
        goalNorm += currentGoal_[i] * currentGoal_[i];
    }
    
    if (stateNorm < 0.0001f || goalNorm < 0.0001f) {
        return 0.0f;
    }
    
    float distanceToGoal = 1.0f - similarity / (std::sqrt(stateNorm) * std::sqrt(goalNorm));
    
    // Return reward based on proximity to goal
    return (1.0f - distanceToGoal) * targetReward;
}

float NeuralPlanner::evaluateCandidateNeural(const PlanningCandidate& candidate,
                                           const std::vector<float>& currentState) const {
    if (candidate.actions.empty()) return 0.0f;
    
    // Evaluate based on neural representation and confidence
    float value = candidate.expectedReward * 0.7f + candidate.confidence * 0.3f;
    
    // Adjust based on working memory consistency
    float wmConsistency = computeWorkingMemoryConsistency(candidate);
    value += wmConsistency * 0.2f;
    
    return value;
}

float NeuralPlanner::computeWorkingMemoryConsistency(const PlanningCandidate& candidate) const {
    if (!brain_) return 0.0f;
    
    auto* workingMem = brain_->getWorkingMemory();
    if (!workingMem) return 0.5f;
    
    // Get current working memory state
    std::vector<float> wmState = workingMem->retrieve();
    
    if (wmState.empty()) return 0.5f;
    
    // Check consistency with candidate's predicted states
    float totalConsistency = 0.0f;
    int count = 0;
    
    for (const auto& predState : candidate.predictedStates) {
        if (predState.size() == wmState.size()) {
            float consistency = 0.0f;
            for (size_t i = 0; i < predState.size(); ++i) {
                consistency += std::abs(predState[i] - wmState[i]);
            }
            consistency = 1.0f - (consistency / predState.size());  // Invert to get consistency
            totalConsistency += consistency;
            ++count;
        }
    }
    
    return count > 0 ? totalConsistency / count : 0.5f;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                    const std::vector<ActionType>& actualActions,
                                    float actualReward) {
    // Update planning neural networks based on actual outcomes
    if (!brain_) return;
    
    // Update action quality based on success/failure
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    for (size_t i = 0; i < minLen; ++i) {
        size_t actionIdx = static_cast<size_t>(actualActions[i]);
        if (actionIdx < actionQuality_.size()) {
            // Positive update if action matched plan and reward was good
            float delta = (actualActions[i] == plannedActions[i]) ? 0.1f : -0.05f;
            delta *= actualReward;
            
            actionQuality_[actionIdx] = std::clamp(
                actionQuality_[actionIdx] + delta, -1.0f, 1.0f);
        }
    }
    
    // Update planning weights based on prediction error
    updatePlanningWeights(actualReward);
    
    NLM_LOG_INFO("Plan quality updated: actual reward=" + std::to_string(actualReward));
}

void NeuralPlanner::updatePlanningWeights(float reward) {
    if (!brain_ || pImpl->planningWeights.empty()) return;
    
    // Hebbian-like update of planning weights based on reward
    for (auto& weights : pImpl->planningWeights) {
        for (auto& weight : weights) {
            // Stronger weights for actions that led to good rewards
            float delta = reward * 0.01f;
            weight += delta;
            
            // Clamp weights
            weight = std::clamp(weight, -1.0f, 1.0f);
        }
    }
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return true;  // No data
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) ++successCount;
    }
    
    return successCount > recentPlanSuccess_.size() / 2;
}

} // namespace nlm
