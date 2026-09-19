#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

// MotorSystem: Controls action selection and motor output
// Integrates with brain neural activity to produce behavior
// NOT a traditional action selection mechanism - emerges from neural dynamics
class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process current state and select action
    ActionType selectAction(const std::vector<float>& currentState, 
                           SimulationStep currentStep);
    
    // Get action quality/confidence
    float getActionConfidence(ActionType action, 
                             const std::vector<float>& state) const;
    
    // Learn from action outcomes
    void learnFromExperience(ActionType action,
                            const std::vector<float>& beforeState,
                            const std::vector<float>& afterState,
                            float reward);
    
    // Set action preferences
    void setActionPreference(ActionType action, float preference);
    
    // Get action preference
    float getActionPreference(ActionType action) const;
    
    // Get action statistics
    float getActionCount(ActionType action) const;
    float getActionSuccessRate(ActionType action) const;
    
    // Enable/disable action types
    void enableActionType(ActionType action, bool enable);
    bool isActionTypeEnabled(ActionType action) const;
    
    // Get enabled action types
    std::vector<ActionType> getEnabledActions() const;
    
    // Clear learning history
    void clearLearningHistory();
    
    // Set exploration vs exploitation balance
    void setExplorationMode(float explorationRate);
    float getExplorationMode() const;
    
    // Get action diversity (measured across recent actions)
    float getActionDiversity() const;
    
    // Apply motor command to environment
    MotorCommand generateMotorCommand(ActionType action, float confidence) const;
    
    // Get prediction error for action selection
    float getActionPredictionError(ActionType action,
                                   const std::vector<float>& state) const;
    
    // Check if action should be explored based on curiosity
    bool shouldExplore(ActionType currentAction, float curiosityLevel,
                      SimulationStep currentStep) const;
    
    // Get action sequence for planning
    std::vector<ActionType> predictActionSequence(
        const std::vector<float>& currentState,
        size_t sequenceLength) const;
    
    // Set action reward modulation
    void setActionRewardMod(ActionType action, float modValue);
    
    // Get action reward modulation
    float getActionRewardMod(ActionType action) const;
    
    // Enable/disable action learning
    void enableActionLearning(bool enable);
    bool isActionLearningEnabled() const;
    
    // Calculate action value (utility) based on history
    float calculateActionValue(ActionType action,
                               const std::vector<float>& state) const;
    
    // Get action execution count
    size_t getActionExecutionCount(ActionType action) const;
    
    // Check if action was recently successful
    bool wasActionRecentlySuccessful(ActionType action,
                                    SimulationStep lookbackSteps) const;
    
    // Get action statistics for analysis
    struct ActionStats {
        ActionType action;
        size_t executionCount;
        float successRate;
        float averageReward;
        float confidence;
        float preference;
        size_t lastExecutionStep;
    };
    
    std::vector<ActionStats> getActionStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
