#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

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
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Neural integration methods
    void processMotorNeurons(const class NeuralRegion& motorRegion);
    float getActionStrength(ActionType action) const;
    void updateActionStrengths(ActionType selectedAction);
    void applyNeuromodulation(float neuromodulationLevel);
    
    // Curiosity-driven exploration support
    void setCuriosityLevel(float level);
    float getCuriosityLevel() const;
    
    // Action selection with exploration/exploitation balance
    std::unique_ptr<Action> selectActionWithExploration(const class NeuralRegion& motorRegion,
                                                         float explorationProbability);
    
    // Get all neural activity patterns
    std::unordered_map<ActionType, float> getActivityPatterns() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
