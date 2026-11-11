#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"

namespace nlm {

// Motor system: converts neural activity to actions
// REAL IMPLEMENTATION - Phase 2 real action selection
class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // REAL IMPLEMENTATION: Select action based on neural activity
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Get action statistics
    const std::vector<float>& getActionStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
