#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <functional>
#include <set>

namespace nlm {

// Advanced prediction and planning systems

// Hierarchical planning with tree search
class HierarchicalPlanner {
public:
    struct PlanningNode {
        size_t id;
        SimulationStep timestamp;
        std::vector<float> state_features;
        std::vector<float> action_distribution;
        float value_estimate;
        float policy_score;
        float prior_prob;
        
        // Planning tree structure
        PlanningNode* parent;
        std::vector<PlanningNode*> children;
        std::vector<PlanningAction> planned_actions;
        
        // Tree search properties
        size_t depth;
        size_t visit_count;
        float action_value;
        float state_value;
        
        PlanningNode() : id(0), timestamp(0), value_estimate(0.0f), policy_score(0.0f),
                        prior_prob(1.0f), parent(nullptr), depth(0), visit_count(0),
                        action_value(0.0f), state_value(0.0f) {}
        
        PlanningNode(size_t node_id, const std::vector<float>& state,
                    PlanningNode* parent_node = nullptr)
            : id(node_id), timestamp(std::chrono::duration<double>().count()),
              state_features(state), value_estimate(0.0f), policy_score(0.0f),
              prior_prob(1.0f), parent(parent_node), depth(parent ? parent->depth + 1 : 0),
              visit_count(0), action_value(0.0f), state_value(0.0f) {}
        
        ~PlanningNode() {
            for (PlanningNode* child : children) {
                delete child;
            }
        }
        
        // MCTS selection criteria
        float getSelectionPriority() const {
            return action_value / (visit_count + 1);
        }
    };
    
    struct PlanningAction {
        size_t action_id;
        std::vector<float> action_features;
        std::vector<float> expected_outcomes;
        float probability;
        float value;
        float uncertainty;
        
        PlanningAction() : action_id(0), probability(0.0f), value(0.0f), uncertainty(0.0f) {}
        
        PlanningAction(size_t id, const std::vector<float>& features,
                      const std::vector<float>& outcomes, float prob)
            : action_id(id), action_features(features), expected_outcomes(outcomes),
              probability(prob), value(0.0f), uncertainty(0.0f) {}
    };
    
    HierarchicalPlanner() : max_tree_depth_(10), mcts_simulations_(1000),
                           exploration_constant_(1.414f), planning_horizon_(100),
                           use_value_network_(true), use_policy_network_(true) {}
    
    // Perform hierarchical planning from current state
    std::vector<PlanningAction> planActions(const std::vector<float>& current_state,
                                           const std::vector<PlanningAction>& available_actions);
    
    // Monte Carlo Tree Search (MCTS) for action selection
    PlanningNode* performMCTS(PlanningNode* root,
                             const std::vector<PlanningAction>& actions,
                             int simulations);
    
    // Expand node with possible actions
    void expandNode(PlanningNode* node, const std::vector<PlanningAction>& actions);
    
    // Simulate action outcomes
    float simulateAction(const PlanningAction& action, const std::vector<float>& current_state,
                        float dt);
    
    // Get planning statistics
    struct PlanningStats {
        size_t total_planning_steps;
        size_t average_tree_size;
        float average_action_value;
        float planning_efficiency;
        size_t successful_planning_attempts;
        
        PlanningStats() : total_planning_steps(0), average_tree_size(0),
                         average_action_value(0.0f), planning_efficiency(0.0f),
                         successful_planning_attempts(0) {}
    };
    
    PlanningStats getPlanningStats() const;
    
    // Set parameters
    void setMaxTreeDepth(size_t depth) { max_tree_depth_ = depth; }
    void setMCTSSimulations(size_t simulations) { mcts_simulations_ = simulations; }
    void setExplorationConstant(float constant) { exploration_constant_ = constant; }
    void setPlanningHorizon(SimulationStep horizon) { planning_horizon_ = horizon; }
    void setUseValueNetwork(bool use) { use_value_network_ = use; }
    void setUsePolicyNetwork(bool use) { use_policy_network_ = use; }
    
private:
    size_t max_tree_depth_;
    size_t mcts_simulations_;
    float exploration_constant_;
    SimulationStep planning_horizon_;
    bool use_value_network_;
    bool use_policy_network_;
    
    PlanningStats planning_stats_;
    std::unordered_map<size_t, PlanningNode*> planning_trees_;
};

// Monte Carlo tree search for action selection
class MonteCarloTreeSearch {
public:
    struct MCTSNode {
        size_t action_id;
        std::vector<float> state_features;
        std::vector<float> action_features;
        
        // MCTS statistics
        size_t visit_count;
        float total_value;
        float prior_probability;
        float action_policy;
        
        // Tree structure
        MCTSNode* parent;
        std::vector<MCTSNode*> children;
        
