#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

/**
 * @class PlanningCandidate
 * @brief Represents a possible action sequence considered during planning
 * @description Contains information about a planned action sequence including the
 *              sequence of actions, predicted states at each step, expected reward,
 *              and confidence in the prediction. Used for planning candidate evaluation.
 * @version 1.0.0
 */
struct PlanningCandidate {
    std::vector<ActionType> actions;
    std::vector<std::vector<float>> predictedStates;
    float expectedReward;
    float confidence;
    size_t depth;
    
    /**
     * @brief Default constructor
     * @description Initializes all fields to default values (0 for numeric types).
     *              Planning candidates start with zero reward, confidence, and depth.
     */
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
};

/**
 * @class NeuralPlanner
 * @brief Plans multi-step actions using learned predictions (emergent planning)
 * @description Uses neural predictive dynamics to evaluate potential action sequences
 *              and select the best immediate action. Unlike traditional search algorithms,
 *              planning emerges from the brain's predictive capabilities and does not
 *              require explicit tree search or optimization algorithms.
 * @note This is NOT a search engine. Planning emerges from neural dynamics
 * @warning During development phase, actual planning implementation may be PLACEHOLDER
 * @version 1.0.0
 * @copyright Copyright (c) 2024 NLM Team
 */
class NeuralPlanner {
public:
    /**
     * @brief Construct neural planner instance
     * @description Creates a new planning system. Requires initialization with
     *              brain reference before planning can begin. The planner learns
     *              action quality from experience over time.
     */
    NeuralPlanner();
    
    /**
     * @virtual Destructor
     * @description Cleans up planner resources and learned action quality data.
     */
    ~NeuralPlanner();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to brain system
     * @description Initializes the planner with access to brain components such
     *              as neurons, motor systems, and predictive systems. This is required
     *              for making predictions and accessing action outcomes.
     * @warning Brain pointer must remain valid throughout planner lifetime
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Plan next action given current state and goal
     * @param currentState Current sensory/motor state vector
     * @param targetReward Desired reward (default: 0.5)
     * @return ActionType Best action to execute now
     * @description Evaluates all possible immediate actions based on learned
     *              action quality and current state, returning the action with highest
     *              expected value. Plans are emergent from predictive dynamics.
     * @note Returns the single best action for immediate execution, not a full plan
     * @see evaluateSequence
     */
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    /**
     * @brief Evaluate a potential action sequence
     * @param actions Sequence of actions to evaluate
     * @param startState Starting state for the sequence
     * @return PlanningCandidate Contains rewards, confidence, and evaluation details
     * @description Thoroughly evaluates a multi-step action sequence using learned
     *              predictions about action consequences and outcomes. Returns a full
     *              candidate including expected reward and confidence.
     * @note This is the core planning function used by planAction()
     */
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                      const std::vector<float>& startState);
    
    /**
     * @brief Get number of steps to look ahead
     * @return Planning depth (number of steps to evaluate)
     * @description Returns the current planning horizon - how many steps ahead
     *              the planner looks when evaluating actions. Higher values enable
     *              more thorough planning but increase computation time.
     */
    size_t getPlanningDepth() const { return planningDepth_; }
    
    /**
     * @brief Set number of steps to look ahead
     * @param depth New planning depth
     * @description Configures how many steps ahead the planner should look when
     *              evaluating actions. Depth typically ranges from 1 (greedy) to
     *              several steps for longer-term planning.
     */
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    
    /**
     * @brief Get planning confidence
     * @return Planning confidence (0.0 to 1.0)
     * @description Returns the planner's confidence in its predictions and evaluations.
     *              Confidence reflects the reliability of learned action outcomes and
     *              prediction accuracy.
     * @note High confidence indicates reliable predictions, low confidence indicates
     *       uncertainty that may require more conservative planning
     */
    float getPlanningConfidence() const { return planningConfidence_; }
    
    /**
     * @brief Update plans based on actual outcome
     * @param plannedActions Actions that were planned
     * @param actualActions Actions that were actually executed
     * @param actualReward Actual reward received
     * @description Updates the planner's internal models based on the difference
     *              between planned and actual outcomes. Refines action quality
     *              estimates and prediction accuracy.
     * @note This learning mechanism allows the planner to adapt to unexpected outcomes
     */
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    /**
     * @brief Clear planning cache
     * @description Clears all learned action quality data and prediction cache.
     *              Resets the planner to an uninitialized state. Useful for
     *              starting fresh or resetting learned behaviors.
     * @note After clearing cache, planner will need to relearn action qualities
     */
    void clearCache();
    
    /**
     * @brief Set action quality function (from experience)
     * @param action Action type to set quality for
     * @param quality Quality value (typically 0.0 to 1.0)
     * @description Stores learned action quality based on experience. Quality
     *              represents how good the action is at producing desired outcomes
     *              or avoiding negative outcomes.
     * @note Action qualities are updated through updatePlanQuality() based on
     *       actual outcomes
     */
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }
    
    /**
     * @brief Get current goal
     * @return Current goal state vector
     * @description Returns the brain's current goal or target state that planning
     *              should work towards. Goals can be set by higher-level cognitive systems.
     */
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    /**
     * @brief Set current goal
     * @param goal New goal state vector
     * @description Configures the current goal that planning should work towards.
     *              Goals are state vectors representing desired outcomes or targets
     *              that the agent wants to achieve.
     * @note Goals guide planning by providing desired outcomes to work towards
     */
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    /**
     * @brief Check if recent planning has been successful
     * @return true if recent plans matched expected outcomes
     * @description Returns true if the planner's recent predictions and plans
     *              matched the actual outcomes, indicating successful planning.
     * @note Used for monitoring planner performance and adapting confidence
     */
    bool wasRecentPlanSuccessful() const;
    
