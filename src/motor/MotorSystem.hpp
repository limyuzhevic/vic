#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace nlm {

// Motor system: converts neural activity to actions
// Uses neural firing patterns and action preferences to select optimal actions

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity from motor region
    // Combines exploitation (highest neural activity) with exploration
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion,
                                         float curiosityLevel = 0.0f,
                                         bool exploreEnabled = false);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration bias)
    void setActionPreference(ActionType action, float preference);
    float getActionPreference(ActionType action) const;
    
    // Get action history for learning
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Record outcome of last action for learning
    void recordActionOutcome(const Action& action, float reward, float predictedReward);
    
    // Update action selection parameters
    void updateActionPreferences(float explorationRate, float exploitationRate);
    
    // Reset system
    void reset();
    
    // Get current action statistics
    float getActionSuccessRate(ActionType action) const;
    float getActionFrequency(ActionType action) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