        MCTSNode() : action_id(0), visit_count(0), total_value(0.0f),
                     prior_probability(0.0f), action_policy(0.0f), parent(nullptr) {}
        
        MCTSNode(size_t act_id, const std::vector<float>& state,
                const std::vector<float>& action, MCTSNode* parent_node = nullptr)
            : action_id(act_id), state_features(state), action_features(action),
              visit_count(0), total_value(0.0f), prior_probability(1.0f),
              action_policy(0.0f), parent(parent_node) {}
        
        float getAverageValue() const {
            return visit_count > 0 ? total_value / visit_count : 0.0f;
        }
        
        float getUCBScore(float exploration_constant) const {
            if (visit_count == 0) return std::numeric_limits<float>::max();
            return getAverageValue() + exploration_constant * 
                   std::sqrt(std::log(parent ? parent->visit_count : 1) / visit_count);
        }
    };
    
    struct SearchResult {
        size_t selected_action;
        std::vector<float> action_distribution;
        float confidence;
        float estimated_value;
        float planning_time;
        
        SearchResult() : selected_action(0), confidence(0.0f), estimated_value(0.0f),
                        planning_time(0.0f) {}
    };
    
    MonteCarloTreeSearch() : max_tree_size_(1000), simulation_horizon_(50),
                            rollout_policy_("uct"), exploration_temperature_(1.0f),
                            use_heuristic_evaluation_(true), backup_all_paths_(false) {}
    
    // Perform MCTS search
    SearchResult search(const std::vector<float>& initial_state,
                       const std::vector<std::vector<float>>& available_actions);
    
    // Selection phase of MCTS
    MCTSNode* selection(MCTSNode* root);
    
    // Expansion phase of MCTS
    std::vector<MCTSNode*> expansion(MCTSNode* node,
                                     const std::vector<std::vector<float>>& available_actions);
    
    // Simulation phase (rollout) of MCTS
    float simulation(MCTSNode* node, const std::vector<std::vector<float>>& available_actions,
                    const std::string& policy);
    
    // Backpropagation phase of MCTS
    void backpropagation(MCTSNode* node, float reward);
    
    // Get search statistics
    struct MCStats {
        size_t total_simulations;
        size_t successful_searches;
        float average_tree_size;
        float average_search_time;
        float uct_usage_rate;
        
        MCStats() : total_simulations(0), successful_searches(0),
                   average_tree_size(0.0f), average_search_time(0.0f),
                   uct_usage_rate(0.0f) {}
    };
    
    MCStats getMCStats() const;
    
    // Set parameters
    void setMaxTreeSize(size_t max) { max_tree_size_ = max; }
    void setSimulationHorizon(size_t horizon) { simulation_horizon_ = horizon; }
    void setRolloutPolicy(const std::string& policy) { rollout_policy_ = policy; }
    void setExplorationTemperature(float temp) { exploration_temperature_ = temp; }
    void setUseHeuristicEvaluation(bool use) { use_heuristic_evaluation_ = use; }
    void setBackupAllPaths(bool backup) { backup_all_paths_ = backup; }
    
private:
    size_t max_tree_size_;
    size_t simulation_horizon_;
    std::string rollout_policy_;
    float exploration_temperature_;
    bool use_heuristic_evaluation_;
    bool backup_all_paths_;
    
    MCStats mc_stats_;
    std::vector<MCTSNode*> active_trees_;
    
    // Internal helper methods
    float evaluateStateHeuristic(const std::vector<float>& state) const;
    float evaluateActionHeuristic(const std::vector<float>& action,
                                 const std::vector<float>& state) const;
    std::vector<float> computeActionProbabilities(const std::vector<MCTSNode*>& children,
                                                  float temperature) const;
    void pruneTree(MCTSNode* root);
    void cleanupInactiveTrees();
};

// Forward modeling and inverse planning
class ForwardModeling {
public:
    struct ForwardModel {
        std::vector<std::vector<float>> transition_weights;  // State transition dynamics
        std::vector<float> noise_parameters;                // Process noise
        std::vector<float> model_uncertainty;               // Model confidence
        float prediction_horizon;                            // How far ahead to predict
        
        ForwardModel() : prediction_horizon(10.0f) {}
    };
    
    struct InversePlanningSolution {
        size_t best_action_id;
        std::vector<size_t> action_sequence;
        float solution_cost;
        float solution_confidence;
        float computational_cost;
        
        InversePlanningSolution() : best_action_id(0), solution_cost(0.0f),
                                   solution_confidence(0.0f), computational_cost(0.0f) {}
    };
    
    ForwardModeling() : learning_rate_(0.01f), model_complexity_(5),
                       horizon_length_(20), use_inverse_planning_(true),
                       use_approximate_inference_(true) {}
    