private:
    /**
     * @brief Generate possible action sequences
     * @param depth Planning depth (number of steps to generate)
     * @return Vector of action sequences (each sequence is a vector of actions)
     * @description Generates all possible action sequences up to the specified depth.
     *              This creates the search space that the planner evaluates.
     * @note Sequence generation is exponential - planning depth controls complexity
     */
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);
    
    /**
     * @brief Evaluate single action from current state
     * @param action Action to evaluate
     * @param state Current state vector
     * @return Expected reward for this action (0.0 to 1.0)
     * @description Computes the expected reward for taking a specific action
     *              from the current state, based on learned action qualities and
     *              predictive models.
     * @note Used by generateActionSequences() to score action sequences
     */
    float evaluateAction(ActionType action, const std::vector<float>& state);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t planningDepth_;
    float planningConfidence_;
    
    /**
     * @brief Current goal state
     * @note Goal state guides planning by specifying desired outcomes
     */
    std::vector<float> currentGoal_;
    
    /**
     * @brief Action quality from experience
     * @note Indexed by ActionType enum value, stores learned quality of each action
     */
    std::vector<float> actionQuality_;
    
    /**
     * @brief Recent plan success history
     * @note Rolling window of recent plan successes for confidence calculation
     */
    std::deque<bool> recentPlanSuccess_;
};

/**
 * @class SelfModel
 * @brief Represents the agent's internal sensorimotor model of itself
 * @description Implements self-awareness through experience of cause-effect relationships
 *              between actions and sensory states. The self-model learns how the agent's own
 *              actions affect the world and uses this knowledge for prediction and control.
 * @note NOT consciousness - represents sensorimotor self-awareness only
 * @warning Currently in development phase
 * @version 1.0.0
 */
class SelfModel {
public:
    /**
     * @brief Construct self-model instance
     * @description Creates a new self-model. Requires initialization with brain reference
     *              before learning can begin. The self-model starts with no learned effects.
     */
    SelfModel();
    
    /**
     * @virtual Destructor
     * @description Cleans up self-model resources and learned effects.
     */
    ~SelfModel();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to brain system
     * @description Initializes the self-model with access to brain components for
     *              recording actions and their effects on sensory states.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record that taking an action caused a specific sensory change
     * @param action Action that was taken
     * @param beforeState Sensory state before action
     * @param afterState Sensory state after action
     * @description Records cause-effect relationships between actions and their
     *              sensory consequences. This builds the agent's internal model of
     *              how its actions affect the environment.
     */
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    /**
     * @brief Predict sensory consequence of an action
     * @param action Action to predict consequences for
     * @param currentState Current sensory state
     * @return Predicted sensory state after action
     * @description Uses learned action effects to predict what sensory states
     *              will result from taking specific actions. This is the forward
     *              model of the agent's internal representation.
     */
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    /**
     * @brief Get confidence in self-model for a given action
     * @param action Action to get confidence for
     * @return Confidence level (0.0 to 1.0)
     * @description Returns how confident the self-model is in its prediction of
     *              the action's consequences based on how many similar observations exist.
     */
    float getSelfModelConfidence(ActionType action) const;
    
