#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <deque>

namespace nlm {

// Action statistics for evaluation
struct ActionStatistics {
    int executionCount;
    int successCount;
    float averageExecutionTime;
    float averageReward;
    float averagePredictionError;
    float noveltyScore;
    
    ActionStatistics() 
        : executionCount(0), successCount(0), averageExecutionTime(0.0f)
        , averageReward(0.0f), averagePredictionError(0.0f), noveltyScore(0.0f) {}
};

// Action selection history entry
struct ActionHistoryEntry {
    SimulationStep timestamp;
    ActionType action;
    std::unique_ptr<Action> actionData;
    float neuralActivation;
    float predictedOutcome;
    float actualOutcome;
    float reward;
    float predictionError;
    bool success;
    
    ActionHistoryEntry() 
        : timestamp(0), neuralActivation(0.0f), predictedOutcome(0.0f)
        , actualOutcome(0.0f), reward(0.0f), predictionError(0.0f), success(false) {}
};

// Motor system: converts neural activity to actions with biological plausibility
// REAL IMPLEMENTATION - Phase 2 complete

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Select action based on memory systems
    std::unique_ptr<Action> selectActionBasedOnMemory();
    
    // Select action based on prediction system
    std::unique_ptr<Action> selectActionBasedOnPrediction();
    
    // Select action based on neural planner
    std::unique_ptr<Action> selectActionBasedOnPlanner();
    
    // Evaluate action based on all available information
    float evaluateAction(ActionType action);
    
    // Action preference and adaptation
    void setActionPreference(ActionType action, float preference);
    const std::unordered_map<ActionType, float>& getActionPreferences() const;
    void adaptActionPreferences();
    void reinforceAction(ActionType action, float strength);
    
    // Action history and statistics
    void recordAction(ActionType action, const Action& actionData);
    const std::deque<ActionHistoryEntry>& getActionHistory() const;
    const std::unordered_map<ActionType, ActionStatistics>& getActionStatistics() const;
    float getSuccessRate(ActionType action) const;
    std::deque<ActionType> getActionSequence(size_t maxLength) const;
    
    // Performance optimization and error handling
    void optimizeActionSelection();
    void handleActionFailure(ActionType action);
    void resetActionHistory();
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Configuration
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
