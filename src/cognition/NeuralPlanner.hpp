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
    NeuralPlanner();
    ~NeuralPlanner();

    // Initialize with brain reference and integrated prediction systems
    void initialize(Brain* brain, PredictionSystem* predictionSystem, 
                    ConceptFormation* conceptFormation);

    // Plan next action given current state and goal using neural dynamics
    // Returns the best action to take now based on predictive inference
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f,
                         SimulationStep currentTime = 0.0f);

    // Evaluate a potential action sequence using forward model and Bayesian inference
    // Returns expected total reward and confidence with uncertainty quantification
    PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                       const std::vector<float>& startState,
                                       SimulationStep currentTime);

    // Get number of steps to look ahead (planning horizon)
    size_t getPlanningDepth() const { return planningDepth_; }
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }

    // Get planning confidence (0-1)
    float getPlanningConfidence() const { return planningConfidence_; }

    // Update plans based on actual outcome with prediction error integration
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward,
                          SimulationStep currentTime);

    // Clear planning cache and reset planning dynamics
    void clearCache();

    // Set action quality function from experience (action-value function)
    void setActionQuality(ActionType action, float quality) {
        actionQuality_[static_cast<size_t>(action)] = quality;
    }

    // Get current goal state
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }

    // Has recent planning been successful?
    bool wasRecentPlanSuccessful() const;

    // Enable/disable hierarchical planning
    void enableHierarchicalPlanning(bool enable) { hierarchicalPlanningEnabled_ = enable; }

    // Get planning statistics
    struct PlanningStats {
        float meanExpectedReward;
        float meanPlanningConfidence;
        size_t totalPlansEvaluated;
        size_t successfulPlans;  // Plans that led to positive outcomes
        float explorationRate;
        float predictionError;
    };

    PlanningStats getStats() const;

    // Update planner based on neural prediction errors
    void updateFromPredictionErrors(const std::vector<float>& predictionErrors,
                                   SimulationStep currentTime);

    // Integrate with neuromodulation systems for adaptive planning
    void setPlanningErrorSignal(PredictionErrorSignal* signal) { errorSignal_ = signal; }

private:
    // Generate possible action sequences using neural dynamics
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth,
                                                                const std::vector<float>& state);

    // Evaluate single action using forward model and learned value function
    float evaluateAction(ActionType action, const std::vector<float>& state,
                        SimulationStep currentTime);

    // Learn from successful planning experiences
    void learnFromPlan(const PlanningCandidate& plan, float actualReward,
                      const std::vector<ActionType>& actualActions,
                      SimulationStep currentTime);

    // Update planning confidence based on prediction accuracy
    void updatePlanningConfidence(float predictionError);

    // Compute planning bonus for exploration (epsilon-greedy with novelty)
    float computeExplorationBonus(const std::vector<float>& state,
                                  const std::vector<size_t>& relevantConcepts);

    // Bayesian planning: maintain distribution over possible action values
    struct ActionValueDistribution {
        std::vector<float> meanValues;      // Mean expected reward per action
        std::vector<float> variances;       // Uncertainty in action values
        std::vector<size_t> observationCounts; // How many times action was tried
        
        ActionValueDistribution(size_t numActions) 
            : meanValues(numActions, 0.0f), variances(numActions, 1.0f),
              observationCounts(numActions, 0) {}
    };

    // Update action value distributions based on experience
    void updateActionValueDistribution(ActionType action, float reward,
                                       SimulationStep currentTime);

    // Monte Carlo tree search for deep planning (simplified for efficiency)
    ActionType monteCarloTreeSearch(const std::vector<float>& state,
                                   size_t maxIterations,
                                   SimulationStep currentTime);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    PredictionSystem* predictionSystem_;
    ConceptFormation* conceptFormation_;
    PredictionErrorSignal* errorSignal_;
    
    size_t planningDepth_;
    float planningConfidence_;
    
    // Current goal state for planning
    std::vector<float> currentGoal_;
    
    // Action quality/value function from experience
    std::vector<float> actionQuality_;
    
    // Action value distributions for Bayesian planning
    ActionValueDistribution actionValueDistributions_;
    
    // Planning statistics and learning state
    std::deque<float> planningRewards_;
    std::deque<float> planningErrors_;
    SimulationStep lastPlanningUpdate_;
    
    // Hierarchical planning support
    bool hierarchicalPlanningEnabled_;
    
    // Exploration parameters
    float explorationRate_;
    float epsilonGreedyThreshold_;
};

