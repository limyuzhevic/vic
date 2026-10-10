#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <deque>

namespace nlm {

/**
 * @brief PlanningCandidate: A possible action sequence considered during planning
 * 
 * This structure represents a complete action sequence that the neural planner
 * considers when making decisions. It includes the planned actions, predicted
 * future states, expected outcomes, and confidence in the prediction.
 * 
 * Used for:
 * - Evaluating potential action sequences
 * - Comparing alternative plans
 * - Learning from successful/unsuccessful plans
 * - Updating planning strategies based on outcomes
 * 
 * Key features:
 * - Multi-step action sequences with predicted consequences
 * - Expected reward and confidence scoring
 * - Temporal depth for forward-looking planning
 * - Integration with reward prediction systems
 */
struct PlanningCandidate {
    std::vector<ActionType> actions;                 ///< Planned action sequence
    std::vector<std::vector<float>> predictedStates; ///< Predicted future states for each step
    float expectedReward;                             ///< Predicted total reward for this sequence
    float confidence;                                  ///< Confidence in prediction (0.0 to 1.0)
    size_t depth;                                      ///< Number of steps in the action sequence
    
    /**
     * @brief Default constructor creates an empty planning candidate
     * 
     * @note All values are initialized to zero/empty. Use the parameterized
     *       constructor for meaningful planning candidates.
     */
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
};

/**
 * @brief NeuralPlanner: Uses learned predictions to plan multi-step actions
 * 
 * The NeuralPlanner implements predictive action planning through forward
 * modeling and reward prediction. Unlike traditional search-based planners,
 * NeuralPlanner's planning emerges from learned prediction dynamics and
 * neural mechanisms rather than explicit computational search.
 * 
 * Key mechanisms:
 * - Uses action-consequence predictions for forward simulation
 * - Evaluates potential futures through learned prediction models
 * - Selects actions leading to high-value outcomes based on predictions
 * - Adapts plans based on prediction confidence and experience
 * 
 * Phase 6 Integration: Part of the complete cognitive system that integrates
 * with perception, memory, and action systems to enable intelligent behavior
 * through predictive processing.
 * 
 * Planning architecture:
 * 1. Current state analysis using forward models
 * 2. Generation of possible action sequences
 * 3. Evaluation of predicted outcomes
 * 4. Selection of optimal action based on predictions
 * 5. Continuous adaptation based on experience
 * 
 * The planner operates as a predictive controller, using learned forward
 * models to simulate potential futures and select actions that maximize
 * expected rewards while considering prediction uncertainty.
 */
class NeuralPlanner {
public:
    /**
     * @brief Construct a new NeuralPlanner instance
     * 
     * @note Initializes the planner with default parameters. The planner
     *       requires initialization with a brain reference before it can
     *       effectively plan actions using learned forward models.
     */
    NeuralPlanner();
    
    /**
     * @brief Destroy the NeuralPlanner instance
     * 
     * @note Ensures proper cleanup of learned forward models and planning state.
     */
    ~NeuralPlanner();
    
    /**
     * @brief Initialize the planner with brain reference
     * 
     * @param brain Pointer to the brain instance this planner will work with
     * 
     * @note This method must be called before the planner can effectively
     *       perform action planning. The brain provides access to prediction
     *       systems, internal state, and sensory information needed for
     *       forward modeling and prediction.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Plan the next action given current state and goal
     * 
     * @param currentState Current internal state of the agent
     * @param targetReward Desired reward level (default: 0.5f)
     * @return ActionType The best action to take now
     * 
     * @note This is the primary planning interface method. It uses learned
     *       forward models to evaluate potential actions and select the one
     *       that maximizes expected reward given the current state and goal.
     *       
     * @note The planner considers:
     * - Immediate rewards and delays
     * - Long-term goal alignment
     * - Prediction uncertainty and confidence
     * - Historical action quality and experience
     * 
     * @see evaluateSequence() for detailed sequence evaluation
     * @see updatePlanQuality() for plan adaptation based on experience
     */
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    /**
     * @brief Evaluate a potential action sequence
     * 
     * @param actions Vector of actions to evaluate
     * @param startState Starting state for the action sequence
     * @return PlanningCandidate Complete evaluation including expected reward and confidence
     * 
     * @note This method performs forward simulation of the action sequence
     *       using learned prediction models. It returns a PlanningCandidate
     *       containing the full predicted sequence, expected reward, and
     *       prediction confidence.
     *       
     * @note Evaluation involves:
     * - Forward simulation using learned models
     * - Reward prediction for each action
     * - Confidence assessment based on model uncertainty
     * - Integration with current goals and state
     */
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                      const std::vector<float>& startState);
    
