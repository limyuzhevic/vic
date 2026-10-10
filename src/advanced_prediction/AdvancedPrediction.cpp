#include "AdvancedPrediction.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <numeric>
#include <limits>

namespace nlm {

// Hierarchical Planning with Tree Search implementation

std::vector<HierarchicalPlanner::PlanningAction> HierarchicalPlanner::planActions(
    const std::vector<float>& current_state,
    const std::vector<PlanningAction>& available_actions) {
    
    if (available_actions.empty()) return {};
    
    // Create root node for planning tree
    PlanningNode* root = new PlanningNode(0, current_state);
    
    // Perform MCTS planning
    PlanningNode* best_node = performMCTS(root, available_actions, mcts_simulations_);
    
    // Extract planned actions from best node
    std::vector<PlanningAction> planned_actions;
    PlanningNode* current = best_node;
    while (current != nullptr && current->parent != nullptr) {
        // Find the action that led to this node
        for (const auto& action : available_actions) {
            if (action.action_id == current->id) {
                planned_actions.push_back(action);
                break;
            }
        }
        current = current->parent;
    }
    
    std::reverse(planned_actions.begin(), planned_actions.end());
    
    // Update planning statistics
    planning_stats_.total_planning_steps++;
    planning_stats_.successful_planning_attempts++;
    
    // Cleanup planning tree
    delete best_node;
    
    return planned_actions;
}

HierarchicalPlanner::PlanningNode* HierarchicalPlanner::performMCTS(
    PlanningNode* root,
    const std::vector<PlanningAction>& actions,
    int simulations) {
    
    expandNode(root, actions);
    
    for (int i = 0; i < simulations; ++i) {
        // Selection
        PlanningNode* node = selection(root);
        
        // Expansion
        if (!node->children.empty()) {
            expandNode(node, actions);
        }
        
        // Simulation (rollout)
        float reward = simulateAction(node->planned_actions.back(), 
                                     node->state_features, 1.0f);
        
        // Backpropagation
        backpropagation(node, reward);
    }
    
    // Return the best node
    PlanningNode* best_node = root;
    for (PlanningNode* child : root->children) {
        if (child->getSelectionPriority() > best_node->getSelectionPriority()) {
            best_node = child;
        }
    }
    
    return best_node;
}

void HierarchicalPlanner::expandNode(PlanningNode* node,
                                    const std::vector<PlanningAction>& actions) {
    if (!node || actions.empty()) return;
    
    for (const auto& action : actions) {
        if (action.action_id >= node->id) {  // Avoid cycles
            PlanningNode* child = new PlanningNode(action.action_id, node->state_features, node);
            node->children.push_back(child);
            
            // Store action in child
            child->planned_actions = node->planned_actions;
            child->planned_actions.push_back(action);
            
            // Initialize action value
            child->action_value = 0.0f;
        }
    }
}

float HierarchicalPlanner::simulateAction(const PlanningAction& action,
                                         const std::vector<float>& current_state,
                                         float dt) {
    // Simple simulation - in real implementation would use forward model
    float reward = 0.0f;
    
    // Apply action effects
    for (float outcome : action.expected_outcomes) {
        reward += outcome * action.probability;
    }
    
    // Add state-dependent bonus
    float state_bonus = 0.0f;
    for (float feature : current_state) {
        state_bonus += feature * action.action_features.size() > 0 ? 
                      action.action_features[0] : 0.0f;
    }
    
    return reward + state_bonus * action.value;
}

void HierarchicalPlanner::backpropagation(PlanningNode* node, float reward) {
    while (node != nullptr) {
        node->visit_count++;
        node->total_value += reward;
        
        // Update node values
        if (use_value_network_) {
            node->value_estimate = (node->value_estimate * (node->visit_count - 1) + reward) / 
                                  node->visit_count;
        }
        
        // Update policy score
        if (use_policy_network_) {
            node->policy_score = node->getSelectionPriority() / (exploration_constant_ + 1.0f);
        }
        
        // Move to parent
        node = node->parent;
    }
}

HierarchicalPlanner::PlanningStats HierarchicalPlanner::getPlanningStats() const {
    PlanningStats stats = planning_stats_;
    
    if (planning_stats_.total_planning_steps > 0) {
        stats.average_tree_size = 0.0f;  // Would calculate from actual trees
        stats.average_action_value = stats.successful_planning_attempts > 0 ? 
                                    stats.average_action_value / stats.successful_planning_attempts : 0.0f;
        stats.planning_efficiency = (stats.successful_planning_attempts / planning_stats_.total_planning_steps) * 100.0f;
    }
    
    return stats;
}

// Monte Carlo Tree Search implementation

MonteCarloTreeSearch::SearchResult MonteCarloTreeSearch::search(
    const std::vector<float>& initial_state,
    const std::vector<std::vector<float>>& available_actions) {
    
    // Create root node
    MCTSNode* root = new MCTSNode(0, initial_state, std::vector<float>(), nullptr);
    
    // Store active tree
    active_trees_.push_back(root);
    
    SearchResult result;
    
    for (size_t sim = 0; sim < max_tree_size_; ++sim) {
        // Selection
        MCTSNode* node = selection(root);
        
        // Expansion
        std::vector<MCTSNode*> expanded_nodes = expansion(node, available_actions);
        
        // Simulation (rollout)
        float reward = simulation(node, available_actions, rollout_policy_);
        
        // Backpropagation
        backpropagation(node, reward);
        
        // Update active trees
        if (backup_all_paths_) {
            backpropagation(root, reward);
        }
    }
    
    // Get best action
    size_t best_action_id = 0;
    float best_value = -std::numeric_limits<float>::max();
    
    for (MCTSNode* child : root->children) {
        float value = child->getAverageValue();
        if (value > best_value) {
            best_value = value;
            best_action_id = child->action_id;
        }
    }
    
    result.selected_action = best_action_id;
    result.action_distribution = computeActionProbabilities(root->children, exploration_temperature_);
    result.confidence = root->visit_count > 0 ? 1.0f / (1.0f + 1.0f / root->visit_count) : 0.0f;
    result.estimated_value = best_value;
    result.planning_time = 0.0f;  // Would track actual time
    
    // Update statistics
    mc_stats_.total_simulations++;
    if (result.confidence > 0.5f) {
        mc_stats_.successful_searches++;
    }
    
    // Cleanup
    for (MCTSNode* child : root->children) {
        delete child;
    }
    delete root;
    
    // Remove from active trees
    active_trees_.clear();
    
    return result;
}

MonteCarloTreeSearch::MCTSNode* MonteCarloTreeSearch::selection(MCTSNode* root) {
    MCTSNode* node = root;
    
    while (!node->children.empty()) {
        // Find child with best UCB score
        MCTSNode* best_child = nullptr;
        float best_score = -std::numeric_limits<float>::max();
        
        for (MCTSNode* child : node->children) {
            float score = child->getUCBScore(exploration_constant_);
            if (score > best_score) {
                best_score = score;
                best_child = child;
            }
        }
        
        if (best_child) {
            node = best_child;
        } else {
            break;
        }
    }
    
    return node;
}

std::vector<MonteCarloTreeSearch::MCTSNode*> MonteCarloTreeSearch::expansion(
    MCTSNode* node,
    const std::vector<std::vector<float>>& available_actions) {
    
    std::vector<MCTSNode*> new_nodes;
    
    // Convert available actions to features (simplified)
    for (size_t i = 0; i < std::min(available_actions.size(), static_cast<size_t>(5)); ++i) {
        MCTSNode* child = new MCTSNode(
            static_cast<size_t>(i + 1),  // Dummy action IDs
            node->state_features,
            available_actions[i],
            node
        );
        
        node->children.push_back(child);
        new_nodes.push_back(child);
    }
    
    return new_nodes;
}

float MonteCarloTreeSearch::simulation(MCTSNode* node,
                                       const std::vector<std::vector<float>>& available_actions,
                                       const std::string& policy) {
    if (policy == "random") {
        // Random simulation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        float reward = 0.0f;
        for (size_t i = 0; i < simulation_horizon_; ++i) {
            reward += dist(gen) * 0.1f - 0.05f;  // Random small rewards
        }
        return reward;
        
    } else if (policy == "heuristic") {
        // Heuristic simulation
        return evaluateStateHeuristic(node->state_features);
        
    } else {  // "uct"
        // UCT simulation (default)
        return evaluateStateHeuristic(node->state_features);
    }
}

void MonteCarloTreeSearch::backpropagation(MCTSNode* node, float reward) {
    MCTSNode* current = node;
    
    while (current != nullptr) {
        current->visit_count++;
        current->total_value += reward;
        
        // Update policy
        if (current->parent) {
            current->parent->visit_count++;
        }
        
        current = current->parent;
    }
}

MonteCarloTreeSearch::MCStats MonteCarloTreeSearch::getMCStats() const {
    MCStats stats = mc_stats_;
    
    if (mc_stats_.total_simulations > 0) {
        stats.average_tree_size = static_cast<float>(active_trees_.size());
        stats.average_search_time = 0.0f;  // Would track actual time
        stats.uct_usage_rate = rollout_policy_ == "uct" ? 1.0f : 0.0f;
    }
    
    return stats;
}

float MonteCarloTreeSearch::evaluateStateHeuristic(const std::vector<float>& state) const {
    if (state.empty()) return 0.0f;
    
    // Simple heuristic evaluation
    float value = 0.0f;
    for (float feature : state) {
        value += feature;
    }
    
    return value / state.size();
}

float MonteCarloTreeSearch::evaluateActionHeuristic(const std::vector<float>& action,
                                                    const std::vector<float>& state) const {
    if (action.empty() || state.empty()) return 0.0f;
    
    // Simple action heuristic
    float value = 0.0f;
    for (float act_feature : action) {
        for (float state_feature : state) {
            value += act_feature * state_feature;
        }
    }
    
    return value / (action.size() * state.size());
}

std::vector<float> MonteCarloTreeSearch::computeActionProbabilities(
    const std::vector<MCTSNode*>& children,
    float temperature) const {
    
    std::vector<float> probabilities;
    
    if (children.empty()) return probabilities;
    
    // Compute raw values from children
    std::vector<float> values;
    float max_value = -std::numeric_limits<float>::max();
    
    for (const MCTSNode* child : children) {
        values.push_back(child->getAverageValue());
        max_value = std::max(max_value, child->getAverageValue());
    }
    
    // Apply temperature scaling
    std::vector<float> scaled_values;
    for (float value : values) {
        scaled_values.push_back(value / temperature);
    }
    
    // Compute softmax probabilities
    float sum_exp = 0.0f;
    for (float value : scaled_values) {
        sum_exp += std::exp(value);
    }
    
    for (float value : scaled_values) {
        probabilities.push_back(std::exp(value) / (sum_exp + 1e-8f));
    }
    
    return probabilities;
}

void MonteCarloTreeSearch::pruneTree(MCTSNode* root) {
    if (!root) return;
    
    // Remove nodes with low visit counts
    std::vector<MCTSNode*> to_delete;
    
    for (MCTSNode* child : root->children) {
        if (child->visit_count < 5) {  // Minimum visits threshold
            to_delete.push_back(child);
        }
    }
    
    for (MCTSNode* node : to_delete) {
        for (MCTSNode* child : node->children) {
            delete child;
        }
        auto it = std::find(root->children.begin(), root->children.end(), node);
        if (it != root->children.end()) {
            root->children.erase(it);
        }
        delete node;
    }
}

void MonteCarloTreeSearch::cleanupInactiveTrees() {
    active_trees_.clear();
}

float MonteCarloTreeSearch::computeReplayPriority(const MCTSNode* node, const MCTSNode& trace) const {
    float priority = node->getAverageValue();
    
    // Boost priority for nodes with higher visit counts
    priority *= std::log(static_cast<float>(node->visit_count + 1));
    
    return priority;
}

// Forward Modeling implementation

void ForwardModeling::learnForwardModel(const std::vector<float>& current_state,
                                       const std::vector<float>& next_state,
                                       const std::vector<float>& action) {
    if (current_state.empty() || next_state.empty() || action.empty()) return;
    
    // Update transition weights (simplified linear model)
    size_t state_dim = current_state.size();
    size_t action_dim = action.size();
    
    // Resize transition weights if needed
    if (forward_model_.transition_weights.empty() || 
        forward_model_.transition_weights[0].size() != state_dim) {
        forward_model_.transition_weights.resize(state_dim, std::vector<float>(state_dim, 0.0f));
    }
    
    // Update based on new transition
    for (size_t i = 0; i < state_dim && i < next_state.size(); ++i) {
        for (size_t j = 0; j < state_dim && j < current_state.size(); ++j) {
            // Simple learning rule
            float weight_update = learning_rate_ * (next_state[i] - current_state[j]);
            forward_model_.transition_weights[i][j] += weight_update;
        }
    }
    
    // Update noise parameters
    float noise = 0.0f;
    for (size_t i = 0; i < state_dim && i < next_state.size(); ++i) {
        noise += std::abs(next_state[i] - current_state[i]);
    }
    noise /= state_dim;
    
    forward_model_.noise_parameters.push_back(noise);
    if (forward_model_.noise_parameters.size() > 100) {
        forward_model_.noise_parameters.erase(forward_model_.noise_parameters.begin());
    }
    
    // Update uncertainty
    float avg_noise = std::accumulate(forward_model_.noise_parameters.begin(),
                                     forward_model_.noise_parameters.end(), 0.0f) /
                     forward_model_.noise_parameters.size();
    
    forward_model_.model_uncertainty.push_back(1.0f / (avg_noise + 0.01f));
    if (forward_model_.model_uncertainty.size() > 100) {
        forward_model_.model_uncertainty.erase(forward_model_.model_uncertainty.begin());
    }
    
    // Update statistics
    forward_model_stats_.total_transitions_learned++;
    forward_model_stats_.prediction_error += noise;
}

std::vector<float> ForwardModeling::predictFutureState(const std::vector<float>& current_state,
                                                      const std::vector<float>& action,
                                                      float dt) {
    if (current_state.empty()) return {};
    
    std::vector<float> predicted_state = current_state;
    
    // Apply forward dynamics
    for (size_t i = 0; i < forward_model_.prediction_horizon && i < predicted_state.size(); ++i) {
        // Simple prediction
        float update = 0.0f;
        for (size_t j = 0; j < predicted_state.size(); ++j) {
            update += forward_model_.transition_weights.size() > j && 
                     i < forward_model_.transition_weights[j].size() ?
                     forward_model_.transition_weights[j][i] * predicted_state[j] : 0.0f;
        }
        
        predicted_state[i] += update * dt + std::rand() % 100 / 100.0f * 0.1f;
    }
    
    return predicted_state;
}

std::vector<std::vector<float>> ForwardModeling::predictMultipleStates(
    const std::vector<float>& current_state,
    const std::vector<float>& action,
    size_t num_predictions) {
    
    std::vector<std::vector<float>> predictions;
    
    for (size_t i = 0; i < num_predictions; ++i) {
        // Add noise to create multiple predictions
        std::vector<float> prediction = predictFutureState(current_state, action, 1.0f);
        
        // Add random noise for diversity
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
        
        for (float& val : prediction) {
            val += dist(gen);
        }
        
        predictions.push_back(prediction);
    }
    
    return predictions;
}

ForwardModeling::InversePlanningSolution ForwardModeling::inversePlan(
    const std::vector<float>& current_state,
    const std::vector<PlanningAction>& available_actions,
    size_t planning_horizon) {
    
    InversePlanningSolution solution;
    
    if (available_actions.empty()) return solution;
    
    // Evaluate each action
    float best_value = std::numeric_limits<float>::max();
    
    for (const auto& action : available_actions) {
        // Simulate action sequence
        std::vector<size_t> action_sequence;
        float total_cost = 0.0f;
        
        for (size_t step = 0; step < planning_horizon; ++step) {
            // Find best action at this step
            size_t best_action_id = action.action_id;  // Simplified
            action_sequence.push_back(best_action_id);
            
            // Estimate cost (simplified)
            total_cost += 1.0f - action.probability;
        }
        
        if (total_cost < best_value) {
            best_value = total_cost;
            solution.action_sequence = action_sequence;
            solution.best_action_id = action.action_id;
        }
    }
    
    solution.solution_cost = best_value;
    solution.solution_confidence = 1.0f / (best_value + 1.0f);
    solution.computational_cost = static_cast<float>(planning_horizon) * 0.1f;
    
    return solution;
}

ForwardModeling::ForwardModelStats ForwardModeling::getForwardModelStats() const {
    ForwardModelStats stats = forward_model_stats_;
    
    if (forward_model_stats_.total_transitions_learned > 0) {
        stats.average_model_accuracy = 1.0f - 
            (stats.prediction_error / forward_model_stats_.total_transitions_learned);
        stats.model_complexity = static_cast<float>(forward_model_.transition_weights.size());
        stats.inference_efficiency = 1.0f / (stats.prediction_error + 0.01f);
    }
    
    return stats;
}

std::vector<float> ForwardModeling::applyForwardDynamics(const std::vector<float>& state,
                                                         const std::vector<float>& action) {
    std::vector<float> result = state;
    
    // Apply dynamics (simplified)
    for (size_t i = 0; i < state.size(); ++i) {
        float update = 0.0f;
        for (size_t j = 0; j < state.size(); ++j) {
            update += forward_model_.transition_weights.size() > j && 
                     i < forward_model_.transition_weights[j].size() ?
                     forward_model_.transition_weights[j][i] * state[j] : 0.0f;
        }
        
        result[i] += update;
    }
    
    return result;
}

float ForwardModeling::computeTransitionProbability(const std::vector<float>& state_from,
                                                   const std::vector<float>& state_to,
                                                   const std::vector<float>& action) const {
    // Simple transition probability computation
    float similarity = 0.0f;
    for (size_t i = 0; i < std::min(state_from.size(), state_to.size()); ++i) {
        similarity += std::abs(state_from[i] - state_to[i]);
    }
    
    return std::exp(-similarity);
}

float ForwardModeling::computeInversePlanningCost(const std::vector<size_t>& action_sequence,
                                                 const std::vector<float>& current_state) const {
    float cost = 0.0f;
    
    for (size_t action_id : action_sequence) {
        cost += 1.0f / (action_id + 1.0f);  // Prefer shorter action IDs
    }
    
    return cost;
}

std::vector<float> ForwardModeling::simulateActionSequence(const std::vector<size_t>& action_sequence,
                                                         const std::vector<float>& initial_state) const {
    std::vector<float> current_state = initial_state;
    
    for (size_t action_id : action_sequence) {
        // Simple action simulation (simplified)
        for (float& val : current_state) {
            val += static_cast<float>(action_id) * 0.01f;
        }
    }
    
    return current_state;
}

float ForwardModeling::evaluatePlanQuality(const std::vector<size_t>& action_sequence,
                                         const std::vector<float>& current_state) const {
    float quality = 0.0f;
    
    for (size_t action_id : action_sequence) {
        quality += 1.0f / (action_id + 1.0f);  // Prefer shorter sequences
    }
    
    return quality;
}

} // namespace nlm
