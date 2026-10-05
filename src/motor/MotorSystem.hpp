#pragma once

#include "Action.hpp"
#include "AgentBrain.hpp"
#include "../agent/AgentBody.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Motor system: converts neural activity to actions with advanced integration
// Enhanced version with full neural system integration

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // Enhanced version with neuromodulation, curiosity, and reward-based selection
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Enhanced integration methods
    // Connect MotorSystem to AgentBrain for neural coordination
    void integrateWithAgentBrain(AgentBrain* agentBrain);
    
    // Update sensory inputs for motor integration
    void updateSensoryInputs(const SensoryPercept& percept);
    
    // Execute pending actions with environmental feedback
    void executeActions(const AgentBody& agentBody, const SimpleWorld& world,
                       double timestep, Timestamp currentTime);
    
    // Get enhanced motor system statistics
    struct MotorSystemStats {
        size_t totalActionsExecuted;
        float averageReward;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        float explorationRate;
        float learningRate;
        float proficiencyLevel;
        std::unordered_map<ActionType, float> actionRewards;
        std::unordered_map<ActionType, float> actionProficiencies;
        
        MotorSystemStats() : totalActionsExecuted(0), averageReward(0.0f), 
                           curiosityLevel(0.0f), noveltyLevel(0.0f), dopamineLevel(0.0f),
                           explorationRate(0.0f), learningRate(0.0f), proficiencyLevel(0.0f) {}
    };
    
    // Get system statistics
    MotorSystemStats getSystemStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
