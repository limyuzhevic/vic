#pragma once

#include "Action.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

// Motor system: converts neural activity to actions
// Enhanced Phase 2 implementation with real neural-based action selection

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Select action based on neural activity
    // Uses neural population activity, action preferences, and reward signals
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Get motor neuron activity levels for debugging
    std::vector<float> getMotorActivityLevels() const;
    
    // Set exploration vs exploitation balance
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    // Set action selection parameters
    void setActionSelectionThreshold(float threshold) { actionSelectionThreshold_ = threshold; }
    void setActionTemperature(float temperature) { actionTemperature_ = temperature; }
    
    // Check if action is preferred based on history
    bool isActionPreferred(ActionType action) const;
    
    // Get preference score for an action
    float getActionPreferenceScore(ActionType action) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float explorationRate_;
    float actionSelectionThreshold_;
    float actionTemperature_;
    
    // Action preferences for exploration
    std::unordered_map<ActionType, float> actionPreferences_;
    
    // Action history for learning
    std::vector<const Action*> actionHistory_;
    
    // Random number generator
    std::mt19937 rng_;
};

} // namespace nlm