    /**
     * @brief Compute self-generated likeness between states
     * @param beforeState State before action
     * @param afterState State after action
     * @param action Action that was taken
     * @return Likeness score (0.0 to 1.0)
     * @description Computes how likely the state change was caused by the agent's
     *              own action versus external factors. Higher scores indicate more
     *              self-generated changes.
     */
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       ActionType action) const;
    
    /**
     * @brief Get the body schema (preferred actions in different states)
     * @param state Sensory state vector
     * @return Preferred action for this state
     * @description Returns the action that has been learned as most effective
     *              or preferred in the given sensory state. Represents the agent's
     *              internal policy.
     */
    ActionType getPreferredAction(const std::vector<float>& state);
    
    /**
     * @brief Update self-model based on prediction error
     * @param predicted Predicted state (from self-model)
     * @param actual Actual state (from experience)
     * @param action Action that was taken
     * @description Adjusts the self-model when predictions don't match reality.
     *              This is the learning mechanism that refines the internal model
     *              based on prediction errors.
     */
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);
    
    /**
     * @brief Get current capability level
     * @return Capability level (0.0 to 1.0)
     * @description Returns the agent's learned capability level based on past successes
     *              and failures. Represents the agent's confidence in its abilities.
     */
    float getCapabilityLevel() const { return capabilityLevel_; }
    
    /**
     * @brief Get body awareness (prediction accuracy)
     * @return Body awareness score (0.0 to 1.0)
     * @description Returns how accurately the self-model predicts the consequences
     *              of its own actions. Higher awareness means better self-prediction.
     */
    float getBodyAwareness() const;
    
    /**
     * @brief Clear self-model
     * @description Removes all learned action effects and resets the self-model.
     *              The agent will need to relearn its sensorimotor relationships.
     */
    void clear();
    
    /**
     * @brief Check if self-model has formed
     * @return true if any action effects have been learned
     * @description Returns true if the self-model has learned at least one action-effect
     *              relationship, indicating basic sensorimotor awareness exists.
     */
    bool hasSelfModel() const { return !actionEffects_.empty(); }
    
private:
    /**
     * @brief Find best matching previous experience
     * @param action Action to find matches for
     * @param beforeState State before action
     * @return Best matching afterState from previous experience
     * @description Searches through learned action effects to find the most similar
     *              previous experience for a given action and starting state.
     */
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float capabilityLevel_;
    
    /**
     * @brief Action -> (beforeState -> afterState) mappings
     * @note These mappings store the learned sensorimotor relationships that form
     *       the self-model. Each action can have multiple effects depending on state.
     */
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
    };
    
    /**
     * @brief All learned action effects
     * @note Indexed by ActionType, stores all effects for each action
     */
    std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
};

/**
 * @class SocialLearning
 * @brief Enables learning from observing other agents
 * @description Allows the agent to learn about other agents' behaviors and effects
 *              through observation, without direct interaction. Supports imitation,
 *              communication signal learning, and social knowledge accumulation.
 * @note NOT language - simple action-effect learning from observation only
 * @warning Currently in development phase
 * @version 1.0.0
 */
class SocialLearning {
public:
    /**
     * @brief Construct social learning system
     * @description Creates a new social learning system. Requires initialization with
     *              brain reference before learning from others can begin.
     */
    SocialLearning();
    
    /**
     * @virtual Destructor
     * @description Cleans up social learning resources and learned social knowledge.
     */
    ~SocialLearning();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to brain system
     * @description Initializes social learning with access to brain components for
     *              processing observed actions and their effects.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record observation of another agent's action and effect
     * @param observedAction Action taken by other agent
     * @param observerState State before observation
     * @param resultingState State after observation
     * @description Records observations of other agents' actions and their effects
     *              on the environment. This builds social knowledge about other agents.
     */
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    /**
     * @brief Check if can imitate this observed action
     * @param observedAction Action to check for imitability
     * @return true if action can be successfully imitated
     * @description Returns true if the agent has learned enough about this action
     *              to successfully imitate it based on observed effects.
     */
    bool canImitate(ActionType observedAction) const;
    
    /**
     * @brief Get the best action to imitate given current state
     * @param currentState Current sensory state
     * @return Best action to imitate
     * @description Returns the action that has been learned from observation and
     *              is most appropriate for the current state.
     */
    ActionType getImitationAction(const std::vector<float>& currentState);
    
