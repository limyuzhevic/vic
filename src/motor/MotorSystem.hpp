#pragma once

#include "Action.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>

namespace nlm {

// Motor system: converts neural activity to actions
// Phase 2: Real action selection based on neural dynamics

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // Analyzes motor neuron populations, incorporates preferences and reward signals
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion, 
                                         const Neuromodulator* rewardSignal = nullptr);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration vs. exploitation)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Update motor system state
    void update(float timestep);
    
    // Reset exploration preferences
    void resetPreferences();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
