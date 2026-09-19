// INeuralPlanner.hpp
// Abstract interface for neural planning systems
// Plans sequences of actions to achieve goals using predictive coding
// Integrates with prediction and action systems

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class PredictionSystem;

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

class INeuralPlanner {
public:
    virtual ~INeuralPlanner() = default;

    // Initialize planner with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Plan next action given current state and goal
    // Returns the best action to take now
    virtual ActionType planAction(const std::vector<float>& currentState,
                                 float targetReward = 0.5f) = 0;

    // Evaluate a potential action sequence
    // Returns expected total reward and confidence
    virtual PlanningCandidate evaluateSequence(const std::vector<ActionType>& actions,
                                              const std::vector<float>& startState) = 0;

    // Get number of steps to look ahead
    virtual size_t getPlanningHorizon() const = 0;
    virtual void setPlanningHorizon(size_t horizon) = 0;

    // Get planning depth
    virtual size_t getPlanningDepth() const = 0;
    virtual void setPlanningDepth(size_t depth) = 0;

    // Update planner with new information
    virtual void update(TimestepDuration dt) = 0;

    // Reset planner
    virtual void reset() = 0;

    // Get current planning statistics
    virtual std::string getPlanningStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "INeuralPlanner"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for planning events
    using ActionPlannedCallback = std::function<void(ActionType action, float confidence)>;
    virtual void setActionPlannedCallback(const ActionPlannedCallback& callback) = 0;

    using SequenceEvaluatedCallback = std::function<void(const PlanningCandidate& candidate)>;
    virtual void setSequenceEvaluatedCallback(const SequenceEvaluatedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getPlanningAccuracy() const = 0;
    virtual float getPlanningSpeed() const = 0;  // Plans per second
    virtual uint64_t getTotalActionsPlanned() const = 0;

    // Planning parameters
    virtual float getExplorationRate() const = 0;
    virtual void setExplorationRate(float rate) = 0;
    virtual float getExploitationRate() const = 0;
    virtual void setExploitationRate(float rate) = 0;
    virtual float getActionSelectionTemperature() const = 0;
    virtual void setActionSelectionTemperature(float temp) = 0;

protected:
    INeuralPlanner() = default;
};

} // namespace nlm
