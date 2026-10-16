#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Action selection algorithm types
enum class ActionSelectionAlgorithm {
    WinnerTakeAll,
    Stochastic,
    Softmax,
    ThompsonSampling
};

// Motor system: converts neural activity to actions
// Enhanced implementation with neural activity-based selection and learning

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Enhanced action selection algorithms
    void setActionSelectionAlgorithm(ActionSelectionAlgorithm algorithm);
    ActionSelectionAlgorithm getActionSelectionAlgorithm() const;
    
    // Exploration and temperature control
    void setExplorationNoise(float noise);
    float getExplorationNoise() const;
    
    void setTemperature(float temp);
    float getTemperature() const;
    
    // Learning and statistics
    void recordActionOutcome(ActionType action, bool success);
    
    float getActionPreference(ActionType action) const;
    int getActionSuccessCount(ActionType action) const;
    int getActionTotalCount(ActionType action) const;
    
    std::vector<float> getActionNeuralCorrelations(ActionType action) const;
    
    // Update action selection based on current neural state
    void updateActionSelection(const class NeuralRegion& motorRegion);
    
    // Reward prediction error for reinforcement learning
    float calculateRewardPredictionError(ActionType action, bool outcome) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
