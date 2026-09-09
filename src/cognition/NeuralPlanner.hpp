#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// PlanningCandidate: A possible action sequence considered during planning
struct PlanningCandidate {
    std::vector<ActionType> actions;
    std::vector<std::vector<float>> predictedStates;
    float expectedReward;
    float confidence;
    size_t depth;
    
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
};

// NeuralPlanner: Uses learned predictions to plan multi-step actions
// NOT a search engine - planning emerges from predictive neural dynamics
//
// Key mechanisms:
// - Uses action-consequence predictions
// - Evaluates potential futures through simulation
// - Selects actions leading to high-value outcomes
// - Adapts plans based on prediction confidence

class NeuralPlanner {
public:
    /**
     * @brief Construct a new Neural Planner object
     * 
     * Initializes neural planner with default planning depth and confidence.
     * Sets up action quality tracking for 10 action types.
     */
    NeuralPlanner();
    
    /**
     * @brief Destroy the Neural Planner object
     */
    ~NeuralPlanner();

    /**
     * @brief Initialize neural planner with brain reference
     * 
     * @param brain Pointer to brain that provides neural substrate for planning
     */
    void initialize(Brain* brain);

    /**
     * @brief Plan next action given current state and goal
     * 
     * @param currentState Current neural state representation
     * @param targetReward Target reward value for planning (default: 0.5f)
     * @return ActionType Best action to take now based on predictive evaluation
     */
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);

    /**
     * @brief Evaluate a potential action sequence
     * 
     * @param actions Action sequence to evaluate
     * @param startState Starting neural state
     * @return PlanningCandidate Contains sequence, predicted states, reward, and confidence
     */
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                      const std::vector<float>& startState);

    /**
     * @brief Get number of steps to look ahead in planning
     * 
     * @return size_t Planning depth (default: 3)
     */
    size_t getPlanningDepth() const { return planningDepth_; }
    
    /**
     * @brief Set planning depth
     * 
     * @param depth Number of steps to look ahead
     */
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }

    /**
     * @brief Get planning confidence
     * 
     * @return float Confidence in planning predictions (0.0 to 1.0)
     */
    float getPlanningConfidence() const { return planningConfidence_; }

    /**
     * @brief Update plans based on actual outcome
     * 
     * @param plannedActions Actions that were planned
     * @param actualActions Actions that were actually taken
     * @param actualReward Reward received from execution
     */
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);

    /**
     * @brief Clear planning cache
     */
    void clearCache();

    /**
     * @brief Set action quality function (from experience)
     * 
     * @param action Action type
     * @param quality Quality/importance of this action (from -1.0 to 1.0)
     */
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }

    /**
     * @brief Get current goal
     * 
     * @return std::vector<float> Current goal state
     */
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    /**
     * @brief Set current goal
     * 
     * @param goal Target goal state
     */
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }

    /**
     * @brief Check if recent planning has been successful
     * 
     * @return bool True if majority of recent plans were successful
     */
    bool wasRecentPlanSuccessful() const;

private:
    /**
     * @brief Generate possible action sequences
     * 
     * @param depth Planning depth
     * @return std::vector<std::vector<ActionType>> Possible action sequences
     */
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);

    /**
     * @brief Evaluate single action from state
     * 
     * @param action Action to evaluate
     * @param state Current neural state
     * @return float Expected value of taking this action
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
    /**
     * @brief Construct a new Self Model object
     */
    SelfModel();
    
    /**
     * @brief Destroy the Self Model object
     */
    ~SelfModel();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to brain that provides neural substrate
     */
    void initialize(Brain* brain);

    /**
     * @brief Record that taking an action caused a specific sensory change
     * 
     * @param action Action taken
     * @param beforeState Neural state before action
     * @param afterState Neural state after action
     */
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);

    /**
     * @brief Predict sensory consequence of an action
     * 
     * @param action Action to predict for
     * @param currentState Current neural state
     * @return std::vector<float> Predicted state after action
     */
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);

    /**
     * @brief Get confidence in self-model for a given action
     * 
     * @param action Action type
     * @return float Confidence level (0.0 to 1.0)
     */
    float getSelfModelConfidence(ActionType action) const;

    /**
     * @brief Check if change is likely caused by self (action) vs external
     * 
     * @param beforeState State before change
     * @param afterState State after change
     * @param action Action taken
     * @return float Similarity score (0.0 to 1.0)
     */
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       ActionType action) const;

    /**
     * @brief Get the body schema (preferred actions in different states)
     * 
     * @param state Neural state
     * @return ActionType Best action for given state
     */
    ActionType getPreferredAction(const std::vector<float>& state);

    /**
     * @brief Update self-model based on prediction error
     * 
     * @param predicted Predicted state
     * @param actual Actual state
     * @param action Action that caused change
     */
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);

    /**
     * @brief Get current capability level
     * 
     * @return float Capability level (0.0 to 1.0)
     */
    float getCapabilityLevel() const { return capabilityLevel_; }

    /**
     * @brief Get body awareness (how accurately can predict consequences)
     * 
     * @return float Body awareness level (0.0 to 1.0)
     */
    float getBodyAwareness() const;

    /**
     * @brief Clear self-model
     */
    void clear();

    /**
     * @brief Check if self-model has been formed
     * 
     * @return bool True if any action effects have been recorded
     */
    bool hasSelfModel() const { return !actionEffects_.empty(); }

private:
    /**
     * @brief Find best matching previous experience
     * 
     * @param action Action type
     * @param beforeState Neural state before action
     * @return std::vector<float> Predicted state after action
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
    /**
     * @brief Construct a new Social Learning object
     */
    SocialLearning();
    
    /**
     * @brief Destroy the Social Learning object
     */
    ~SocialLearning();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to brain that provides neural substrate
     */
    void initialize(Brain* brain);

    /**
     * @brief Record observation of another agent's action and effect
     * 
     * @param observedAction Action observed from other agent
     * @param observerState Observer's state before observation
     * @param resultingState State after observed action
     */
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);

    /**
     * @brief Check if can imitate this observed action
     * 
     * @param observedAction Action type to check
     * @return bool True if action has been observed previously
     */
    bool canImitate(ActionType observedAction) const;

    /**
     * @brief Get the best action to imitate given current state
     * 
     * @param currentState Observer's current state
     * @return ActionType Best action to imitate
     */
    ActionType getImitationAction(const std::vector<float>& currentState);

    /**
     * @brief Learn simple communication signal from another agent
     * 
     * @param signalPattern Neural pattern that predicts reward
     * @param signalReward Associated reward value
     */
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);

    /**
     * @brief Detect if another agent is signaling
     * 
     * @param neuralPattern Neural pattern to check
     * @return bool True if pattern matches learned signal
     */
    bool detectSignal(const std::vector<float>& neuralPattern) const;

    /**
     * @brief Get learned signal pattern
     * 
     * @return std::vector<float> Learned signal pattern
     */
    std::vector<float> getSignalPattern() const;

    /**
     * @brief Get signal meaning (associated reward)
     * 
     * @return float Signal meaning (reward value)
     */
    float getSignalMeaning() const;

    /**
     * @brief Update social knowledge based on interactions
     * 
     * @param interactionReward Reward from social interaction
     */
    void updateSocialKnowledge(float interactionReward);

    /**
     * @brief Clear social learning
     */
    void clear();

    /**
     * @brief Check if has learned from others
     * 
     * @return bool True if any observations have been recorded
     */
    bool hasSocialKnowledge() const { return observationCount_ > 0; }

    /**
     * @brief Get observation count
     * 
     * @return size_t Number of social observations recorded
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