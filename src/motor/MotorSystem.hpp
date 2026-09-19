#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Motor system: converts neural activity to actions
// PLACEHOLDER - Phase 2 will implement real action selection

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Add action to history
    void addActionToHistory(std::unique_ptr<Action> action);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
