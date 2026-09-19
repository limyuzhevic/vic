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
    
    // Select action based on neural activity in motor region
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Calculate action preference score based on neural activity
    float calculateActionScore(const ActionType& action, const NeuralRegion& motorRegion) const;
    
    // Enable/disable action selection strategies
    void enableActionSelectionStrategy(bool enable) { actionSelectionEnabled_ = enable; }
    bool isActionSelectionEnabled() const { return actionSelectionEnabled_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool actionSelectionEnabled_;
};

} // namespace nlm
