#pragma once

/**
 * @file NeuralPlanner.hpp
 * @brief Neural action planning and decision-making system
 * 
 * This header defines the NeuralPlanner class for the brain's action planning
 * and decision-making system. The neural planner implements intelligent action
 * selection based on predictive models and value estimation, guiding goal-directed
 * behavior through simulated outcome evaluation and confidence-based planning.
 * 
 * Neural Planning Theory:
 * - Planning emerges from predictive neural dynamics, not search algorithms
 * - Action selection based on value estimation and prediction confidence
 * - Future simulation through internal models and predictive coding
 * - Adaptive planning based on prediction confidence and experience
 * - Integration of multiple information sources (memory, prediction, reward)
 * 
 * Planning Functions:
 * - Action planning: Select best action for current state and goal
 * - Sequence evaluation: Assess multiple action sequences
 * - Confidence computation: Estimate certainty of predictions
 * - Plan updating: Refine plans based on actual outcomes
 * - Cache management: Store and retrieve planning results
 * - Quality learning: Learn action effectiveness from experience
 * 
 * Integration Points:
 * - Connected to prediction system for action-consequence prediction
 * - Modulated by neuromodulation systems (dopamine, curiosity)
 * - Uses reward system for value-based evaluation
 * - Incorporates memory systems (working, episodic, associative)
 * - Interacts with attention system for selective planning
 * - Coordinated with development system for age-appropriate planning
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Forward declarations
class NeuralPlanner;

/**
 * @brief Planning candidate representing a possible action sequence
 * 
 * Represents a potential action sequence considered during planning.
 * Each candidate includes predicted states, expected rewards, and confidence
 * levels to support evaluation and selection of optimal actions.
 * 
 * PlanningCandidate Structure:
 * - actions: Sequence of actions to be executed
 * - predictedStates: Corresponding predicted environmental states
 * - expectedReward: Estimated total reward for the sequence
 * - confidence: Certainty of prediction (0.0-1.0)
 * - depth: Planning horizon length
 * 
 * Candidate Evaluation:
 * - Reward maximization: Higher expected rewards preferred
 * - Confidence weighting: More certain predictions prioritized
 * - Depth consideration: Longer horizons for complex goals
 * - State feasibility: Predicted states must be achievable
 * 
 * Thread Safety: Planning candidates are immutable after creation
 * and can be safely shared between threads.
 */
struct PlanningCandidate {
    /**
     * @brief Sequence of actions in this planning candidate
     * 
     * The ordered list of actions to be executed to achieve the planned outcome.
     * Represents a potential policy from current state to goal state.
     */
    std::vector<ActionType> actions;
    
    /**
     * @brief Predicted environmental states for each action
     * 
     * Sequence of predicted environmental states resulting from each action
     * in the candidate. Used to evaluate feasibility and consequences.
     */
    std::vector<std::vector<float>> predictedStates;
    
    /**
     * @brief Expected total reward for this action sequence
     * 
     * Estimated cumulative reward from executing the entire action sequence.
     * Used for reward-based selection among planning candidates.
     */
    float expectedReward;
    
    /**
     * @brief Confidence in prediction accuracy
     * 
     * Certainty level of the predictions for this action sequence.
     * Higher confidence indicates more reliable predictions.
     */
    float confidence;
    
    /**
     * @brief Planning horizon depth
     * 
     * Number of steps into the future considered in this plan.
     * Larger depth values consider longer-term consequences.
     */
    size_t depth;
    
    /**
     * @brief Default constructor
     */
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
};

/**
 * @brief Neural action planning and decision-making system
 * 
 * Implements intelligent action selection and planning based on predictive
 * models and value estimation. The neural planner guides goal-directed
 * behavior through simulated outcome evaluation and confidence-based planning,
 * complementing reactive control with prospective decision-making.
 * 
 * Neural Planner Architecture:
 * 1. Prediction: Use action-consequence predictions to simulate futures
 * 2. Evaluation: Assess potential futures through simulation
 * 3. Selection: Choose actions based on expected value and confidence
 * 4. Adaptation: Update plans based on prediction confidence and outcomes
 * 5. Caching: Store successful plans for reuse
 * 
 * Planning Mechanisms:
 * - Predictive simulation: Model action consequences through internal models
 * - Value estimation: Evaluate potential futures based on reward prediction
 * - Confidence-based selection: Prefer more certain predictions
 * - Temporal abstraction: Plan at multiple time scales
 * - Hierarchical planning: Coordinate actions across different levels
 * 
 * Thread Safety: NeuralPlanner is thread-safe for concurrent access
 * from different brain components. Planning operations are protected
 * with mutexes.
 * 
 * Memory Management: NeuralPlanner uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note NeuralPlanner is essential for goal-directed behavior and prospective
 *       decision-making. It complements reactive control with intelligent
 *       planning for complex tasks and long-term goals.
 * @see PredictionSystem for action-consequence prediction
 * @see Reward for value-based evaluation
 * @see Neuromodulator for confidence-based modulation
 * @see SelfModel for action consequence modeling
 */
class NeuralPlanner {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a neural planner with initial state. The neural planner implementation
     * uses the pImpl pattern for efficient memory usage and to reduce
     * compilation dependencies.
     */
    NeuralPlanner();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of NeuralPlanner's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~NeuralPlanner();
    
