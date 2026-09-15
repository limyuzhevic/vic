#pragma once

#include "Action.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/Neuron.hpp"
#include "../../core/Random/Random.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Motor system: converts neural activity to actions
// IMPLEMENTED: Real action selection based on neural population activity patterns

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // IMPLEMENTED: Winner-take-all competition with temporal integration
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration/exploitation)
    // IMPLEMENTED: Action preference weights for intelligent exploration
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
