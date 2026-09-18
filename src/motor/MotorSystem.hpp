#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Motor system: converts neural activity to actions
// PLACEHOLDER - Phase 2 will implement real action selection

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<std::unique_ptr<Action>>& getActionHistory() const;
    void clearHistory();
    
    // Record action result for learning
    void recordActionResult(ActionType action, float reward, float value);
    
    // Statistics tracking
    struct ActionStats {
        size_t executionCount{0};
        float totalReward{0.0f};
        float averageValue{0.0f};
    };
    
    // Get statistics for an action type
    const ActionStats& getActionStats(ActionType action) const;
    
    // Get total number of actions executed
    size_t getTotalActionsExecuted() const;
    
    // Get most frequently executed action
    ActionType getMostFrequentAction() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex;  // Add mutex for thread safety
};

} // namespace nlm
