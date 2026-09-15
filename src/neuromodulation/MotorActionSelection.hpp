#pragma once

#include "Action.hpp"
#include <memory>

namespace nlm {

// Motor Action Selection: selects actions based on neural activity
// Simplified action selection system using softmax over motor neuron activity

class MotorActionSelection {
public:
    MotorActionSelection();
    ~MotorActionSelection();
    
    // Select action based on motor neuron activity
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Apply dopamine-modulated action selection
    void applyDopamineModulation(float dopamineLevel, float temperature);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Set temperature for softmax (exploration vs exploitation)
    void setTemperature(float temperature);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
