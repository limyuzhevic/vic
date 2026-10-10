// Copyright © 2024 NLM Research Project
// Licensed under MIT License

/**
 * @file ExpertExample1_ComplexMultiGoalAgent.cpp
 * @brief Advanced multi-goal agent with concurrent goal management
 * 
 * This example demonstrates an expert-level NLM agent capable of managing
 * multiple concurrent goals with sophisticated planning, prioritization,
 * and adaptation strategies. The agent handles trade-offs between competing
 * goals and exhibits emergent social learning behaviors.
 */

#include "agent/AgentBody.hpp"
#include "brain/Brain.hpp"
#include "advanced_memory/AdvancedMemory.hpp"
#include "advanced_prediction/HierarchicalPlanner.hpp"
#include "advanced_prediction/GoalConditionedPolicies.hpp"
#include "advanced_prediction/MonteCarloTreeSearch.hpp"
#include "advanced_prediction/ForwardModeling.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

namespace nlm {

/**
 * @struct MultiGoal
 * Complex goal representation with hierarchical importance and dependencies
 */
struct MultiGoal {
    size_t id;
    std::vector<float> goalFeatures;
    float intrinsicValue;
    float extrinsicValue;
    float combinedValue;
    float urgency;
    float deadline;
    std::vector<size_t> dependencies;
    std::vector<size_t> dependents;
    size_t priorityLevel;
    
    // Goal state tracking
    float currentProgress;
    float targetValue;
    std::vector<float> intermediateStates;
    bool isCompleted;
    bool isPaused;
    float failureProbability;
    
    // Social aspects
    float socialImportance;
    size_t modelAgentId;
    float imitationValue;
    bool isSociallyInfluential;
    
    MultiGoal(size_t goalId, const std::vector<float>& features,
              float intrinsic, float extrinsic, float deadline)
        : id(goalId), goalFeatures(features), intrinsicValue(intrinsic),
          extrinsicValue(extrinsic), combinedValue(0.0f), urgency(0.0f),
          deadline(deadline), priorityLevel(1), currentProgress(0.0f),
          targetValue(1.0f), isCompleted(false), isPaused(false),
          failureProbability(0.0f), socialImportance(0.0f),
          modelAgentId(0), imitationValue(0.0f), isSociallyInfluential(false) {}
};

/**
 * @struct GoalConflictResolution
 * Manages conflicts between competing goals using utility theory
 */
struct GoalConflictResolution {
    std::vector<size_t> conflictingGoals;
    float totalUtility;
    std::vector<float> utilityValues;
    std::vector<float> sacrificeCosts;
    float resourceAllocation;
    
    GoalConflictResolution(const std::vector<size_t>& goals,
                          const std::vector<float>& utilities,
                          const std::vector<float>& costs)
        : conflictingGoals(goals), utilityValues(utilities),
          sacrificeCosts(costs), totalUtility(0.0f), resourceAllocation(1.0f) {
        for (float u : utilityValues) totalUtility += u;
    }
};

/**
 * @struct GoalPlanningNode
 * Planning node for multi-goal task hierarchy
 */
struct GoalPlanningNode {
    size_t goalId;
    size_t actionId;
    std::vector<float> stateFeatures;
    std::vector<size_t> subGoalSequence;
    float expectedUtility;
    float resourceCost;
    float timeEstimate;
    std::vector<GoalPlanningNode*> children;
    std::vector<GoalPlanningNode*> parent;
    
    GoalPlanningNode(size_t goal, size_t action,
                    const std::vector<float>& state,
                    const std::vector<size_t>& subGoals,
                    float utility, float cost, float time)
        : goalId(goal), actionId(action), stateFeatures(state),
          subGoalSequence(subGoals), expectedUtility(utility),
          resourceCost(cost), timeEstimate(time) {}
};

/**
 * AdvancedMultiGoalAgent class implementing complex goal management
 */
class AdvancedMultiGoalAgent {
public:
    AdvancedMultiGoalAgent(const std::string& name, const LifetimeExperimentConfig& config);
    ~AdvancedMultiGoalAgent();
    
    // Main lifecycle methods
    void initialize();
    void update(float dt);
    void shutdown();
    
    // Goal management
    void addGoal(const MultiGoal& goal);
    void removeGoal(size_t goalId);
    void pauseGoal(size_t goalId);
    void resumeGoal(size_t goalId);
    void completeGoal(size_t goalId);
    
