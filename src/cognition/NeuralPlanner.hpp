#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

/**
 * @file NeuralPlanner.hpp
 * 
 * Cognitive planning system for NLM
 * 
 * This file provides the cognitive planning system for NLM, implementing:
 * - Neural planning for action sequences
 * - Self-model and self-awareness
 * - Social learning and observation
 * - Action consequence prediction
 * - Goal-directed behavior
 * 
 * The cognitive planning system in NLM:
 * - **NeuralPlanner**: Plans action sequences using learned predictions
 * - **SelfModel**: Maintains internal model of self and capabilities
 * - **SocialLearning**: Learns from observed actions of other agents
 * - **Integration**: Connects planning with neural dynamics
 * - **Learning**: Updates plans based on experience
 * 
 * Key features:
 * - **Action planning**: Plans multi-step action sequences
 * - **Self-model**: Maintains internal self-representation
 * - **Social learning**: Learns from observed actions
 * - **Goal-directed**: Plans actions based on goals
 * - **Adaptive**: Adapts plans based on experience
 * 
 * @note Critical for cognitive function and behavior
 * @note Used extensively in brain and motor systems
 * @note Essential for goal-directed behavior
 * @see Config.hpp - Configuration options for planning
 * @see Brain.hpp - Integration with brain architecture
 * @see ConceptFormation.hpp - For concept formation
 * @see Action.hpp - For action execution
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @struct PlanningCandidate
 * 
 * A possible action sequence considered during planning.
 * 
 * This structure represents a candidate action sequence with associated
 * predictions and expected outcomes. It is used by the NeuralPlanner to:
 * - Evaluate potential action sequences
 * - Store planning candidates
 * - Compare alternative plans
 * - Make planning decisions
 * 
 * Key features:
 * - **Action sequence**: Sequence of actions to execute
 * - **Predicted states**: Expected sensory states for each action
 * - **Expected reward**: Predicted total reward for the sequence
 * - **Confidence**: Prediction confidence
 * - **Depth**: Planning depth (number of actions)
 * 
 * @note Used by NeuralPlanner for action sequence evaluation
 * @see NeuralPlanner - For planning implementation
 * @see ActionType - For action types
 * 
 * @author NLM Development Team
 */
struct PlanningCandidate {
    /** Sequence of actions to execute */
    std::vector<ActionType> actions;
    
    /** Predicted sensory states for each action */
    std::vector<std::vector<float>> predictedStates;
    
    /** Expected total reward for the action sequence */
    float expectedReward;
    
    /** Prediction confidence (0-1) */
    float confidence;
    
    /** Planning depth (number of actions) */
    size_t depth;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default planning candidate with empty values.
     * 
     * @post Candidate created with default values
     * @post All fields initialized to defaults
     */
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
};

/**
 * @class NeuralPlanner
 * 
 * Neural implementation of action planning and planning.
 * 
 * This class implements neural planning using actual neural dynamics.
 * Instead of using symbolic representations, it stores action sequences
 * as neural patterns and uses neural dynamics to plan and select actions.
 * This provides:
 * - **Action planning**: Plans multi-step action sequences
 * - **Prediction evaluation**: Evaluates action sequences based on predictions
 * - **Goal-directed**: Plans actions based on goals
 * - **Adaptive**: Adapts plans based on experience
 * - **Learning**: Learns better action sequences from experience
 * 
 * NeuralPlanner mechanisms:
 * - **Action sequence generation**: Generates possible action sequences
 * - **Action evaluation**: Evaluates actions based on predictions
 * - **Planning**: Plans sequences based on goals and predictions
 * - **Adaptation**: Adapts plans based on prediction errors
 * - **Confidence**: Maintains prediction confidence
 * 
 * Key characteristics of neural planning in NLM:
 * - **Neural representation**: Sequences stored as neural patterns
 * - **Action planning**: Plans action sequences based on goals
 * - **Prediction evaluation**: Evaluates actions based on predictions
 * - **Adaptation**: Adapts plans based on experience
 * - **Learning**: Learns better action sequences
 * 
 * @note This is the neural implementation of action planning
 * @see Action - For action execution
 * @see SelfModel - For self-representation
 * @see SocialLearning - For social learning
 * @see ConceptFormation.hpp - For concept formation
 * 
 * @author NLM Development Team
 */
class NeuralPlanner {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new neural planner instance with default parameters.
     * The neural planner is initialized with default settings for
     * action sequence planning.
     */
    NeuralPlanner();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neural planner resources and performs any necessary
     * cleanup operations. The neural planner is left in a valid but
     * non-operational state after destruction.
     */
    ~NeuralPlanner();
    