    /**
     * @brief Get the number of steps to look ahead for planning
     * 
     * @return size_t Current planning depth (number of steps to consider)
     * 
     * @note Planning depth controls how far ahead the planner looks when
     *       evaluating action sequences. Higher depths enable longer-term
     *       planning but increase computational complexity and uncertainty.
     */
    size_t getPlanningDepth() const { return planningDepth_; }
    
    /**
     * @brief Set the number of steps to look ahead for planning
     * 
     * @param depth Number of steps to look ahead (must be >= 0)
     * 
     * @note Adjusts the planner's temporal horizon. Larger values enable
     *       more sophisticated long-term planning but may reduce prediction
     *       accuracy due to increased uncertainty over longer time periods.
     */
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    
    /**
     * @brief Get the current planning confidence
     * 
     * @return float Current planning confidence (0.0 to 1.0)
     * 
     * @note Planning confidence represents the uncertainty in predictions
     *       and models used for action selection. Higher confidence indicates
     *       more reliable predictions and stronger learned forward models.
     */
    float getPlanningConfidence() const { return planningConfidence_; }
    
    /**
     * @brief Update plans based on actual outcome
     * 
     * @param plannedActions Actions that were originally planned
     * @param actualActions Actions that were actually taken
     * @param actualReward Actual reward received from the outcome
     * 
     * @note This method adapts the planner's internal models based on the
     *       difference between planned and actual outcomes. It updates:
     *       - Prediction accuracy
     *       - Action quality assessments
     *       - Forward model parameters
     *       - Planning confidence
     */
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    /**
     * @brief Clear the planning cache
     * 
     * @note Resets the planner's internal state, including learned forward
     *       models, action quality assessments, and planning confidence.
     *       This forces the planner to relearn from experience.
     */
    void clearCache();
    
    /**
     * @brief Set the quality of an action based on experience
     * 
     * @param action The action to rate
     * @param quality Quality rating (0.0 to 1.0, where higher is better)
     * 
     * @note This method allows the planner to learn from experience by
     *       recording which actions lead to good versus bad outcomes. These
     *       quality ratings are used during action selection to prefer
     *       historically successful actions.
     */
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }
    
    /**
     * @brief Get the current goal state
     * 
     * @return std::vector<float> Current goal state
     * 
     * @note The goal state represents the desired internal state the agent
     *       is working toward. It guides action selection and planning by
     *       providing a target for prediction and evaluation.
     */
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    /**
     * @brief Set the current goal state
     * 
     * @param goal New goal state
     * 
     * @note The goal state guides all planning and action selection.
     *       Changing the goal dynamically allows the agent to adapt to
     *       changing environmental conditions or task requirements.
     */
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    /**
     * @brief Check if recent planning has been successful
     * 
     * @return bool true if recent plans were successful, false otherwise
     * 
     * @note This method uses the planner's recent success history to assess
     *       whether the current planning strategies are effective. It's used
     *       for:
     *       - Meta-learning and strategy adaptation
     *       - Monitoring planning performance
     *       - Adjusting planning parameters based on success rates
     */
    bool wasRecentPlanSuccessful() const;
    
private:
    // -------------------------------------------------------------------------
    // PRIVATE METHODS (Internal Planning Operations)
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate possible action sequences for planning
     * 
     * @param depth Number of steps to plan ahead
     * @return std::vector<std::vector<ActionType>> All possible action sequences
     * 
     * @note This method generates all possible action sequences up to the
     *       specified depth. In a complete implementation, this would consider
     *       action availability, constraints, and action quality ratings.
     */
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);
    
    /**
     * @brief Evaluate a single action from a given state
     * 
     * @param action The action to evaluate
     * @param state The starting state
     * @return float Predicted reward for taking this action
     * 
     * @note This method uses learned prediction models to estimate the
     *       expected reward of taking a specific action from a given state.
     *       It combines immediate and delayed rewards with prediction confidence.
     */
    float evaluateAction(ActionType action, const std::vector<float>& state);
    
    // -------------------------------------------------------------------------
    // PRIVATE DATA MEMBERS (Planning State)
    // -------------------------------------------------------------------------
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation for PIMPL pattern
    
    Brain* brain_;                ///< Pointer to the brain this planner works with
    size_t planningDepth_;        ///< Number of steps to look ahead for planning
    float planningConfidence_;    ///< Current confidence in planning predictions
    
    // Current goal state for planning
    std::vector<float> currentGoal_;  ///< Desired internal state the agent is working toward
    
    // Action quality ratings from experience
    std::vector<float> actionQuality_;  ///< Quality ratings for each action type
    
    // Recent planning success history
    std::deque<bool> recentPlanSuccess_;  ///< History of recent planning success/failure
};