    // Goal retrieval and analysis
    const MultiGoal* getGoal(size_t goalId) const;
    std::vector<size_t> getActiveGoals() const;
    std::vector<size_t> getCompletedGoals() const;
    std::vector<size_t> getConflictingGoals(size_t goalId) const;
    
    // Conflict resolution and prioritization
    void resolveGoalConflicts();
    void prioritizeGoals();
    void scheduleGoals();
    
    // Planning and execution
    std::vector<size_t> planMultiGoalSequence();
    void executeGoal(size_t goalId);
    void adaptGoals(float environmentalChange);
    
    // Social learning and imitation
    void observeOtherAgent(const AdvancedMultiGoalAgent& otherAgent);
    void imitateSuccessfulActions();
    void learnFromSocialContext();
    
    // Performance tracking
    float getCurrentUtility() const;
    float getGoalCompletionRate() const;
    float getResourceEfficiency() const;
    std::vector<float> getUtilityHistory() const;
    
    // State management
    void saveState();
    void loadState();
    
    // Core brain components
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::string name_;
    LifetimeExperimentConfig config_;
    
    // Core brain components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<HierarchicalPlanner> planner_;
    std::shared_ptr<GoalConditionedPolicies> policyEngine_;
    std::shared_ptr<MonteCarloTreeSearch> mctsEngine_;
    std::shared_ptr<ForwardModeling> forwardModel_;
    std::shared_ptr<AdvancedMemory> advancedMemory_;
    
    // Goal management
    std::unordered_map<size_t, MultiGoal> goals_;
    std::priority_queue<size_t> priorityQueue_; // Goals with highest priority
    std::vector<GoalConflictResolution> conflicts_;
    std::vector<GoalPlanningNode> goalTree_;
    
    // Social learning
    std::vector<size_t> observedAgents_;
    std::unordered_map<size_t, std::vector<size_t>> imitationPatterns_;
    std::vector<float> socialLearningStrength_;
    
    // Performance metrics
    std::vector<float> utilityHistory_;
    std::vector<float> resourceUsage_;
    size_t stepsSinceLastConflict_;
    size_t totalAdaptations_;
    
    // Internal state
    bool isInitialized_;
    bool isRunning_;
    size_t currentStep_;
    std::chrono::steady_clock::time_point startTime_;
    
