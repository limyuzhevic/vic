#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <limits>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    // MCTS state
    struct MCTSNode {
        std::vector<ActionType> actionSequence;
        std::vector<float> state;
        float totalReward;
        int visits;
        float value;
        float confidence;
        std::vector<MCTSNode*> children;
        MCTSNode* parent;
        ActionType lastAction;
        
        MCTSNode(const std::vector<ActionType>& seq, const std::vector<float>& s, MCTSNode* parent = nullptr)
            : actionSequence(seq), state(s), totalReward(0.0f), visits(0), value(0.0f), confidence(1.0f),
              parent(parent), lastAction(ActionType::Wait) {}
    };
    
    std::vector<MCTSNode*> mctsRoot;
    size_t mctsMaxDepth;
    float explorationConstant;
    float mctsTimeBudget;
    
    // Planning statistics
    size_t totalPlansGenerated;
    size_t successfulPlans;
    float averagePlanQuality;
    
    Impl() : brain(nullptr), mctsMaxDepth(5), explorationConstant(1.5f), mctsTimeBudget(100.0f),
             totalPlansGenerated(0), successfulPlans(0), averagePlanQuality(0.0f) {}
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
{
    actionQuality_.resize(10, 0.0f);
    
    // Initialize MCTS parameters
    pImpl->explorationConstant = 1.5f;
    pImpl->mctsMaxDepth = 5;
    pImpl->mctsTimeBudget = 100.0f;
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralPlanner initialized with MCTS and advanced planning");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                     float targetReward) {
    // Implement multiple planning strategies based on current brain state
    ActionType selectedAction = ActionType::Wait;
    
    // Choose planning strategy based on available resources and context
    if (pImpl->brain && pImpl->brain->getPredictionSystem()) {
        // Use MCTS for complex planning
        selectedAction = monteCarloTreeSearch(currentState, targetReward);
    } else {
        // Fallback to simpler heuristic planning
        selectedAction = heuristicPlanning(currentState, targetReward);
    }
    
    // Record planning decision
    pImpl->totalPlansGenerated++;
    if (selectedAction != ActionType::Wait) {
        pImpl->successfulPlans++;
    }
    
    // Update planning confidence based on plan success
    updatePlanningConfidence();
    
    return selectedAction;
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
        
        // Predict next state using forward model
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

void NeuralPlanner::implementMonteCarloTreeSearch(const std::vector<float>& currentState,
                                                float targetReward,
                                                ActionType& selectedAction) {
    // MCTS implementation for planning
    pImpl->mctsRoot.clear();
    
    // Create root node
    pImpl->mctsRoot.push_back(new Impl::MCTSNode({}, currentState, nullptr));
    
    // Run MCTS iterations
    size_t iterations = static_cast<size_t>(pImpl->mctsTimeBudget / 10.0f); // Time budget
    
    for (size_t iter = 0; iter < iterations && !pImpl->mctsRoot.empty(); ++iter) {
        // Selection
        Impl::MCTSNode* leaf = selectNode(pImpl->mctsRoot[0]);
        
        // Expansion
        if (!isTerminalNode(leaf) && leaf->depth < pImpl->mctsMaxDepth) {
            expandNode(leaf);
        }
        
        // Simulation (rollout)
        float simulatedReward = simulateRollout(leaf);
        
        // Backpropagation
        backpropagateResult(leaf, simulatedReward);
    }
    
    // Extract best action from root
    if (!pImpl->mctsRoot.empty()) {
        selectedAction = extractBestAction(pImpl->mctsRoot[0]);
    }
    
    // Clean up MCTS nodes
    cleanupMCTSNodes(pImpl->mctsRoot[0]);
}

ActionType NeuralPlanner::monteCarloTreeSearch(const std::vector<float>& currentState,
                                              float targetReward) {
    ActionType selectedAction = ActionType::Wait;
    
    // Primary MCTS planning
    implementMonteCarloTreeSearch(currentState, targetReward, selectedAction);
    
    if (selectedAction == ActionType::Wait) {
        // Fallback to heuristic planning
        selectedAction = heuristicPlanning(currentState, targetReward);
    }
    
    return selectedAction;
}

void NeuralPlanner::implementNeuroEvolutionPlanning() {
    if (!brain_) return;
    
    // Simple neuro-evolution: evolve action selection strategies over time
    static float evolutionRate = 0.01f;
    
    // Update action quality based on evolutionary pressure
    for (size_t i = 0; i < actionQuality_.size(); ++i) {
        // Add small mutation noise
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * evolutionRate;
        actionQuality_[i] += noise;
        actionQuality_[i] = std::clamp(actionQuality_[i], -1.0f, 1.0f);
    }
    
    // Occasionally replace entire action strategy
    if (rand() % 1000 < 5) {  // 0.5% chance per step
        // Reset to average quality
        float avgQuality = 0.0f;
        for (float q : actionQuality_) avgQuality += q;
        avgQuality /= actionQuality_.size();
        
        for (size_t i = 0; i < actionQuality_.size(); ++i) {
            actionQuality_[i] = avgQuality + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f;
        }
    }
}

void NeuralPlanner::implementValueBasedPlanning() {
    if (!brain_) return;
    
    // Value-based planning using learned action values
    float valueThreshold = 0.3f;  // Minimum value to consider
    
    ActionType bestAction = ActionType::Wait;
    float bestValue = -1000.0f;
    
    for (size_t i = 0; i < actionQuality_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Evaluate action value using neural network (simplified)
        float actionValue = evaluateAction(action, currentGoal_);
        
        // Combine with learned quality
        float combinedValue = actionQuality_[i] * 0.6f + actionValue * 0.4f;
        
        if (combinedValue > bestValue && combinedValue > valueThreshold) {
            bestValue = combinedValue;
            bestAction = action;
        }
    }
    
    // Update current goal based on value-based planning
    if (bestValue > 0.0f) {
        currentGoal_ = std::vector<float>(10, bestValue); // Simplified goal setting
    }
}

void NeuralPlanner::implementPolicySearchPlanning() {
    if (!brain_) return;
    
    // Policy search: adjust policy parameters based on observed outcomes
    static float policyLearningRate = 0.05f;
    
    // Update policy parameters based on prediction error
    PredictionError* predictionError = brain_->getPredictionErrorSignal();
    if (predictionError) {
        float error = predictionError->getError();
        
        // Adjust policy based on prediction error
        float policyAdjustment = error * policyLearningRate;
        
        for (size_t i = 0; i < actionQuality_.size(); ++i) {
            // Policy gradient update
            actionQuality_[i] += policyAdjustment * (rand() % 100 / 100.0f - 0.5f);
            actionQuality_[i] = std::clamp(actionQuality_[i], -1.0f, 1.0f);
        }
    }
}

void NeuralPlanner::implementPredictionBasedPlanning() {
    if (!brain_) return;
    
    PredictionSystem* predictionSystem = brain_->getPredictionSystem();
    if (!predictionSystem) return;
    
    // Use prediction system for forward model planning
    float predictionError = predictionSystem->getPredictionError();
    
    // Planning based on prediction accuracy
    float planningModulation = 1.0f - predictionError;  // Better predictions enable more ambitious planning
    
    // Adjust planning depth based on prediction confidence
    planningDepth_ = static_cast<size_t>(3 + planningModulation * 3); // 3 to 6 steps
    
    // Use prediction errors to guide action selection
    if (predictionError > 0.5f) {
        // High prediction error: prefer exploratory actions
        // This would bias action selection toward less-used actions
        NLM_LOG_INFO("High prediction error guiding exploratory planning (error: " + 
                    std::to_string(predictionError) + ")");
    }
}

void NeuralPlanner::integrateWithWorkingMemory() {
    if (!brain_) return;
    
    NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Get current working memory content
    std::vector<float> wmContent = workingMemory->retrieve();
    
    if (!wmContent.empty()) {
        // Use working memory to guide planning
        float wmActivity = workingMemory->getMemoryActivity();
        
        // Adjust planning based on working memory activity
        if (wmActivity > 0.5f) {
            // High working memory activity: focus on complex plans
            planningDepth_ = std::min(planningDepth_ + 2, size_t(8));
        } else {
            // Low working memory activity: use simpler plans
            planningDepth_ = std::max(planningDepth_ - 1, size_t(2));
        }
        
        // Incorporate working memory patterns into planning
        integratePlanningWithMemoryContent(wmContent);
    }
}

void NeuralPlanner::integratePlanningWithMemoryContent(const std::vector<float>& memoryContent) {
    // Use working memory content to influence planning decisions
    if (memoryContent.empty()) return;
    
    // Compute similarity between memory content and planning goals
    float memorySimilarity = 0.0f;
    if (!currentGoal_.empty() && memoryContent.size() == currentGoal_.size()) {
        for (size_t i = 0; i < memoryContent.size(); ++i) {
            memorySimilarity += std::abs(memoryContent[i] - currentGoal_[i]);
        }
        memorySimilarity /= memoryContent.size();
    }
    
    // Adjust planning based on memory content
    if (memorySimilarity > 0.3f) {
        // Strong memory content influence: plan that leverages memory
        planningDepth_ = std::min(planningDepth_ + 1, size_t(8));
    }
}

void NeuralPlanner::implementPlanningActionSelection() {
    if (!brain_) return;
    
    // Implement action selection based on planning results
    ActionType selectedAction = ActionType::Wait;
    float bestScore = -1000.0f;
    
    // Score each potential action based on planning
    for (size_t i = 0; i < actionQuality_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Get planning score for this action
        float planningScore = evaluateAction(action, currentGoal_);
        
        // Combine with action quality
        float combinedScore = planningScore * 0.7f + actionQuality_[i] * 0.3f;
        
        if (combinedScore > bestScore) {
            bestScore = combinedScore;
            selectedAction = action;
        }
    }
    
    // Apply attention modulation to action selection
    AttentionalSelection* attention = brain_->getAttention();
    if (attention && bestScore > 0.0f) {
        float attentionBonus = attention->getExcitationFor(selectedAction);
        if (attentionBonus > 0.0f) {
            bestScore += attentionBonus;
        }
    }
    
    // Store selected action for next step
    lastSelectedAction_ = selectedAction;
    lastPlanningScore_ = bestScore;
}

ActionType NeuralPlanner::heuristicPlanning(const std::vector<float>& currentState,
                                           float targetReward) {
    // Simple heuristic planning for fallback
    ActionType bestAction = ActionType::Wait;
    float bestScore = -1000.0f;
    
    // Evaluate each action using heuristics
    for (size_t i = 0; i < actionQuality_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Heuristic evaluation
        float heuristicScore = evaluateAction(action, currentState);
        
        // Combine with learned quality
        float combinedScore = heuristicScore * 0.6f + actionQuality_[i] * 0.4f;
        
        if (combinedScore > bestScore) {
            bestScore = combinedScore;
            bestAction = action;
        }
    }
    
    return bestAction;
}

Impl::MCTSNode* NeuralPlanner::selectNode(Impl::MCTSNode* node) {
    // MCTS node selection ( UCB1 formula )
    while (!node->children.empty()) {
        // Find child with highest UCB1 score
        Impl::MCTSNode* bestChild = nullptr;
        float bestScore = -std::numeric_limits<float>::max();
        
        for (Impl::MCTSNode* child : node->children) {
            if (child->visits == 0) {
                bestChild = child;
                break; // Always explore unvisited nodes first
            }
            
            // UCB1 formula: (reward / visits) + C * sqrt(log(parent_visits) / child_visits)
            float ucbScore = (child->value / child->visits) +
                           pImpl->explorationConstant * std::sqrt(
                               std::log(static_cast<float>(node->visits)) / child->visits);
            
            if (ucbScore > bestScore) {
                bestScore = ucbScore;
                bestChild = child;
            }
        }
        
        if (bestChild) {
            node = bestChild;
        } else {
            break; // No children to explore
        }
    }
    
    return node;
}

bool NeuralPlanner::isTerminalNode(Impl::MCTSNode* node) const {
    return node->visits == 0 || node->depth >= pImpl->mctsMaxDepth;
}

void NeuralPlanner::expandNode(Impl::MCTSNode* node) {
    if (!node || node->children.size() >= 6) return; // Limit branching factor
    
    // Generate possible actions from current state
    std::vector<ActionType> actions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // Sample a subset of actions (to limit branching)
    std::vector<ActionType> sampledActions;
    size_t numSamples = std::min(size_t(3), actions.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        size_t idx = rand() % actions.size();
        sampledActions.push_back(actions[idx]);
        actions.erase(actions.begin() + idx);
    }
    
    // Create child nodes for each sampled action
    for (ActionType action : sampledActions) {
        std::vector<ActionType> newSequence = node->actionSequence;
        newSequence.push_back(action);
        
        std::vector<float> newState = node->state;
        // Predict new state (simplified)
        auto predicted = predictNextState(action, node->state);
        if (!predicted.empty()) {
            newState = predicted;
        }
        
        Impl::MCTSNode* child = new Impl::MCTSNode(newSequence, newState, node);
        child->lastAction = action;
        node->children.push_back(child);
    }
}

float NeuralPlanner::simulateRollout(Impl::MCTSNode* node) {
    // Simple rollout: simulate random actions from this node
    std::vector<ActionType> currentSequence = node->actionSequence;
    std::vector<float> currentState = node->state;
    
    float totalReward = 0.0f;
    
    for (size_t i = 0; i < 5; ++i) {  // 5-step rollout
        // Select random action
        std::vector<ActionType> actions = {
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Interact, ActionType::Wait
        };
        
        ActionType action = actions[rand() % actions.size()];
        currentSequence.push_back(action);
        
        // Get action value
        float actionValue = evaluateAction(action, currentState);
        totalReward += actionValue;
        
        // Update state
        auto predictedNext = predictNextState(action, currentState);
        if (!predictedNext.empty()) {
            currentState = predictedNext;
        }
    }
    
    return totalReward;
}

void NeuralPlanner::backpropagateResult(Impl::MCTSNode* node, float reward) {
    // Backpropagate simulation result up the MCTS tree
    while (node) {
        node->visits++;
        node->value += reward;
        
        // Update confidence based on visit count
        node->confidence = std::min(1.0f, static_cast<float>(node->visits) / 10.0f);
        
        node = node->parent;
    }
}

ActionType NeuralPlanner::extractBestAction(Impl::MCTSNode* root) {
    if (!root || root->children.empty()) return ActionType::Wait;
    
    // Find child with most visits
    Impl::MCTSNode* bestChild = nullptr;
    size_t maxVisits = 0;
    
    for (Impl::MCTSNode* child : root->children) {
        if (child->visits > maxVisits) {
            maxVisits = child->visits;
            bestChild = child;
        }
    }
    
    if (bestChild && !bestChild->actionSequence.empty()) {
        return bestChild->actionSequence.back();
    }
    
    return ActionType::Wait;
}

void NeuralPlanner::cleanupMCTSNodes(Impl::MCTSNode* node) {
    if (!node) return;
    
    for (Impl::MCTSNode* child : node->children) {
        cleanupMCTSNodes(child);
        delete child;
    }
}

void NeuralPlanner::updatePlanningConfidence() {
    // Update planning confidence based on recent plan success
    if (recentPlanSuccess_.empty()) {
        planningConfidence_ = 0.5f; // Default confidence
        return;
    }
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) successCount++;
    }
    
    planningConfidence_ = static_cast<float>(successCount) / recentPlanSuccess_.size();
    
    // Decay confidence over time
    planningConfidence_ *= 0.9f;
}