/**
 * @brief SelfModel: Represents the agent's internal model of itself
 * 
 * This class implements the agent's self-model, which represents its internal
 * understanding of its own body schema, capabilities, and the consequences of
 * its actions. Unlike consciousness, the self-model is a practical computational
 * representation used for planning, control, and learning.
 * 
 * Key mechanisms:
 * - Learns body schema: Maps actions to sensory state changes
 * - Predicts consequences: Uses forward models to predict action outcomes
 * - Distinguishes self from external: Identifies self-generated vs. external events
 * - Represents capabilities: Models own limits and strengths
 * 
 * Phase 6 Integration: Essential for embodied cognition and action planning,
 * enabling the agent to understand how its actions affect the environment and
 * to plan effectively within its physical and functional constraints.
 * 
 * Self-model architecture:
 * 1. Body schema: Action → sensory consequence mappings
 * 2. Forward model: State → state prediction
 * 3. Self-recognition: Self-generated vs. external event detection
 * 4. Capability assessment: Performance limits and skill levels
 * 5. Adaptive learning: Updating models based on prediction errors
 */
class SelfModel {
public:
    /**
     * @brief Construct a new SelfModel instance
     * 
     * @note Initializes the self-model with empty knowledge. The model requires
     *       initialization with a brain reference before it can effectively
     *       represent the agent's body and capabilities.
     */
    SelfModel();
    
    /**
     * @brief Destroy the SelfModel instance
     * 
     * @note Ensures proper cleanup of learned body schema and action-effect mappings.
     */
    ~SelfModel();
    
    /**
     * @brief Initialize the self-model with brain reference
     * 
     * @param brain Pointer to the brain instance
     * 
     * @note This method must be called before the self-model can effectively
     *       represent the agent's body and capabilities. The brain provides
     *       access to sensory systems, motor systems, and internal state needed
     *       for building and updating the self-model.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record that taking an action caused a specific sensory change
     * 
     * @param action The action that was taken
     * @param beforeState The sensory state before the action
     * @param afterState The sensory state after the action
     * 
     * @note This method builds the agent's body schema by recording the
     *       relationship between actions and their sensory consequences. This
     *       forms the basis for prediction, planning, and self-awareness.
     */
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    /**
     * @brief Predict sensory consequence of an action
     * 
     * @param action The action to predict
     * @param currentState The current sensory state
     * @return std::vector<float> Predicted sensory state after the action
     * 
     * @note This implements the agent's forward model, predicting how actions
     *       will affect its sensory state. This is essential for planning,
     *       action selection, and understanding the consequences of behavior.
     */
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    /**
     * @brief Get confidence in self-model for a given action
     * 
     * @param action The action to evaluate
     * @return float Confidence in prediction (0.0 to 1.0)
     * 
     * @note Confidence reflects the amount of experience with this action
     *       and the consistency of predicted versus actual outcomes. Higher
     *       confidence indicates more reliable prediction models.
     */
    float getSelfModelConfidence(ActionType action) const;
    
    /**
     * @brief Is this change likely caused by self (action) vs external?
     * 
     * @param beforeState Sensory state before the change
     * @param afterState Sensory state after the change
     * @param action Action that was taken (if any)
     * @return float Likelihood that change was self-generated (0.0 to 1.0)
     * 
     * @note This method implements self-other distinction, determining whether
     *       a sensory change was caused by the agent's own actions or by external
     *       events. This is essential for agency, responsibility, and planning.
     */
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                      const std::vector<float>& afterState,
                                      ActionType action) const;
    
    /**
     * @brief Get the body schema (preferred actions in different states)
     * 
     * @param state The current sensory state
     * @return ActionType The preferred action for this state
     * 
     * @note This method returns the action that the agent has learned is
     *       most effective or appropriate for producing desired outcomes in
     *       the given state. It represents the learned policy or control strategy.
     */
    ActionType getPreferredAction(const std::vector<float>& state);
    
    /**
     * @brief Update self-model based on prediction error
     * 
     * @param predicted Predicted sensory state
     * @param actual Actual sensory state
     * @param action Action that was taken
     * 
     * @note This method adapts the self-model when prediction errors occur,
     *       learning from the discrepancy between expected and actual outcomes.
     *       This is the mechanism for self-model refinement and learning.
     */
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);
    
    /**
     * @brief Get current capability level
     * 
     * @return float Capability level (0.0 to 1.0)
     * 
     * @note This represents the agent's current skill level or proficiency
     *       in performing actions. Capability level affects action selection
     *       and planning by limiting which actions are considered feasible.
     */
    float getCapabilityLevel() const { return capabilityLevel_; }
    
    /**
     * @brief Get body awareness (how accurately can predict consequences)
     * 
     * @return float Body awareness (0.0 to 1.0)
     * 
     * @note This measures how accurately the agent's self-model predicts
     *       the consequences of its actions. High awareness indicates good
     *       understanding of cause-effect relationships and effective planning.
     */
    float getBodyAwareness() const;
    
    /**
     * @brief Clear the self-model
     * 
     * @note Resets the self-model to an empty state. This forces the agent to
     *       relearn its body schema and action-effect relationships from
     *       experience.
     */
    void clear();
    
    /**
     * @brief Check if the self-model has been formed
     * 
     * @return bool true if self-model contains learned action-effect mappings
     * 
     * @note Returns true when the agent has recorded enough experience to
     *       form a functional self-model capable of predicting action outcomes
     *       and guiding behavior.
     */
    bool hasSelfModel() const { return !actionEffects_.empty(); }
    
