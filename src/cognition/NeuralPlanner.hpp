#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Forward declarations
class SelfModel;
class SocialLearning;

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
    
    // Get confidence in planning
    float getPlanningConfidence() const { return planningConfidence_; }
    
    // Get action quality for specific action
    float getActionQuality(ActionType action) const {
        size_t idx = static_cast<size_t>(action);
        return (idx < actionQuality_.size()) ? actionQuality_[idx] : 0.0f;
    }
    
    // Set action quality
    void setActionQuality(ActionType action, float quality) {
        size_t idx = static_cast<size_t>(action);
        if (idx < actionQuality_.size()) {
            actionQuality_[idx] = quality;
        }
    }
    
    // Update plan quality based on actual experience
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    // Check if recent plans have been successful
    bool wasRecentPlanSuccessful() const;
    
    // Get SelfModel reference
    SelfModel* getSelfModel() { return selfModel_.get(); }
    const SelfModel* getSelfModel() const { return selfModel_.get(); }
    
    // Get SocialLearning reference
    SocialLearning* getSocialLearning() { return socialLearning_.get(); }
    const SocialLearning* getSocialLearning() const { return socialLearning_.get(); }
    
    // Clear any internal caches
    void clearCache();
    
    // Set goal for planning
    void setGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    const std::vector<float>& getGoal() const { return currentGoal_; }
    
    // Clear goal
    void clearGoal() { currentGoal_.clear(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::unique_ptr<SelfModel> selfModel_;
    std::unique_ptr<SocialLearning> socialLearning_;
    std::vector<ActionType> actionQuality_;
    size_t planningDepth_;
    float planningConfidence_;
    std::deque<bool> recentPlanSuccess_;
    std::vector<float> currentGoal_;
    
    // Private helper methods
    std::vector<std::vector<ActionType>> generateActionSequences(size_t depth);
    float evaluateAction(ActionType action, const std::vector<float>& state);
    std::vector<float> predictNextState(ActionType action, const std::vector<float>& currentState);
    float getSelfModelConfidence(ActionType action) const;
};

// SelfModel: Internal cognitive model of self and body
// Tracks how one's actions affect the environment
// Used for predicting consequences of planned actions
//
// Key mechanisms:
// - Stores learned action-effect associations
// - Can predict outcomes of actions
// - Updates based on prediction errors
// - Supports goal-directed planning

class SelfModel {
public:
    SelfModel();
    ~SelfModel();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Record an action and its effect on state
    // This learns a new cause-effect relationship
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    // Predict the state that will result from an action
    // Uses similar past experiences to make predictions
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState) const;
    
    // Get confidence in a prediction for this action
    // Higher confidence means more reliable prediction
    float getSelfModelConfidence(ActionType action) const;
    
    // Get how well one understands one's own capabilities
    // Measures consistency between predictions and outcomes
    float getBodyAwareness() const { return bodyAwarenessLevel_; }
    
    // Update model based on prediction error
    // Helps the system learn from mistakes
    void updateSelfModel(const std::vector<float>& predicted,
                       const std::vector<float>& actual,
                       ActionType action);
    
    // Get preferred action for current state
    // Based on past experience and self-knowledge
    ActionType getPreferredAction(const std::vector<float>& state) const;
    
    // Clear all learned experiences
    void clear();
    
    // Get most confident prediction for an action
    // Returns similarity score for matching experiences
    float getPredictionConfidence(ActionType action) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float bodyAwarenessLevel_;
    
    // Action effects database
    // Maps actions to past experiences and their outcomes
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
        
        ActionEffect() : confidence(0.0f), observationCount(0) {}
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;
};

// SocialLearning: Learn from other agents' actions and experiences
// Models how other agents behave and what outcomes their actions produce
//
// Key mechanisms:
// - Observes other agents' actions
// - Learns that actions produce predictable outcomes
// - Can imitate successful actions
// - Builds pattern associations

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Record observation of another agent's action and effect
    // Helps learn about other agents' behavior
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    // Can we imitate this observed action?
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
    
    // Helper method
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
};

} // namespace nlm