// Advanced neural planning with hierarchical goal structures
class HierarchicalNeuralPlanner {
public:
    HierarchicalNeuralPlanner();
    ~HierarchicalNeuralPlanner();

    // Initialize with brain reference and integrated cognitive systems
    void initialize(Brain* brain, PredictionSystem* predictionSystem,
                    ConceptFormation* conceptFormation,
                    NeuralPlanner* lowLevelPlanner);

    // Plan at multiple levels of abstraction
    std::vector<ActionType> hierarchicalPlan(const std::vector<float>& currentState,
                                             SimulationStep currentTime);

    // Update hierarchical plan based on performance feedback
    void updateHierarchicalPlan(const std::vector<ActionType>& actions,
                               float reward,
                               SimulationStep currentTime);

    // Get current plan at different abstraction levels
    std::vector<std::vector<ActionType>> getCurrentHierarchicalPlan() const;

    // Clear hierarchical planning state
    void clear();

    // Set planning parameters
    void setMaxHierarchyDepth(size_t depth) { maxHierarchyDepth_ = depth; }
    void setAbstractionLevel(float level) { abstractionLevel_ = level; }

private:
    // Generate plans at different hierarchy levels
    void generateHierarchicalPlans(const std::vector<float>& state,
                                   SimulationStep currentTime);

    // Evaluate plan across hierarchy levels
    float evaluateHierarchicalPlan(const std::vector<std::vector<ActionType>>& plan,
                                   const std::vector<float>& state);

    // Integrate low-level and high-level planning
    void integrateHierarchicalLevels();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    PredictionSystem* predictionSystem_;
    ConceptFormation* conceptFormation_;
    NeuralPlanner* lowLevelPlanner_;
    
    // Hierarchy levels
    std::vector<std::vector<ActionType>> hierarchicalPlan_;
    size_t maxHierarchyDepth_;
    float abstractionLevel_;
    
    // Planning statistics
    std::vector<float> levelRewards_;
    SimulationStep lastUpdate_;
};

// Reward prediction learning for planning
class RewardPredictionLearning {
public:
    RewardPredictionLearning();
    ~RewardPredictionLearning();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn reward predictions from experience
    void learnRewardPrediction(const std::vector<float>& state,
                               ActionType action,
                               const std::vector<float>& nextState,
                               float reward,
                               SimulationStep currentTime);

    // Predict reward for given state-action pair
    float predictReward(const std::vector<float>& state,
                        ActionType action,
                        const std::vector<float>& nextState);

    // Update reward predictions based on actual outcomes
    void updateFromOutcome(const std::vector<float>& state,
                          ActionType action,
                          float actualReward,
                          SimulationStep currentTime);

    // Get reward prediction uncertainty
    float getRewardPredictionUncertainty(const std::vector<float>& state,
                                         ActionType action) const;

    // Clear reward prediction memory
    void clear();

    // Enable reward prediction learning
    void enableRewardLearning(bool enable) { rewardLearningEnabled_ = enable; }

private:
    // Neural reward prediction model
    struct RewardPredictor {
        std::vector<float> weights;
        std::vector<float> biases;
        std::vector<float> activationHistory;
        
        RewardPredictor() : weights(20, 0.0f), biases(20, 0.0f) {}
    };

    // Learn reward prediction from neural patterns
    void learnFromNeuralPattern(const std::vector<float>& pattern,
                               float reward,
                               SimulationStep currentTime);

    // Compute reward prediction using neural dynamics
    float computeRewardPrediction(const std::vector<float>& pattern) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<RewardPredictor> rewardPredictors_;
    bool rewardLearningEnabled_;
    
    // Reward prediction statistics
    std::deque<std::tuple<std::vector<float>, ActionType, float>> rewardHistory_;
    SimulationStep lastUpdate_;
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