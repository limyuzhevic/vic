#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Motor system: converts neural activity to actions
// Phase 6: Motor action system
// Current implementation: Placeholder action selection
// TODO FUTURE PHASE: Implement real action selection based on neural activity and preferences

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // Phase 6: Abstract action selection
    // TODO FUTURE PHASE: Implement real action selection with reinforcement learning
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