private:
    // -------------------------------------------------------------------------
    // PRIVATE METHODS (Internal Self-Model Operations)
    // -------------------------------------------------------------------------
    
    /**
     * @brief Find best matching previous experience
     * 
     * @param action The action to find matches for
     * @param beforeState The sensory state before the action
     * @return std::vector<float> Best matching afterState from experience
     * 
     * @note This method searches through recorded action-effect mappings
     *       to find the experience most similar to the current situation.
     *       It uses the similarity of beforeState to find relevant matches.
     */
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;
    
    // -------------------------------------------------------------------------
    // PRIVATE DATA MEMBERS (Self-Model State)
    // -------------------------------------------------------------------------
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation for PIMPL pattern
    
    Brain* brain_;                ///< Pointer to the brain this self-model works with
    float capabilityLevel_;       ///< Current capability level (0.0 to 1.0)
    
    // Action effect mappings: ActionType -> vector of (beforeState, afterState, confidence, count)
    struct ActionEffect {
        std::vector<float> beforeState;      ///< Sensory state before the action
        std::vector<float> afterState;       ///< Sensory state after the action
        float confidence;                    ///< Confidence in this mapping
        size_t observationCount;             ///< Number of times this mapping was observed
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;  ///< Action-effect mappings indexed by action type
};

/**
 * @brief SocialLearning: Enables learning from observing other agents
 * 
 * This class implements the agent's ability to learn from observing other
 * agents' actions and their consequences. Unlike language-based social learning,
 * this system focuses on simple action-effect learning from observation, enabling
 * imitation, pattern recognition, and the development of basic communication signals.
 * 
 * Key mechanisms:
 * - Observes other agent's actions and their effects
 * - Learns that other agents cause predictable changes
 * - Can imitate observed actions
 * - Develops simple communication signals
 * 
 * Phase 6 Integration: Essential for social learning, cooperation, and the
 * development of shared understanding in multi-agent environments.
 * 
 * Social learning architecture:
 * 1. Observation recording: Store observed actions and outcomes
 * 2. Pattern extraction: Identify consistent action-effect relationships
 * 3. Imitation capability: Select actions based on observed success
 * 4. Communication development: Learn and detect signaling patterns
 * 5. Knowledge integration: Apply learned patterns to own behavior
 */
class SocialLearning {
public:
    /**
     * @brief Construct a new SocialLearning instance
     * 
     * @note Initializes the social learning system with empty knowledge. The system
     *       requires initialization with a brain reference before it can effectively
     *       observe and learn from other agents.
     */
    SocialLearning();
    
    /**
     * @brief Destroy the SocialLearning instance
     * 
     * @note Ensures proper cleanup of observed action-effect mappings and learned signals.
     */
    ~SocialLearning();
    
    /**
     * @brief Initialize the social learning system with brain reference
     * 
     * @param brain Pointer to the brain instance
     * 
     * @note This method must be called before the social learning system can
     *       effectively observe and learn from other agents. The brain provides
     *       access to sensory systems and internal state needed for observation.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record observation of another agent's action and effect
     * 
     * @param observedAction The action observed from another agent
     * @param observerState The observer's sensory state
     * @param resultingState The resulting sensory state
     * 
     * @note This method records observations of other agents' behavior,
     *       building a knowledge base of action-effect relationships from the
     *       perspective of an external observer. This enables learning from
     *       others' experiences.
     */
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    /**
     * @brief Check if the agent can imitate a specific action
     * 
     * @param observedAction The action to consider imitating
     * @return bool true if the action can be imitated, false otherwise
     * 
     * @note This determines whether the agent has sufficient experience or
     *       capability to successfully execute the observed action. Imitation
     *       capability depends on:
     *       - Having recorded successful executions of this action
     *       - Current capability level allowing the action
     *       - Body schema compatibility with the action
     */
    bool canImitate(ActionType observedAction) const;
    