    // Learn forward model from experience
    void learnForwardModel(const std::vector<float>& current_state,
                          const std::vector<float>& next_state,
                          const std::vector<float>& action);
    
    // Predict future states
    std::vector<float> predictFutureState(const std::vector<float>& current_state,
                                         const std::vector<float>& action,
                                         float dt);
    
    // Generate multiple forward predictions
    std::vector<std::vector<float>> predictMultipleStates(
        const std::vector<float>& current_state,
        const std::vector<float>& action,
        size_t num_predictions);
    
    // Inverse planning to find optimal action sequence
    InversePlanningSolution inversePlan(const std::vector<float>& current_state,
                                       const std::vector<PlanningAction>& available_actions,
                                       size_t planning_horizon);
    
    // Get forward model statistics
    struct ForwardModelStats {
        size_t total_transitions_learned;
        float average_model_accuracy;
        float prediction_error;
        float model_complexity;
        float inference_efficiency;
        
        ForwardModelStats() : total_transitions_learned(0), average_model_accuracy(0.0f),
                             prediction_error(0.0f), model_complexity(0.0f), inference_efficiency(0.0f) {}
    };
    
    ForwardModelStats getForwardModelStats() const;
    
    // Set parameters
    void setLearningRate(float rate) { learning_rate_ = rate; }
    void setModelComplexity(size_t complexity) { model_complexity_ = complexity; }
    void setHorizonLength(size_t horizon) { horizon_length_ = horizon; }
    void setUseInversePlanning(bool use) { use_inverse_planning_ = use; }
    void setUseApproximateInference(bool use) { use_approximate_inference_ = use; }
    
private:
    ForwardModel forward_model_;
    float learning_rate_;
    size_t model_complexity_;
    size_t horizon_length_;
    bool use_inverse_planning_;
    bool use_approximate_inference_;
    
    // Internal helper methods
    std::vector<float> applyForwardDynamics(const std::vector<float>& state,
                                            const std::vector<float>& action);
    float computeTransitionProbability(const std::vector<float>& state_from,
                                       const std::vector<float>& state_to,
                                       const std::vector<float>& action) const;
    float computeInversePlanningCost(const std::vector<size_t>& action_sequence,
                                    const std::vector<float>& current_state) const;
    std::vector<float> simulateActionSequence(const std::vector<size_t>& action_sequence,
                                             const std::vector<float>& initial_state) const;
    float evaluatePlanQuality(const std::vector<size_t>& action_sequence,
                             const std::vector<float>& current_state) const;
};

// Goal-conditioned policies
class GoalConditionedPolicies {
public:
    struct Policy {
        std::vector<std::vector<float>> goal_conditions;    // What makes a goal
        std::vector<std::vector<float>> action_weights;    // How to act given goal
        std::vector<float> policy_parameters;             // Policy hyperparameters
        float intrinsic_motivation;                        // Drive to achieve goals
        float goal_achievement_reward;                     // Reward for goal achievement
        
        Policy() : intrinsic_motivation(1.0f), goal_achievement_reward(10.0f) {}
    };
    
    struct PolicyState {
        std::vector<float> current_goal;
        float goal_progress;                               // How close to goal
        float goal_urgency;                                // How important the goal is
        float policy_confidence;                           // Confidence in policy
        float intrinsic_drive;                             // Internal motivation
        
        PolicyState() : goal_progress(0.0f), goal_urgency(1.0f),
                       policy_confidence(0.5f), intrinsic_drive(1.0f) {}
    };
    
    GoalConditionedPolicies() : num_policies_(5), goal_learning_rate_(0.01f),
                               policy_adaptation_rate_(0.1f), goal_abstraction_level_(3),
                               use_meta_awareness_(true), policy_transfer_(true) {}
    
    // Execute goal-conditioned policy
    std::vector<float> executePolicy(const PolicyState& policy_state,
                                    const std::vector<PlanningAction>& available_actions);
    
    // Learn goal-conditioned policy
    void learnPolicy(const PolicyState& policy_state,
                    const std::vector<float>& outcome,
                    const std::vector<PlanningAction>& actions_taken);
    
    // Set goal based on environment context
    void setGoal(const std::vector<float>& goal,
                const std::vector<float>& context);
    
    // Get policy statistics
    struct PolicyStats {
        size_t total_policies_learned;
        float average_policy_success;
        float goal_achievement_rate;
        float policy_adaptation_effectiveness;
        float intrinsic_motivation_level;
        
        PolicyStats() : total_policies_learned(0), average_policy_success(0.0f),
                       goal_achievement_rate(0.0f), policy_adaptation_effectiveness(0.0f),
                       intrinsic_motivation_level(0.0f) {}
    };
    
    PolicyStats getPolicyStats() const;
    