    // Disable copying - PImpl idiom
    NeuralPlanner(const NeuralPlanner&) = delete;
    NeuralPlanner& operator=(const NeuralPlanner&) = delete;
    NeuralPlanner(NeuralPlanner&&) noexcept;
    NeuralPlanner& operator=(NeuralPlanner&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the neural planner with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Plan next action given current state and goal
     * 
     * Plans the next action to take based on the current state and goal.
     * This is the core planning function that:
     * - Evaluates possible actions based on predictions
     * - Selects the best action based on expected reward
     * - Considers prediction confidence
     * - Integrates goal information
     * 
     * @param currentState Current sensory state
     * @param targetReward Target reward for planning
     * @return Best action to take now
     */
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    /**
     * @brief Evaluate a potential action sequence
     * 
     * Evaluates a potential action sequence to determine its expected
     * total reward and confidence. This is essential for:
     * - Action sequence evaluation
     * - Planning alternative actions
     * - Comparing action sequences
     * - Making planning decisions
     */
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                       const std::vector<float>& startState);
    
    /**
     * @brief Get number of steps to look ahead
     * 
     * Returns the current planning depth, which determines how many steps
     * into the future to plan. This is used for:
     * - Configuring planning depth
     * - Controlling computational complexity
     * - Tuning planning behavior
     * - Testing planning dynamics
     */
    size_t getPlanningDepth() const { return planningDepth_; }
    
    /**
     * @brief Set planning depth
     * 
     * Sets the planning depth, which determines how many steps into the
     * future to plan. This is used for:
     * - Configuring planning capabilities
     * - Experimental control
     * - Tuning planning parameters
     * - Testing planning dynamics
     */
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    
    /**
     * @brief Get planning confidence
     * 
     * Returns the current planning confidence, which represents the
     * certainty of planning predictions. Higher confidence indicates
     * more certain predictions.
     */
    float getPlanningConfidence() const { return planningConfidence_; }
    
    /**
     * @brief Update plans based on actual outcome
     * 
     * Updates planning quality based on actual outcomes. This is used for:
     * - Learning from actual outcomes
     * - Updating action quality
     * - Improving planning accuracy
     * - Adaptive planning
     */
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    /**
     * @brief Clear planning cache
     * 
     * Clears the planning cache, removing all stored planning information.
     * This is useful for:
     * - Resetting planning system
     * - Managing planning resources
     * - Error recovery
     * - Planning management
     */
    void clearCache();
    
    /**
     * @brief Set action quality function (from experience)
     * 
     * Sets the action quality function based on experience. This is used
     * for:
     * - Storing action quality from experience
     * - Evaluating action sequences
     * - Action quality assessment
     * - Learning from experience
     */
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }
    
    /**
     * @brief Get current goal
     * 
     * Returns the current goal state, which represents the target state
     * for planning. This is used for:
     * - Goal-directed planning
     * - Target-based planning
     * - Planning based on goals
     * - Goal management
     */
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    /**
     * @brief Set current goal
     * 
     * Sets the current goal state, which represents the target state for
     * planning. This is used for:
     * - Setting planning goals
     * - Goal-directed planning
     * - Target-based planning
     * - Goal management
     */
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    /**
     * @brief Has recent planning been successful?
     * 
     * Determines whether recent planning has been successful. This is used
     * for:
     * - Evaluating planning performance
     * - Learning from planning outcomes
     * - Adjusting planning strategies
     * - Planning assessment
     */
    bool wasRecentPlanSuccessful() const;
    
private:
    /**
     * @brief Generate possible action sequences
     * 
     * Generates possible action sequences based on planning depth.
     * This is used for:
     * - Action sequence generation
     * - Planning alternatives
     * - Action sequence exploration
     * - Planning diversity
     */
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);
    
    /**
     * @brief Evaluate single action from state
     * 
     * Evaluates a single action from a given state. This is used for:
     * - Action evaluation
     * - Action selection
     * - Planning evaluation
     * - Action assessment
     */
    float evaluateAction(ActionType action, const std::vector<float>& state);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t planningDepth_;
    float planningConfidence_;
    
    // Current goal state
    std::vector<float> currentGoal_;
    
    // Action quality from experience
    std::vector<float> actionQuality_;
    
    // Recent plan success history
    std::deque<bool> recentPlanSuccess_;
};

