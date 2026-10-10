#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/SocialLearning.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

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
};

} // namespace nlm