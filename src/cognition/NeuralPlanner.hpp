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
// - Integrates with working memory for maintaining action plans
// - Adapts strategies across developmental stages
// - Uses curiosity-driven exploration
// - Implements reinforcement learning
// - Uses neuromodulation gating
// - Performs trial-and-error learning
// - Supports transfer learning
// - Implements hierarchical goals
// - Switches adaptive strategies

class NeuralPlanner {
public:
    NeuralPlanner();
    ~NeuralPlanner();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Plan next action given current state and goal
    // Returns the best action to take now
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);

    // Evaluate a potential action sequence
    // Returns expected total reward and confidence
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                      const std::vector<float>& startState);

    // Get number of steps to look ahead
    size_t getPlanningDepth() const { return planningDepth_; }
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }

    // Get planning confidence
    float getPlanningConfidence() const { return planningConfidence_; }

    // Update plans based on actual outcome
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);

    // Clear planning cache
    void clearCache();

    // Set action quality function (from experience)
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }

    // Get current goal
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }

    // Has recent planning been successful?
    bool wasRecentPlanSuccessful() const;

private:
    // Generate possible action sequences
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);

    // Evaluate single action from state
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
    
    // ===== ADVANCED PLANNING STRUCTURES =====
    
    // Hierarchical goal levels (abstract -> concrete)
    struct HierarchicalGoal {
        std::vector<float> goalVector;
        float abstractionLevel;  // 0.0 = concrete, 1.0 = abstract
        size_t priority;
        float expectedReward;
        
        HierarchicalGoal() : abstractionLevel(0.0f), priority(1), expectedReward(0.0f) {}
    };
    
    // Planning context with working memory integration
    struct PlanningContext {
        std::vector<float> currentState;
        std::vector<float> goalState;
        std::vector<float> actionPlan;
        float rewardEstimate;
        float predictionError;
        float curiosityLevel;
        float neuromodulationLevel;
        DevelopmentalStage developmentalStage;
        StrategyType currentStrategy;
        std::vector<StrategyType> alternativeStrategies;
        
        PlanningContext() 
            : rewardEstimate(0.0f), predictionError(0.0f), 
              curiosityLevel(0.0f), neuromodulationLevel(1.0f), 
              developmentalStage(DevelopmentalStage::Initial) {}
    };
    
    // Strategy types for adaptive switching
    enum class StrategyType {
        RandomSearch,
        Greedy,
        TreeSearch,
        MonteCarlo,
        Evolutionary,
        CuriosityDriven,
        Developmental
    };
    
    // Transfer learning memory (task -> plan mapping)
    struct TransferMemory {
        std::vector<std::vector<ActionType>> successfulPlan;
        std::vector<float> resultingState;
        float successProbability;
        std::string taskSignature;
        size_t executionCount;
        
        TransferMemory() : successProbability(0.0f), executionCount(0) {}
    };
    
    // Trial-and-error learning statistics
    struct TrialErrorStats {
        std::vector<float> actionSequence;
        std::vector<float> rewards;
        std::vector<float> predictionErrors;
        float learningRate;
        float explorationRate;
        
        TrialErrorStats() : learningRate(0.1f), explorationRate(0.1f) {}
    };
    
    // ===== ADVANCED PLANNING METHODS =====
    
    // Backward planning: goal to action sequence
    std::vector<ActionType> planBackward(const std::vector<float>& goal,
                                         const std::vector<float>& startState,
                                         const PlanningContext& context);
    
    // Forward planning: action to predicted outcome
    std::vector<float> predictForward(const std::vector<ActionType>& action,
                                       const std::vector<float>& currentState,
                                       const PlanningContext& context) const;
    
    // Integrated planning with working memory
    PlanningCandidate planWithWorkingMemory(const std::vector<float>& currentState,
                                           const PlanningContext& context);
    
    // Prediction error integration for planning
    void incorporatePredictionError(const std::vector<ActionType>& actions,
                                   float predictionError,
                                   const PlanningContext& context);
    
    // Developmental stage-dependent planning
    float getDevelopmentalPlanningGain(DevelopmentalStage stage) const;
    
    // Curiosity-driven exploration
    std::vector<ActionType> generateCuriosityDrivenActions(const std::vector<float>& currentState,
                                                         const PlanningContext& context) const;
    
    // Reinforcement learning planning
    PlanningCandidate planWithRL(const std::vector<float>& currentState,
                                const PlanningContext& context);
    
    // Neuromodulation gating
    float getNeuromodulationGating(NeuromodulatorType type) const;
    void applyNeuromodulationGating(PlanningContext& context) const;
    
    // Trial-and-error learning
    void applyTrialErrorLearning(const std::vector<ActionType>& actions,
                                float reward,
                                const PlanningContext& context);
    
    // Transfer learning
    float evaluateTransferLearning(const std::vector<ActionType>& actions,
                                   const std::vector<float>& startState,
                                   const std::string& taskId) const;
    
    // Hierarchical goal processing
    float evaluateHierarchicalGoal(const HierarchicalGoal& goal,
                                   const std::vector<float>& currentState) const;
    
    // Adaptive strategy selection and switching
    StrategyType selectStrategy(const PlanningContext& context) const;
    void switchStrategy(StrategyType newStrategy, PlanningContext& context);
    void evaluateStrategyPerformance(StrategyType strategy, float reward, PlanningContext& context);
    
    // ===== ADVANCED DATA MEMBERS =====
    
    // Working memory integration
    Brain* brain_;
    
    // Backward planning cache
    std::unordered_map<std::string, std::vector<ActionType>> backwardPlanCache_;
    
    // Forward prediction models
    std::vector<std::vector<float>> forwardPredictionModels_;
    
    // Transfer learning memory
    std::vector<TransferMemory> transferMemory_;
    
    // Trial-and-error learning stats
    TrialErrorStats trialErrorStats_;
    
    // Strategy performance tracking
    std::unordered_map<StrategyType, float> strategyPerformance_;
    std::vector<StrategyType> strategyHistory_;
    
    // Hierarchical goals
    std::vector<HierarchicalGoal> hierarchicalGoals_;
    
    // Neuromodulation gating parameters
    std::unordered_map<NeuromodulatorType, float> neuromodulationGating_;
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