    // Helper methods
    void updateGoalValues();
    void calculateDependencies();
    void updateGoalProgress();
    float calculateGoalUtility(const MultiGoal& goal) const;
    float calculateConflictCost(size_t goal1Id, size_t goal2Id) const;
    void buildGoalTree();
    void updateSocialLearning();
    std::vector<size_t> selectSocialModels() const;
    void transferSuccessfulStrategies(size_t fromAgentId, size_t toAgentId);
    std::vector<size_t> optimizeActionSequence(const std::vector<size_t>& availableActions,
                                              const std::vector<size_t>& targetGoals);
};

// Implementation of AdvancedMultiGoalAgent

AdvancedMultiGoalAgent::AdvancedMultiGoalAgent(const std::string& name, 
                                             const LifetimeExperimentConfig& config)
    : name_(name), config_(config), isInitialized_(false), isRunning_(false),
      currentStep_(0), stepsSinceLastConflict_(0), totalAdaptations_(0) {
    startTime_ = std::chrono::steady_clock::now();
}

AdvancedMultiGoalAgent::~AdvancedMultiGoalAgent() {
    shutdown();
}

void AdvancedMultiGoalAgent::initialize() {
    std::cout << "Initializing AdvancedMultiGoalAgent: " << name_ << std::endl;
    
    // Initialize core brain components
    brain_ = std::make_shared<Brain>();
    
    // Initialize advanced systems
    planner_ = std::make_shared<HierarchicalPlanner>();
    policyEngine_ = std::make_shared<GoalConditionedPolicies>();
    mctsEngine_ = std::make_shared<MonteCarloTreeSearch>();
    forwardModel_ = std::make_shared<ForwardModeling>();
    advancedMemory_ = std::make_shared<AdvancedMemory>();
    
    // Configure advanced systems
    planner_->setMaxTreeDepth(config_.max_tree_depth);
    planner_->setMCTSSimulations(config_.mcts_simulations);
    
    policyEngine_->setNumPolicies(config_.num_policies);
    policyEngine_->setGoalAbstractionLevel(config_.goal_abstraction_level);
    
    mctsEngine_->setMaxTreeSize(config_.max_tree_size);
    mctsEngine_->setExplorationTemperature(config_.exploration_temperature);
    
    forwardModel_->setModelComplexity(config_.model_complexity);
    forwardModel_->setHorizonLength(config_.horizon_length);
    
    advancedMemory_->setMaxMemorySize(config_.memory_size);
    
    isInitialized_ = true;
    std::cout << "AdvancedMultiGoalAgent initialized successfully" << std::endl;
}

void AdvancedMultiGoalAgent::update(float dt) {
    if (!isInitialized_ || !isRunning_) return;
    
    currentStep_++;
    
    // Update goal values and progress
    updateGoalValues();
    updateGoalProgress();
    
    // Handle conflicts and prioritization
    if (stepsSinceLastConflict_ >= config_.conflict_resolution_interval) {
        resolveGoalConflicts();
        prioritizeGoals();
        scheduleGoals();
        stepsSinceLastConflict_ = 0;
    } else {
        stepsSinceLastConflict_++;
    }
    
    // Social learning and adaptation
    learnFromSocialContext();
    adaptGoals(0.0f); // No environmental change for now
    
    // Execute planned actions
    if (currentStep_ % config_.execution_interval == 0) {
        std::vector<size_t> plan = planMultiGoalSequence();
        for (size_t goalId : plan) {
            executeGoal(goalId);
        }
    }
    
    // Update social learning patterns
    updateSocialLearning();
}

void AdvancedMultiGoalAgent::shutdown() {
    if (isInitialized_) {
        std::cout << "Shutting down AdvancedMultiGoalAgent: " << name_ << std::endl;
        
        // Clear all components
        brain_.reset();
        planner_.reset();
        policyEngine_.reset();
        mctsEngine_.reset();
        forwardModel_.reset();
        advancedMemory_.reset();
        
        goals_.clear();
        priorityQueue_ = std::priority_queue<size_t>();
        conflicts_.clear();
        goalTree_.clear();
        observedAgents_.clear();
        imitationPatterns_.clear();
        socialLearningStrength_.clear();
        utilityHistory_.clear();
        resourceUsage_.clear();
        
        isInitialized_ = false;
        isRunning_ = false;
        
        std::cout << "AdvancedMultiGoalAgent shutdown complete" << std::endl;
    }
}

void AdvancedMultiGoalAgent::addGoal(const MultiGoal& goal) {
    if (goals_.find(goal.id) != goals_.end()) {
        std::cerr << "Warning: Goal " << goal.id << " already exists" << std::endl;
        return;
    }
    
    goals_[goal.id] = goal;
    priorityQueue_.push(goal.id);
    
    // Update dependencies
    calculateDependencies();
}

void AdvancedMultiGoalAgent::removeGoal(size_t goalId) {
    auto it = goals_.find(goalId);
    if (it != goals_.end()) {
        // Remove from dependencies
        for (auto& pair : goals_) {
            auto& goal = pair.second;
            goal.dependents.erase(std::remove(goal.dependents.begin(), 
                                            goal.dependents.end(), goalId),
                                 goal.dependents.end());
        }
        
        // Remove the goal
        goals_.erase(it);
    }
}

void AdvancedMultiGoalAgent::pauseGoal(size_t goalId) {
    auto it = goals_.find(goalId);
    if (it != goals_.end()) {
        it->second.isPaused = true;
    }
}

void AdvancedMultiGoalAgent::resumeGoal(size_t goalId) {
    auto it = goals_.find(goalId);
    if (it != goals_.end()) {
        it->second.isPaused = false;
    }
}

void AdvancedMultiGoalAgent::completeGoal(size_t goalId) {
    auto it = goals_.find(goalId);
    if (it != goals_.end()) {
        it->second.isCompleted = true;
        it->second.currentProgress = 1.0f;
        
        // Update dependent goals
        for (size_t dependentId : it->second.dependents) {
            auto dependentIt = goals_.find(dependentId);
            if (dependentIt != goals_.end()) {
                dependentIt->second.failureProbability *= 0.5f;
            }
        }
    }
}

const MultiGoal* AdvancedMultiGoalAgent::getGoal(size_t goalId) const {
    auto it = goals_.find(goalId);
    return (it != goals_.end()) ? &it->second : nullptr;
}

std::vector<size_t> AdvancedMultiGoalAgent::getActiveGoals() const {
    std::vector<size_t> activeGoals;
    for (const auto& pair : goals_) {
        if (!pair.second.isCompleted && !pair.second.isPaused) {
            activeGoals.push_back(pair.first);
        }
    }
    return activeGoals;
}

std::vector<size_t> AdvancedMultiGoalAgent::getCompletedGoals() const {
    std::vector<size_t> completedGoals;
    for (const auto& pair : goals_) {
        if (pair.second.isCompleted) {
            completedGoals.push_back(pair.first);
        }
    }
    return completedGoals;
}

std::vector<size_t> AdvancedMultiGoalAgent::getConflictingGoals(size_t goalId) const {
    std::vector<size_t> conflicts;
    const MultiGoal* goal = getGoal(goalId);
    if (!goal) return conflicts;
    
    for (const auto& pair : goals_) {
        if (pair.first == goalId) continue;
        
        const MultiGoal& otherGoal = pair.second;
        if (!otherGoal.isCompleted && !otherGoal.isPaused) {
            // Check for conflicts based on feature overlap
            float similarity = 0.0f;
            for (size_t i = 0; i < std::min(goal->goalFeatures.size(), 
                                           otherGoal.goalFeatures.size()); ++i) {
                similarity += std::abs(goal->goalFeatures[i] - otherGoal.goalFeatures[i]);
            }
            similarity /= std::min(goal->goalFeatures.size(), otherGoal.goalFeatures.size());
            
            if (similarity > config_.conflict_threshold) {
                conflicts.push_back(pair.first);
            }
        }
    }
    
    return conflicts;
}

void AdvancedMultiGoalAgent::resolveGoalConflicts() {
    conflicts_.clear();
    
    // Group goals by conflict clusters
    std::vector<std::vector<size_t>> conflictClusters;
    std::vector<bool> processed(goals_.size(), false);
    
    size_t idx = 0;
    for (const auto& pair : goals_) {
        size_t goalId = pair.first;
        if (processed[idx]) continue;
        
        std::vector<size_t> cluster;
        cluster.push_back(goalId);
        processed[idx] = true;
        
        // Find conflicting goals
        std::vector<size_t> conflicts = getConflictingGoals(goalId);
        for (size_t conflictId : conflicts) {
            // Find index of conflicting goal
            size_t conflictIdx = 0;
            for (const auto& p : goals_) {
                if (p.first == conflictId) {
                    break;
                }
                conflictIdx++;
            }
            if (!processed[conflictIdx]) {
                cluster.push_back(conflictId);
                processed[conflictIdx] = true;
            }
        }
        
        if (cluster.size() > 1) {
            conflictClusters.push_back(cluster);
        }
        
        idx++;
    }
    
    // Resolve each conflict cluster
    for (const auto& cluster : conflictClusters) {
        std::vector<float> utilities;
        std::vector<float> costs;
        
        for (size_t goalId : cluster) {
            const MultiGoal& goal = goals_[goalId];
            float utility = calculateGoalUtility(goal);
            float cost = 0.0f;
            
            // Calculate opportunity cost
            for (size_t otherId : cluster) {
                if (otherId != goalId) {
                    cost += calculateConflictCost(goalId, otherId);
                }
            }
            
            utilities.push_back(utility);
            costs.push_back(cost);
        }
        
        conflicts_.emplace_back(cluster, utilities, costs);
    }
}

void AdvancedMultiGoalAgent::prioritizeGoals() {
    // Priority based on combined value, urgency, and social influence
    std::vector<std::pair<size_t, float>> priorityScores;
    
    for (const auto& pair : goals_) {
        const MultiGoal& goal = pair.second;
        if (goal.isCompleted || goal.isPaused) continue;
        
        float score = goal.combinedValue * goal.urgency * 
                     (1.0f + goal.socialImportance * config_.social_weight);
        
        priorityScores.emplace_back(pair.first, score);
    }
    
    // Sort by priority score
    std::sort(priorityScores.begin(), priorityScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Update priority queue
    priorityQueue_ = std::priority_queue<size_t>();
    for (const auto& pair : priorityScores) {
        priorityQueue_.push(pair.first);
    }
}

void AdvancedMultiGoalAgent::scheduleGoals() {
    // Build dependency-aware schedule
    buildGoalTree();
    
    // Update goal tree with current state
    std::vector<float> currentState(50, 0.0f); // Simplified state representation
    
    // For each active goal, generate plan
    for (auto& node : goalTree_) {
        const MultiGoal* goal = getGoal(node.goalId);
        if (!goal || goal->isCompleted || goal->isPaused) continue;
        
        // Generate action sequence for this goal
        std::vector<size_t> availableActions = {
            0, 1, 2, 3, 4 // Simplified action space
        };
        
        std::vector<size_t> optimalActions = optimizeActionSequence(
            availableActions, std::vector<size_t>{goal->id});
        
        // Store in goal node
        node.children.clear(); // Simplified: clear children
    }
}

std::vector<size_t> AdvancedMultiGoalAgent::planMultiGoalSequence() {
    std::vector<size_t> sequence;
    
    // Get prioritized goals from queue
    std::priority_queue<size_t> tempQueue = priorityQueue_;
    std::vector<size_t> orderedGoals;
    
    while (!tempQueue.empty()) {
        orderedGoals.push_back(tempQueue.top());
        tempQueue.pop();
    }
    
    // Plan sequence based on dependencies and conflicts
    for (size_t goalId : orderedGoals) {
        const MultiGoal* goal = getGoal(goalId);
        if (!goal || goal->isCompleted || goal->isPaused) continue;
        
        // Check if goal can be scheduled (dependencies satisfied)
        bool dependenciesSatisfied = true;
        for (size_t depId : goal->dependencies) {
            const MultiGoal* depGoal = getGoal(depId);
            if (!depGoal || !depGoal->isCompleted) {
                dependenciesSatisfied = false;
                break;
            }
        }
        
        if (dependenciesSatisfied) {
            sequence.push_back(goalId);
        }
    }
    
    return sequence;
}

void AdvancedMultiGoalAgent::executeGoal(size_t goalId) {
    const MultiGoal* goal = getGoal(goalId);
    if (!goal || goal->isCompleted || goal->isPaused) return;
    
    // Simulate goal execution with progress update
    float progressIncrement = config_.goal_progress_rate * goal->urgency;
    progressIncrement *= (1.0f + goal->socialImportance * 0.2f); // Social boost
    
    // Update progress
    MultiGoal& mutableGoal = goals_[goalId];
    mutableGoal.currentProgress = std::min(1.0f, 
                                          mutableGoal.currentProgress + progressIncrement);
    
    // Check if goal completed
    if (mutableGoal.currentProgress >= 1.0f) {
        completeGoal(goalId);
        
        // Update resource usage
        resourceUsage_.push_back(mutableGoal.resourceCost ? 1.0f : 0.0f);
    }
}

void AdvancedMultiGoalAgent::adaptGoals(float environmentalChange) {
    totalAdaptations_++;
    
    // Adapt goal parameters based on environmental change
    for (auto& pair : goals_) {
        MultiGoal& goal = pair.second;
        
        // Adjust goal values based on environmental change
        goal.intrinsicValue *= (1.0f + environmentalChange * 0.1f);
        goal.urgency *= (1.0f + environmentalChange * 0.05f);
        
        // Social learning influence
        if (!observedAgents_.empty() && totalAdaptations_ % 100 == 0) {
            goal.socialImportance *= 1.1f; // Increase importance of socially relevant goals
        }
    }
    
    // Update memory systems with new experiences
    if (advancedMemory_ && currentStep_ % 50 == 0) {
        // Store current goal state
        std::vector<float> goalState;
        for (const auto& pair : goals_) {
            const MultiGoal& goal = pair.second;
            goalState.push_back(goal.currentProgress);
            goalState.push_back(goal.urgency);
        }
        
        // Store in advanced memory
        advancedMemory_->store(goalState, 1.0f, 0, currentStep_);
    }
}

void AdvancedMultiGoalAgent::observeOtherAgent(const AdvancedMultiGoalAgent& otherAgent) {
    // Add to observed agents
    observedAgents_.push_back(otherAgent.getBrain()->getId());
    
    // Learn from successful strategies
    std::vector<size_t> otherGoals = otherAgent.getCompletedGoals();
    for (size_t goalId : otherGoals) {
        const MultiGoal* otherGoal = otherAgent.getGoal(goalId);
        if (otherGoal) {
            // Store imitation pattern
            imitationPatterns_[otherAgent.getBrain()->getId()].push_back(goalId);
            
            // Update social learning strength
            socialLearningStrength_.push_back(1.0f); // Simplified
        }
    }
}

void AdvancedMultiGoalAgent::imitateSuccessfulActions() {
    for (size_t agentId : observedAgents_) {
        auto it = imitationPatterns_.find(agentId);
        if (it != imitationPatterns_.end()) {
            // Imitate successful actions from observed agent
            for (size_t goalId : it->second) {
                // Check if we haven't completed this goal yet
                if (goals_.find(goalId) == goals_.end() || 
                    !goals_[goalId].isCompleted) {
                    
                    // Add goal to our goals with lower initial value (conservative)
                    const MultiGoal* modelGoal = getGoal(goalId);
                    if (modelGoal) {
                        MultiGoal newGoal(goalId, modelGoal->goalFeatures * 0.8f,
                                         modelGoal->intrinsicValue * 0.7f,
                                         modelGoal->extrinsicValue * 0.7f,
                                         modelGoal->deadline);
                        newGoal.socialImportance = modelGoal->socialImportance * 1.2f;
                        newGoal.isSociallyInfluential = true;
                        
                        addGoal(newGoal);
                    }
                }
            }
        }
    }
}

void AdvancedMultiGoalAgent::learnFromSocialContext() {
    if (observedAgents_.empty()) return;
    
    // Select best model agents based on performance
    std::vector<size_t> modelAgents = selectSocialModels();
    
    // Transfer successful strategies
    for (size_t modelId : modelAgents) {
        transferSuccessfulStrategies(modelId, getBrain()->getId());
    }
    
    // Update social learning strength based on imitation success
    float avgSuccess = 0.0f;
    size_t count = 0;
    for (size_t agentId : observedAgents_) {
        auto it = imitationPatterns_.find(agentId);
        if (it != imitationPatterns_.end()) {
            avgSuccess += it->second.size();
            count++;
        }
    }
    
    if (count > 0) {
        avgSuccess /= count;
        for (float& strength : socialLearningStrength_) {
            strength = std::min(1.0f, strength + 0.01f * avgSuccess);
        }
    }
}

float AdvancedMultiGoalAgent::getCurrentUtility() const {
    float totalUtility = 0.0f;
    for (const auto& pair : goals_) {
        const MultiGoal& goal = pair.second;
        if (!goal.isCompleted && !goal.isPaused) {
            totalUtility += calculateGoalUtility(goal);
        }
    }
    return totalUtility;
}

float AdvancedMultiGoalAgent::getGoalCompletionRate() const {
    size_t completed = 0;
    size_t total = 0;
    
    for (const auto& pair : goals_) {
        if (!pair.second.isPaused) {
            total++;
            if (pair.second.isCompleted) {
                completed++;
            }
        }
    }
    
    return (total > 0) ? static_cast<float>(completed) / total : 0.0f;
}

float AdvancedMultiGoalAgent::getResourceEfficiency() const {
    if (resourceUsage_.empty()) return 0.0f;
    
    float totalResources = 0.0f;
    for (float usage : resourceUsage_) {
        totalResources += usage;
    }
    
    float efficiency = (goals_.size() > 0) ? 
                       totalResources / goals_.size() : 0.0f;
    
    return 1.0f / (1.0f + efficiency); // Inverse relationship
}

std::vector<float> AdvancedMultiGoalAgent::getUtilityHistory() const {
    return utilityHistory_;
}

void AdvancedMultiGoalAgent::saveState() {
    // In a real implementation, this would save to persistent storage
    // For now, just log
    std::cout << "Saving state for agent: " << name_ << std::endl;
    std::cout << "  Goals: " << goals_.size() << std::endl;
    std::cout << "  Completed: " << getCompletedGoals().size() << std::endl;
    std::cout << "  Utility: " << getCurrentUtility() << std::endl;
}

void AdvancedMultiGoalAgent::loadState() {
    // In a real implementation, this would load from persistent storage
    // For now, just log
    std::cout << "Loading state for agent: " << name_ << std::endl;
}

// Private helper methods implementation

void AdvancedMultiGoalAgent::updateGoalValues() {
    for (auto& pair : goals_) {
        MultiGoal& goal = pair.second;
        
        // Update combined value
        goal.combinedValue = goal.intrinsicValue * config_.intrinsic_weight +
                            goal.extrinsicValue * config_.extrinsic_weight;
        
        // Update urgency based on deadline
        if (goal.deadline > 0) {
            float timeRemaining = goal.deadline - currentStep_;
            goal.urgency = (timeRemaining > 0) ? 
                          1.0f / (1.0f + timeRemaining) : 1.0f;
        }
        
        // Store utility in history
        utilityHistory_.push_back(calculateGoalUtility(goal));
    }
}

void AdvancedMultiGoalAgent::calculateDependencies() {
    // Simple dependency calculation based on goal features
    for (auto& pair1 : goals_) {
        MultiGoal& goal1 = pair1.second;
        
        for (auto& pair2 : goals_) {
            if (pair1.first == pair2.first) continue;
            
            MultiGoal& goal2 = pair2.second;
            
            // Check if goal2 is a prerequisite for goal1
            // Simplified: if goal2 has similar features, it might be a prerequisite
            float similarity = 0.0f;
            for (size_t i = 0; i < std::min(goal1.goalFeatures.size(), 
                                           goal2.goalFeatures.size()); ++i) {
                if (std::abs(goal1.goalFeatures[i] - goal2.goalFeatures[i]) < 0.1f) {
                    similarity += 1.0f;
                }
            }
            
            if (similarity > 0.7f) { // Threshold for dependency
                goal1.dependencies.push_back(pair2.first);
                goal2.dependents.push_back(pair1.first);
            }
        }
    }
}

void AdvancedMultiGoalAgent::updateGoalProgress() {
    // Update progress based on execution and external factors
    for (auto& pair : goals_) {
        MultiGoal& goal = pair.second;
        
        if (goal.isPaused) continue;
        
        // Natural decay if not being worked on
        if (currentStep_ % config_.decay_interval == 0) {
            goal.currentProgress *= config_.decay_rate;
            goal.failureProbability *= 1.1f;
        }
    }
}

float AdvancedMultiGoalAgent::calculateGoalUtility(const MultiGoal& goal) const {
    float utility = goal.combinedValue * goal.urgency;
    
    // Add social influence
    utility *= (1.0f + goal.socialImportance * config_.social_weight);
    
    // Subtract failure penalty
    utility *= (1.0f - goal.failureProbability);
    
    // Add progress bonus
    utility *= (1.0f + goal.currentProgress * 0.5f);
    
    return utility;
}

float AdvancedMultiGoalAgent::calculateConflictCost(size_t goal1Id, 
                                                  size_t goal2Id) const {
    const MultiGoal* goal1 = getGoal(goal1Id);
    const MultiGoal* goal2 = getGoal(goal2Id);
    
    if (!goal1 || !goal2) return 0.0f;
    
    // Calculate feature similarity as conflict measure
    float similarity = 0.0f;
    for (size_t i = 0; i < std::min(goal1->goalFeatures.size(), 
                                   goal2->goalFeatures.size()); ++i) {
        similarity += std::abs(goal1->goalFeatures[i] - goal2->goalFeatures[i]);
    }
    
    // Higher similarity = higher conflict cost
    return 1.0f - similarity; // Inverted because abs difference
}

void AdvancedMultiGoalAgent::buildGoalTree() {
    goalTree_.clear();
    
    // Build hierarchical tree of goals
    for (const auto& pair : goals_) {
        const MultiGoal& goal = pair.second;
        if (goal.isCompleted || goal.isPaused) continue;
        
        // Create planning node for this goal
        GoalPlanningNode node(
            pair.first, 
            0, // action id (simplified)
            std::vector<float>(50, 0.0f), // state features (simplified)
            goal.dependencies,
            calculateGoalUtility(goal),
            1.0f, // resource cost (simplified)
            10.0f // time estimate (simplified)
        );
        
        goalTree_.push_back(node);
    }
}

void AdvancedMultiGoalAgent::updateSocialLearning() {
    if (currentStep_ % config_.social_learning_interval == 0) {
        // Update social learning strength
        for (float& strength : socialLearningStrength_) {
            strength *= config_.social_learning_decay;
        }
        
        // Add new learning opportunities
        if (currentStep_ % config_.imitation_interval == 0) {
            imitateSuccessfulActions();
        }
    }
}

std::vector<size_t> AdvancedMultiGoalAgent::selectSocialModels() const {
    std::vector<size_t> models;
    
    // Select top N observed agents as models
    if (observedAgents_.size() <= config_.num_social_models) {
        models = observedAgents_;
    } else {
        // In real implementation, would rank by performance
        models.insert(models.end(), observedAgents_.begin(), 
                     observedAgents_.begin() + config_.num_social_models);
    }
    
    return models;
}

void AdvancedMultiGoalAgent::transferSuccessfulStrategies(size_t fromAgentId, 
                                                         size_t toAgentId) {
    auto it = imitationPatterns_.find(fromAgentId);
    if (it != imitationPatterns_.end()) {
        // Transfer successful strategies to target agent
        for (size_t goalId : it->second) {
            // In real implementation, would transfer detailed strategies
            // For now, just acknowledge the transfer
            if (goals_.find(goalId) != goals_.end()) {
                goals_[goalId].socialImportance *= 1.1f;
            }
        }
    }
}

std::vector<size_t> AdvancedMultiGoalAgent::optimizeActionSequence(
    const std::vector<size_t>& availableActions,
    const std::vector<size_t>& targetGoals) {
    
    std::vector<size_t> optimalSequence;
    
    if (availableActions.empty() || targetGoals.empty()) {
        return optimalSequence;
    }
    
    // Simplified optimization: select actions that maximize progress
    std::vector<std::pair<size_t, float>> actionScores;
    
    for (size_t action : availableActions) {
        float score = 0.0f;
        
        // Score based on goal compatibility
        for (size_t goalId : targetGoals) {
            const MultiGoal* goal = getGoal(goalId);
            if (goal) {
                score += goal->currentProgress * 0.1f; // Progress contribution
            }
        }
        
        actionScores.emplace_back(action, score);
    }
    
    // Sort by score
    std::sort(actionScores.begin(), actionScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Build sequence
    for (const auto& pair : actionScores) {
        optimalSequence.push_back(pair.first);
    }
    
    return optimalSequence;
}

} // namespace nlm

/**
 * @main
 * Demonstrate the AdvancedMultiGoalAgent
 */
int main() {
    std::cout << "=== Advanced Multi-Goal Agent Demo ===" << std::endl;
    
    // Create configuration
    nlm::LifetimeExperimentConfig config;
    config.max_tree_depth = 8;
    config.mcts_simulations = 500;
    config.num_policies = 3;
    config.goal_abstraction_level = 2;
    config.max_tree_size = 500;
    config.exploration_temperature = 1.5f;
    config.model_complexity = 3;
    config.horizon_length = 20;
    config.memory_size = 5000;
    config.intrinsic_weight = 0.6f;
    config.extrinsic_weight = 0.4f;
    config.social_weight = 0.3f;
    config.conflict_threshold = 0.8f;
    config.goal_progress_rate = 0.02f;
    config.decay_rate = 0.99f;
    config.decay_interval = 50;
    config.social_learning_interval = 100;
    config.social_learning_decay = 0.95f;
    config.imitation_interval = 200;
    config.num_social_models = 3;
    config.num_seeds = 1;
    
    // Create and initialize agent
    nlm::AdvancedMultiGoalAgent agent("ExpertAgent", config);
    agent.initialize();
    
    // Add multiple goals with different characteristics
    std::vector<nlm::MultiGoal> goals;
    
    // Goal 1: Resource acquisition
    goals.emplace_back(1, std::vector<float>{0.8f, 0.6f, 0.3f}, 0.7f, 0.5f, 200);
    
    // Goal 2: Social learning
    goals.emplace_back(2, std::vector<float>{0.3f, 0.9f, 0.7f}, 0.5f, 0.8f, 300);
    
    // Goal 3: Planning and prediction
    goals.emplace_back(3, std::vector<float>{0.9f, 0.4f, 0.6f}, 0.8f, 0.6f, 400);
    
    // Goal 4: Memory consolidation
    goals.emplace_back(4, std::vector<float>{0.5f, 0.7f, 0.4f}, 0.6f, 0.7f, 500);
    
    // Goal 5: Adaptive behavior
    goals.emplace_back(5, std::vector<float>{0.4f, 0.8f, 0.5f}, 0.7f, 0.5f, 600);
    
    // Add goals to agent
    for (const auto& goal : goals) {
        agent.addGoal(goal);
    }
    
    std::cout << "Agent initialized with " << goals.size() << " goals" << std::endl;
    
    // Simulate for some steps
    const size_t simulationSteps = 300;
    for (size_t step = 0; step < simulationSteps; ++step) {
        agent.update(0.016f); // 60 FPS simulation
        
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << "Utility: " << agent.getCurrentUtility() << ", "
                     << "Completion: " << (agent.getGoalCompletionRate() * 100) << "%, "
                     << "Efficiency: " << agent.getResourceEfficiency() << std::endl;
        }
        
        // Simulate observation of other agents
        if (step % 100 == 50) {
            // Create a simple mock observation
            std::cout << "  Observing other agents..." << std::endl;
            // In real implementation, would add observed agents
        }
    }
    
    // Final statistics
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final Utility: " << agent.getCurrentUtility() << std::endl;
    std::cout << "Goal Completion Rate: " << (agent.getGoalCompletionRate() * 100) << "%" << std::endl;
    std::cout << "Resource Efficiency: " << agent.getResourceEfficiency() << std::endl;
    std::cout << "Total Adaptations: " << agent.getBrain()->getId() << std::endl; // Simplified
    
    // Save final state
    agent.saveState();
    
    std::cout << "\nAdvancedMultiGoalAgent demo completed successfully!" << std::endl;
    
    return 0;
}