    /**
     * @brief Learn simple communication signal from another agent
     * @param signalPattern Neural pattern that predicts reward from other agent
     * @param signalReward Associated reward value
     * @description Learns that a specific neural pattern (signal) predicts reward
     *              from another agent. This forms the basis for simple communication.
     */
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);
    
    /**
     * @brief Detect if another agent is signaling
     * @param neuralPattern Neural pattern to check
     * @return true if pattern matches learned signal
     * @description Returns true if the given neural pattern matches the learned
     *              communication signal from another agent.
     */
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    /**
     * @brief Get learned signal pattern
     * @return Learned communication signal pattern
     * @description Returns the neural pattern that has been learned as a signal
     *              from another agent, predicting associated reward.
     */
    std::vector<float> getSignalPattern() const;
    
    /**
     * @brief Get signal meaning (associated reward)
     * @return Associated reward value
     * @description Returns the reward value associated with the learned communication
     *              signal. This is what the agent expects from other agents when they signal.
     */
    float getSignalMeaning() const;
    
    /**
     * @brief Update social knowledge based on interactions
     * @param interactionReward Reward from social interaction
     * @description Updates social knowledge and refines beliefs about other agents
     *              based on rewards received from social interactions.
     */
    void updateSocialKnowledge(float interactionReward);
    
    /**
     * @brief Clear social learning
     * @description Removes all learned social knowledge, observations, and signals.
     *              Resets the social learning system to an uninitialized state.
     */
    void clear();
    
    /**
     * @brief Check if has learned from others
     * @return true if any observations have been recorded
     * @description Returns true if the agent has observed any other agents' actions
     *              and learned from them. Indicates existing social knowledge.
     */
    bool hasSocialKnowledge() const { return observationCount_ > 0; }
    
    /**
     * @brief Get observation count
     * @return Number of observations recorded
     * @description Returns the total number of other agents' actions that have been
     *              observed and learned from.
     */
    size_t getObservationCount() const { return observationCount_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t observationCount_;
    
    /**
     * @brief Observed action effects (other agent's actions)
     * @note These store the effects of observing other agents' actions, which can
     *       be used for imitation learning and social coordination.
     */
    struct ObservedEffect {
        std::vector<float> state;
        std::vector<float> resultingState;
        float reward;
    };
    
    /**
     * @brief All observed actions from other agents
     * @note Stores pairs of observed actions and their effects for later imitation
     */
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    
    /**
     * @brief Communication signal learned from other agents
     * @note Represents a simple communication signal that predicts reward from
     *       other agents, forming the basis for rudimentary social signaling.
     */
    std::vector<float> signalPattern_;
    float signalMeaning_;
};

} // namespace nlm

// SelfModel: Represents the agent's internal model of itself
// NOT consciousness - sensorimotor self-awareness through experience
//
// Key mechanisms:
// - Learns body schema (how actions affect sensory state)
// - Predicts consequences of own actions
// - Distinguishes self from external events
// - Represents own capabilities and limitations

class SelfModel {
public:
    SelfModel();
    ~SelfModel();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record that taking an action caused a specific sensory change
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);

    // Predict sensory consequence of an action
    // This is the "forward model" of the self
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);

    // Get confidence in self-model for a given action
    float getSelfModelConfidence(ActionType action) const;

    // Is this change likely caused by self (action) vs external?
    float computeSelfGenerated Likeness(const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       ActionType action) const;

    // Get the body schema (preferred actions in different states)
    // Returns map of state -> preferred action
    ActionType getPreferredAction(const std::vector<float>& state);

    // Update self-model based on prediction error
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);

    // Get current capability level (0-1)
    float getCapabilityLevel() const { return capabilityLevel_; }

    // Get body awareness (how accurately can predict consequences)
    float getBodyAwareness() const;

    // Clear self-model
    void clear();

    // Has self-model formed?
    bool hasSelfModel() const { return !actionEffects_.empty(); }

private:
    // Find best matching previous experience
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float capabilityLevel_;
    
    // Action -> (beforeState -> afterState) mappings
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
};

// SocialLearning: Enables learning from observing other agents
// NOT language - simple action-effect learning from observation
//
// Key mechanisms:
// - Observes other agent's actions
// - Learns that other agents cause predictable changes
// - Can imitate observed actions
// - Develops simple communication signals

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record observation of another agent's action and effect
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);

    // Can I imitate this observed action?
    bool canImitate(ActionType observedAction) const;

    // Get the best action to imitate given current state
    ActionType getImitationAction(const std::vector<float>& currentState);

    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);

    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const;

    // Get learned signal pattern
    std::vector<float> getSignalPattern() const;

    // Get signal meaning (associated reward)
    float getSignalMeaning() const;

    // Update social knowledge based on interactions
    void updateSocialKnowledge(float interactionReward);

    // Clear social learning
    void clear();

    // Has learned from others?
    bool hasSocialKnowledge() const { return observationCount_ > 0; }

    // Get observation count
    size_t getObservationCount() const { return observationCount_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t observationCount_;
    
    // Observed action effects (other agent's actions)
    struct ObservedEffect {
        std::vector<float> state;
        std::vector<float> resultingState;
        float reward;
    };
    
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    
    // Communication signal
    std::vector<float> signalPattern_;
    float signalMeaning_;
};

} // namespace nlm