void NeuralPlanner::setPlanningDepth(size_t depth) {
    planningDepth_ = std::max(depth, size_t(2)); // Minimum 2 steps
}

void NeuralPlanner::setPlanningDepth(size_t depth) {
    planningDepth_ = depth;
}

void NeuralPlanner::modulateCuriosityDrivenExploration(float modulation) {
    // Modulate exploration based on curiosity
    curiosityExplorationModulation_ += modulation;
    curiosityExplorationModulation_ = std::clamp(curiosityExplorationModulation_, -1.0f, 1.0f);
}

void NeuralPlanner::updateActionProbabilities(float bonus) {
    // Update action selection probabilities based on bonus
    if (bonus > 0.0f) {
        for (size_t i = 0; i < actionQuality_.size(); ++i) {
            actionQuality_[i] *= (1.0f + bonus * 0.1f);
        }
    }
}

void NeuralPlanner::logPlanningStats() const {
    NLM_LOG_INFO("NeuralPlanner Statistics:");
    NLM_LOG_INFO("  Planning depth: " + std::to_string(planningDepth_));
    NLM_LOG_INFO("  Planning confidence: " + std::to_string(planningConfidence_));
    NLM_LOG_INFO("  Plans generated: " + std::to_string(pImpl->totalPlansGenerated));
    NLM_LOG_INFO("  Successful plans: " + std::to_string(pImpl->successfulPlans));
    if (pImpl->totalPlansGenerated > 0) {
        float successRate = static_cast<float>(pImpl->successfulPlans) / pImpl->totalPlansGenerated;
        NLM_LOG_INFO("  Plan success rate: " + std::to_string(successRate));
    }
    NLM_LOG_INFO("  Average plan quality: " + std::to_string(pImpl->averagePlanQuality));
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
    
    return currentState; // Default: no predicted change
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