    // Set parameters
    void setNumPolicies(size_t num) { num_policies_ = num; }
    void setGoalLearningRate(float rate) { goal_learning_rate_ = rate; }
    void setPolicyAdaptationRate(float rate) { policy_adaptation_rate_ = rate; }
    void setGoalAbstractionLevel(size_t level) { goal_abstraction_level_ = level; }
    void setMetaAwareness(bool enable) { use_meta_awareness_ = enable; }
    void setPolicyTransfer(bool enable) { policy_transfer_ = enable; }
    
private:
    std::vector<Policy> policies_;
    PolicyState current_policy_state_;
    size_t num_policies_;
    float goal_learning_rate_;
    float policy_adaptation_rate_;
    size_t goal_abstraction_level_;
    bool use_meta_awareness_;
    bool policy_transfer_;
    
    // Internal helper methods
    size_t selectBestPolicy(const std::vector<float>& goal,
                           const std::vector<float>& context) const;
    float computePolicyValue(const Policy& policy, const PolicyState& state,
                           const std::vector<float>& outcome) const;
    std::vector<float> adaptPolicyParameters(const Policy& old_policy,
                                            const std::vector<float>& experience) const;
    void transferPolicyKnowledge(const Policy& source_policy, Policy& target_policy) const;
};

// Reranking and preference learning
class PreferenceLearning {
public:
    struct PreferenceData {
        SimulationStep timestamp;
        std::vector<float> query_context;                    // Context of the query
        std::vector<size_t> preferred_actions;               // Actions that were preferred
        std::vector<size_t> dispreferred_actions;            // Actions that were not preferred
        std::vector<float> preference_strength;              // How strongly they were preferred
        
        PreferenceData() : timestamp(0) {}
    };
    
    struct PreferenceModel {
        std::vector<std::vector<float>> feature_weights;     // Weights for each feature
        std::vector<float> bias_terms;                       // Bias for preferences
        std::vector<float> preference_parameters;            // Additional preference parameters
        std::vector<float> uncertainty_estimates;            // Uncertainty in preferences
        
        PreferenceModel() : feature_weights(10, std::vector<float>(10, 0.0f)),
                          bias_terms(10, 0.0f), preference_parameters(5, 0.0f),
                          uncertainty_estimates(10, 1.0f) {}
    };
    
    PreferenceLearning() : preference_memory_size_(1000), learning_rate_(0.01f),
                          preference_forgetting_rate_(0.99f), 
                          use_context_learning_(true), use_online_learning_(true),
                          reranking_iterations_(3) {}
    
    // Learn preferences from pairwise comparisons
    void learnFromPreferences(const PreferenceData& data);
    
    // Rank actions based on learned preferences
    std::vector<size_t> rankActions(const std::vector<PlanningAction>& actions,
                                   const std::vector<float>& context);
    
    // Get preference-based recommendations
    std::vector<size_t> getRecommendations(const std::vector<PlanningAction>& actions,
                                          const std::vector<float>& context,
                                          size_t top_k = 5);
    
    // Get preference statistics
    struct PreferenceStats {
        size_t total_preferences_learned;
        size_t total_comparisons_made;
        float average_preference_strength;
        float recommendation_accuracy;
        float context_learning_effectiveness;
        
        PreferenceStats() : total_preferences_learned(0), total_comparisons_made(0),
                           average_preference_strength(0.0f), recommendation_accuracy(0.0f),
                           context_learning_effectiveness(0.0f) {}
    };
    
    PreferenceStats getPreferenceStats() const;
    
    // Set parameters
    void setPreferenceMemorySize(size_t size) { preference_memory_size_ = size; }
    void setLearningRate(float rate) { learning_rate_ = rate; }
    void setPreferenceForgettingRate(float rate) { preference_forgetting_rate_ = rate; }
    void setContextLearning(bool enable) { use_context_learning_ = enable; }
    void setOnlineLearning(bool enable) { use_online_learning_ = enable; }
    void setRerankingIterations(size_t iterations) { reranking_iterations_ = iterations; }
    
private:
    std::vector<PreferenceData> preference_data_;
    PreferenceModel preference_model_;
    size_t preference_memory_size_;
    float learning_rate_;
    float preference_forgetting_rate_;
    bool use_context_learning_;
    bool use_online_learning_;
    size_t reranking_iterations_;
    
    // Internal helper methods
    void updatePreferenceModel(const PreferenceData& data);
    float computePreferenceScore(const PlanningAction& action,
                                const std::vector<float>& context,
                                const PreferenceModel& model) const;
    std::vector<size_t> rerankActions(const std::vector<PlanningAction>& actions,
                                     const std::vector<float>& context,
                                     const PreferenceModel& model) const;
    float computeContextRelevance(const std::vector<float>& context,
                                 const PreferenceData& data) const;
    void cleanupOldPreferences();
};

} // namespace nlm