    /**
     * @brief Get the best action to imitate given current state
     * 
     * @param currentState The current sensory state
     * @return ActionType The best action to imitate
     * 
     * @note This method selects the action from observed actions that is
     *       most appropriate or beneficial given the current state. It uses
     *       learned action-effect relationships and the agent's current capabilities
     *       to determine the optimal imitation choice.
     */
    ActionType getImitationAction(const std::vector<float>& currentState);
    
    /**
     * @brief Learn simple communication signal from another agent
     * 
     * @param signalPattern The neural pattern that predicts reward from other agent
     * @param signalReward The reward associated with this signal pattern
     * 
     * @note This method enables the agent to learn communication signals from
     *       other agents. A signal is a neural pattern that reliably predicts
     *       reward when observed from other agents, allowing for the development
     *       of basic social communication.
     */
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);
    
    /**
     * @brief Detect if another agent is signaling
     * 
     * @param neuralPattern The neural pattern to check for signaling
     * @return bool true if this pattern represents a learned signal, false otherwise
     * 
     * @note This method checks whether an observed neural pattern matches
     *       any learned communication signals. It's used to detect when
     *       other agents are intentionally communicating with the observing agent.
     */
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    /**
     * @brief Get the learned signal pattern
     * 
     * @return std::vector<float> The learned signal pattern
     * 
     * @note Returns the neural pattern that was learned as a communication
     *       signal from other agents. This pattern can be used to:
     *       - Detect when other agents are signaling
     *       - Understand the meaning of social signals
     *       - Adapt behavior in response to social cues
     */
    std::vector<float> getSignalPattern() const;
    
    /**
     * @brief Get the signal meaning (associated reward)
     * 
     * @return float The reward value associated with the learned signal
     * 
     * @note This returns the reward value that was learned to be associated
     *       with the communication signal. This helps the agent understand
     *       the motivational value of social signals.
     */
    float getSignalMeaning() const;
    
    /**
     * @brief Update social knowledge based on interactions
     * 
     * @param interactionReward The reward received from the interaction
     * 
     * @note This method updates the agent's social knowledge based on direct
     *       interactions with other agents. It refines learned action-effect
     *       relationships and updates social signal interpretations based
     *       on actual interaction outcomes.
     */
    void updateSocialKnowledge(float interactionReward);
    
    /**
     * @brief Clear social learning
     * 
     * @note Resets the social learning system to an empty state. This forces
     *       the agent to relearn from observation and direct interaction with
     *       other agents.
     */
    void clear();
    
    /**
     * @brief Check if the agent has learned from others
     * 
     * @return bool true if the agent has recorded observations or interactions
     * 
     * @note Returns true when the agent has accumulated sufficient experience
     *       from observing other agents or interacting with them. This indicates
     *       that the agent has developed social knowledge and capabilities.
     */
    bool hasSocialKnowledge() const { return observationCount_ > 0; }
    
    /**
     * @brief Get the number of observations recorded
     * 
     * @return size_t Number of observations recorded
     * 
     * @note This provides a measure of how much experience the agent has
     *       gained from observing other agents. It's used to assess the depth
     *       and quality of the agent's social learning.
     */
    size_t getObservationCount() const { return observationCount_; }
    
private:
    // -------------------------------------------------------------------------
    // PRIVATE DATA MEMBERS (Social Learning State)
    // -------------------------------------------------------------------------
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation for PIMPL pattern
    
    Brain* brain_;                ///< Pointer to the brain this social learning works with
    size_t observationCount_;     ///< Number of observations recorded from other agents
    
    // Observed action effects: pair of (action, ObservedEffect)
    struct ObservedEffect {
        std::vector<float> state;           ///< Observer's sensory state
        std::vector<float> resultingState;  ///< Resulting sensory state
        float reward;                       ///< Reward associated with this observation
    };
    
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;  ///< Observed action-effect mappings
    
    // Communication signal: neural pattern and its meaning
    std::vector<float> signalPattern_;  ///< Learned communication signal pattern
    float signalMeaning_;              ///< Reward value associated with the signal
};

} // namespace nlm