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
    
    // Select action based on neural activity
    // TODO PHASE 2: Implement real action selection
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Enhanced action selection using working memory and neuromodulation
    std::unique_ptr<Action> selectActionAdvanced(const class NeuralRegion& motorRegion,
                                                 const class NeuralWorkingMemory* workingMemory,
                                                 const class Neuromodulator* dopamine,
                                                 const class Brain* brain);
    
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

// Motor system implementation header for convenience
inline std::unique_ptr<Action> MotorSystem::selectAction(const class NeuralRegion& motorRegion) {
    return selectActionAdvanced(motorRegion, nullptr, nullptr, nullptr);
}

} // namespace nlm