/**
 * @class SelfModel
 * 
 * Neural implementation of the agent's internal model of itself.
 * 
 * This class implements neural self-modeling, maintaining an internal
 * representation of the agent's capabilities, limitations, and
 * understanding of its own effects. This is essential for:
 * - Self-awareness and self-monitoring
 * - Action planning and prediction
 * - Learning from experience
 * - Distinguishing self from external events
 * - Representing own capabilities and limitations
 * 
 * Self-model mechanisms in NLM:
 * - **Body schema**: Learns how actions affect sensory state
 * - **Prediction**: Predicts consequences of own actions
 * - **Self-distinction**: Distinguishes self from external events
 * - **Capability representation**: Represents own capabilities
 * - **Learning**: Learns from own actions and their consequences
 * 
 * Key characteristics of self-model in NLM:
 * - **Internal representation**: Maintains internal self-representation
 * - **Action prediction**: Predicts consequences of own actions
 * - **Self-distinction**: Distinguishes self from external events
 * - **Capability learning**: Learns own capabilities and limitations
 * - **Adaptation**: Adapts self-model based on experience
 * 
 * @note Essential for self-awareness and self-monitoring
 * @note Used extensively in action planning and prediction
 * @note Critical for understanding own capabilities
 * @see NeuralPlanner - For planning
 * @see SocialLearning - For social interaction
 * @see ConceptFormation.hpp - For concept formation
 * 
 * @author NLM Development Team
 */
class SelfModel {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new self-model instance with default parameters.
     * The self-model is initialized with default settings for
     * self-representation and capability learning.
     */
    SelfModel();
    
    /**
     * @brief Destructor
     * 
     * Cleans up self-model resources and performs any necessary
     * cleanup operations. The self-model is left in a valid but
     * non-operational state after destruction.
     */
    ~SelfModel();
    
    // Disable copying - PImpl idiom
    SelfModel(const SelfModel&) = delete;
    SelfModel& operator=(const SelfModel&) = delete;
    SelfModel(SelfModel&&) noexcept;
    SelfModel& operator=(SelfModel&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the self-model with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record that taking an action caused a specific sensory change
     * 
     * Records that taking a specific action caused a specific sensory
     * change. This is essential for:
     * - Building body schema
     * - Learning action effects
     * - Understanding own capabilities
     * - Self-model learning
     */
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    /**
     * @brief Predict sensory consequence of an action
     * 
     * Predicts the sensory consequence of a specific action. This is the
     * "forward model" of the self that predicts:
     * - Action consequences
     * - Expected sensory states
     * - Action effectiveness
     * - Self-capability
     */
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    /**
     * @brief Get confidence in self-model for a given action
     * 
     * Returns the confidence in the self-model for a specific action.
     * This is essential for:
     * - Action planning
     * - Self-awareness
     * - Confidence assessment
     * - Self-model evaluation
     */
    float getSelfModelConfidence(ActionType action) const;
    
    /**
     * @brief Is this change likely caused by self (action) vs external?
     * 
     * Computes the self-generated likeness for a specific action and its
     * consequences. This is used to distinguish self from external events
     * and understand:
     * - Self-action effects
     * - Self-generated vs external
     * - Self-awareness
     * - Self-distinction
     */
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                     const std::vector<float>& afterState,
                                     ActionType action) const;
    
    /**
     * @brief Get the body schema (preferred actions in different states)
     * 
     * Returns the body schema, which maps states to preferred actions.
     * This is used for:
     * - Action selection
     * - Self-representation
     * - Capability understanding
     * - Action planning
     */
    ActionType getPreferredAction(const std::vector<float>& state);
    
    /**
     * @brief Update self-model based on prediction error
     * 
     * Updates the self-model based on prediction errors. This is used for:
     * - Learning from prediction errors
     * - Updating self-model
     * - Self-improvement
     * - Self-learning
     */
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);
    
    /**
     * @brief Get current capability level
     * 
     * Returns the current capability level, which represents the agent's
     * overall capability and proficiency. This is used for:
     * - Capability assessment
     * - Skill evaluation
     * - Capability improvement
     * - Self-assessment
     */
    float getCapabilityLevel() const { return capabilityLevel_; }
    
    /**
     * @brief Get body awareness
     * 
     * Returns the body awareness, which represents how accurately the
     * agent can predict its own actions and their consequences. This is
     * used for:
     * - Self-awareness
     * - Action prediction
     * - Capability assessment
     * - Self-model evaluation
     */
    float getBodyAwareness() const;
    
    /**
     * @brief Clear self-model
     * 
     * Clears the self-model, removing all stored self-representations.
     * This is used for:
     * - Resetting self-model
     * - Managing self-model resources
     * - Error recovery
     * - Self-model management
     */
    void clear();
    
    /**
     * @brief Has self-model formed?
     * 
     * Determines whether the self-model has been formed. This is used for:
     * - Self-model assessment
     * - Self-awareness
     * - Self-model evaluation
     * - Self-model management
     */
    bool hasSelfModel() const { return !actionEffects_.empty(); }
    
private:
    /**
     * @brief Find best matching previous experience
     * 
     * Finds the best matching previous experience for a given action and
     * state. This is used for:
     * - Pattern completion
     * - Pattern matching
     * - Memory-based learning
     * - Learning from experience
     */
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

