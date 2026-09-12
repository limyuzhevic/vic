#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>

namespace nlm {

// TypeInfo: Helper for type checking and validation
// Used to validate neural activity patterns and state consistency
struct TypeInfo {
    static bool isValidActionType(ActionType action) {
        return static_cast<size_t>(action) < static_cast<size_t>(ActionType::Custom);
    }
    
    static bool isValidNeuronType(NeuronType type) {
        return static_cast<size_t>(type) < static_cast<size_t>(NeuronType::Internal);
    }
    
    static bool isValidRegionId(const RegionId& id) {
        return id.index() != static_cast<uint64_t>(INVALID_REGION_ID);
    }
    
    static bool isValidNeuronId(const NeuronId& id) {
        return id.index() != static_cast<uint64_t>(INVALID_NEURON_ID);
    }
    
    static bool isValidSynapseId(const SynapseId& id) {
        return id.index() != static_cast<uint64_t>(INVALID_SYNAPSE_ID);
    }
    
    static size_t actionToIndex(ActionType action) {
        return static_cast<size_t>(action);
    }
    
    static size_t neuronTypeToIndex(NeuronType type) {
        return static_cast<size_t>(type);
    }
};

// PlanningCandidate: A possible action sequence considered during planning
struct PlanningCandidate {
    std::vector<ActionType> actions;
    std::vector<std::vector<float>> predictedStates;
    float expectedReward;
    float confidence;
    size_t depth;
    
    PlanningCandidate() 
        : expectedReward(0), confidence(0), depth(0) {}
    
    // Validation
    bool isValid() const {
        return !actions.empty() && expectedReward >= -1000.0f && confidence >= 0.0f;
    }
    
    // Debug output
    std::string toString() const {
        std::stringstream ss;
        ss << "PlanningCandidate[depth=" << depth << ",reward=" << expectedReward 
           << ",confidence=" << confidence << ",actions=" << actions.size() << "]";
        return ss.str();
    }
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
        if (TypeInfo::isValidActionType(action)) {
            actionQuality_[TypeInfo::actionToIndex(action)] = quality;
        }
    }
    
    // Get current goal
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    // Has recent planning been successful?
    bool wasRecentPlanSuccessful() const;
    
    // Configuration options
    void setMaxSequences(size_t max) { maxSequences_ = max; }
    void setSequencePruneThreshold(float threshold) { sequencePruneThreshold_ = threshold; }
    void setConfidenceThreshold(float threshold) { confidenceThreshold_ = threshold; }
    
    // Statistics
    float getAveragePlanSuccess() const;
    size_t getTotalPlansEvaluated() const { return totalPlansEvaluated_; }
    
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
    
    // Configuration options
    size_t maxSequences_;
    float sequencePruneThreshold_;
    float confidenceThreshold_;
    
    // Statistics
    size_t totalPlansEvaluated_;
    float cumulativeSuccessRate_;
};

} // namespace nlm