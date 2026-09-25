#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Motor system: Converts neural activity to adaptive actions
// Real implementation based on action selection and reinforcement learning

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity patterns
    // Implements action selection using policy gradient or actor-critic methods
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available action types
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences for exploration
    void setActionPreference(ActionType action, float preference);
    
    // Get action history for analysis
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