    /**
     * @brief Initialize neural planner with brain reference
     * 
     * Initializes the neural planner with reference to the brain,
     * enabling access to brain's prediction models, memory systems,
     * and other components for context-aware planning.
     * 
     * @param brain Pointer to the brain that contains this neural planner
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Plan next action given current state and goal
     * 
     * Plans the optimal action to take now based on current state and goal.
     * Returns the best action to execute immediately for progress toward goals.
     * 
     * Planning Algorithm:
     * 1. Generate possible action sequences (generateActionSequences)
     * 2. Evaluate each sequence (evaluateSequence)
     * 3. Select action from best sequence (first element)
     * 4. Consider confidence and uncertainty factors
     * 5. Return selected action
     * 
     * @param currentState Current environmental state
     * @param targetReward Target reward value for planning (default 0.5f)
     * @return Best action to take now for goal achievement
     * 
     * @note Planning is based on simulated outcomes rather than search.
     *       Higher confidence predictions are preferred for action selection.
     */
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    /**
     * @brief Evaluate a potential action sequence
     * 
     * Evaluates a specific action sequence from the current state, returning
     * expected total reward and confidence for the sequence. Used for
     * comparing different action sequences during planning.
     * 
     * Evaluation Algorithm:
     * 1. Validate action sequence feasibility
     * 2. Simulate action consequences using predictive models
     * 3. Accumulate expected rewards across steps
     * 4. Compute prediction confidence based on model uncertainty
     * 5. Return PlanningCandidate with results
     * 
     * @param actions Action sequence to evaluate
     * @param startState Starting environmental state
     * @return PlanningCandidate with evaluation results
     * 
     * @note Evaluation considers both expected rewards and prediction confidence.
     *       More confident predictions are weighted more heavily.
     */
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                      const std::vector<float>& startState);
    
    /**
     * @brief Get number of steps to look ahead (planning horizon)
     * 
     * Returns the current planning depth (how many steps into the future
     * the planner considers). Larger values consider longer-term consequences.
     * 
     * @return Current planning depth
     */
    size_t getPlanningDepth() const { return planningDepth_; }
    
    /**
     * @brief Set planning horizon depth
     * 
     * Sets the number of steps to look ahead for planning. Larger values
     * consider longer-term consequences but increase computation time.
     * 
     * @param depth Planning horizon depth
     */
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    
    /**
     * @brief Get planning confidence
     * 
     * Returns the current confidence level in planning predictions.
     * Confidence affects action selection and planning reliability.
     * 
     * @return Current planning confidence (0.0-1.0)
     */
    float getPlanningConfidence() const { return planningConfidence_; }
    
    /**
     * @brief Update plans based on actual outcome
     * 
     * Updates planning confidence and refines predictive models based on
     * actual outcomes compared to planned actions. This is essential for
     * learning and improving planning accuracy.
     * 
     * Plan Update Algorithm:
     * 1. Get planned actions from recent planning
     * 2. Compare with actually executed actions
     * 3. Compute prediction error
     * 4. Update confidence based on error magnitude
     * 5. Refine predictive models if needed
     * 6. Update success history
     * 
     * @param plannedActions Actions that were planned
     * @param actualActions Actions that were actually executed
     * @param actualReward Reward actually received
     */
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    /**
     * @brief Clear planning cache
     * 
     * Removes all cached planning results. This is used to free memory
     * or when planning should be completely recomputed.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation affects planning performance but preserves
     *       configuration parameters.
     */
    void clearCache();
    
    /**
     * @brief Set action quality function from experience
     * 
     * Records the effectiveness of an action based on experience for use
     * in future planning. This allows learning from past experience.
     * 
     * @param action Action to learn quality for
     * @param quality Effectiveness measure (0.0-1.0)
     */
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }
    
    /**
     * @brief Get current goal
     * 
     * Returns the current goal state that the planner is working toward.
     * Goals guide planning and action selection.
     * 
     * @return Current goal state
     */
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    /**
     * @brief Set current goal
     * 
     * Sets the goal state that the planner should work toward. Goals guide
     * planning and action selection toward specific objectives.
     * 
     * @param goal Target goal state
     */
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    /**
     * @brief Has recent planning been successful?
     * 
     * Checks whether recent planning has been successful based on
     * comparison of planned vs. executed actions.
     * 
     * @return true if recent planning was successful, false otherwise
     */
    bool wasRecentPlanSuccessful() const;
    
private:
    /**
     * @brief Generate possible action sequences for planning
     * 
     * Generates a set of possible action sequences given a planning depth.
     * This is a core component of the planning algorithm.
     * 
     * @param depth Planning horizon depth
     * @return Vector of possible action sequences
     */
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);
    
    /**
     * @brief Evaluate single action from state
     * 
     * Evaluates the expected outcome of taking a single action from the
     * current state. Used for immediate action selection.
     * 
     * @param action Action to evaluate
     * @param state Current environmental state
     * @return Expected reward for taking this action
     */
    float evaluateAction(ActionType action, const std::vector<float>& state);
    
    /**
     * @brief Implementation details (pImpl pattern)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Reference to brain for planning context
     */
    Brain* brain_;
    
    /**
     * @brief Current planning horizon depth
     */
    size_t planningDepth_;
    
    /**
     * @brief Current planning confidence
     */
    float planningConfidence_;
    
    /**
     * @brief Current goal state
     */
    std::vector<float> currentGoal_;
    
    /**
     * @brief Action quality from experience
     */
    std::vector<float> actionQuality_;
    
    /**
     * @brief Recent plan success history
     */
    std::deque<bool> recentPlanSuccess_;
};

} // namespace nlm