/**
 * @class SocialLearning
 * 
 * Neural implementation of social learning and observation.
 * 
 * This class implements neural social learning, enabling the agent to
 * learn from observing the actions of other agents. It is essential for:
 * - Learning from observation
 * - Imitation learning
 * - Social coordination
 * - Communication
 * - Cooperative behavior
 * 
 * Social learning mechanisms in NLM:
 * - **Observation learning**: Learns from observed actions
 * - **Imitation**: Imitates observed actions
 * - **Communication**: Learns simple communication signals
 * - **Social coordination**: Coordinates with other agents
 * - **Cooperation**: Enables cooperative behavior
 * 
 * Key characteristics of social learning in NLM:
 * - **Observation learning**: Learns from observed actions
 * - **Imitation**: Imitates observed actions
 * - **Communication**: Learns simple communication signals
 * - **Coordination**: Coordinates with other agents
 * - **Cooperation**: Enables cooperative behavior
 * 
 * @note Essential for social learning and observation
 * @note Used extensively in social interaction
 * @note Critical for social coordination
 * @see NeuralPlanner - For planning
 * @see SelfModel - For self-representation
 * @see SocialLearning - For social interaction
 * 
 * @author NLM Development Team
 */
class SocialLearning {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new social learning instance with default parameters.
     * The social learning is initialized with default settings for
     * learning from observation.
     */
    SocialLearning();
    
    /**
     * @brief Destructor
     * 
     * Cleans up social learning resources and performs any necessary
     * cleanup operations. The social learning is left in a valid but
     * non-operational state after destruction.
     */
    ~SocialLearning();
    
    // Disable copying - PImpl idiom
    SocialLearning(const SocialLearning&) = delete;
    SocialLearning& operator=(const SocialLearning&) = delete;
    SocialLearning(SocialLearning&&) noexcept;
    SocialLearning& operator=(SocialLearning&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the social learning with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record observation of another agent's action
     * 
     * Records observation of another agent's action and its consequences.
     * This is essential for:
     * - Learning from observation
     * - Imitation learning
     * - Social coordination
     * - Communication
     * - Social learning
     */
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    /**
     * @brief Can I imitate this observed action?
     * 
     * Determines whether the observed action can be imitated. This is used for:
     * - Imitation capability
     * - Action selection
     * - Social learning
     * - Imitation assessment
     */
    bool canImitate(ActionType observedAction) const;
    
    /**
     * @brief Get the best action to imitate given current state
     * 
     * Returns the best action to imitate given the current state.
     * This is used for:
     * - Imitation selection
     * - Action imitation
     * - Social learning
     * - Imitation-based behavior
     */
    ActionType getImitationAction(const std::vector<float>& currentState);
    
    /**
     * @brief Learn simple communication signal from another agent
     * 
     * Learns a simple communication signal from another agent. This is used
     * for:
     * - Learning from communication
     * - Signal recognition
     * - Communication learning
     * - Social interaction
     */
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);
    
    /**
     * @brief Detect if another agent is signaling
     * 
     * Detects whether another agent is signaling based on the observed neural
     * pattern. This is used for:
     * - Signal detection
     * - Communication recognition
     * - Social interaction
     * - Signal interpretation
     */
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    /**
     * @brief Get learned signal pattern
     * 
     * Returns the learned signal pattern. This is used for:
     * - Signal recognition
     * - Communication
     * - Signal retrieval
     * - Signal interpretation
     */
    std::vector<float> getSignalPattern() const;
    
    /**
     * @brief Get signal meaning (associated reward)
     * 
     * Returns the signal meaning (associated reward). This is used for:
     * - Signal interpretation
     * - Reward association
     * - Communication
     * - Social interaction
     */
    float getSignalMeaning() const;
    
    /**
     * @brief Update social knowledge based on interactions
     * 
     * Updates social knowledge based on interactions. This is used for:
     * - Social learning
     * - Interaction learning
     * - Social adaptation
     * - Social coordination
     */
    void updateSocialKnowledge(float interactionReward);
    
    /**
     * @brief Clear social learning
     * 
     * Clears social learning, removing all learned social knowledge.
     * This is used for:
     * - Resetting social learning
     * - Managing social learning resources
     * - Error recovery
     * - Social learning management
     */
    void clear();
    
    /**
     * @brief Has learned from others?
     * 
     * Determines whether social learning has occurred. This is used for:
     * - Social learning assessment
     * - Social learning evaluation
     * - Social learning management
     * - Social learning tracking
     */
    bool hasSocialKnowledge() const { return observationCount_ > 0; }
    
    /**
     * @brief Get observation count
     * 
     * Returns the number of observations. This is used for:
     * - Observation tracking
     * - Social learning assessment
     * - Social learning evaluation
     * - Social learning management
     */
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
