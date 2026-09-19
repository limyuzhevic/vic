#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Motor system: converts neural activity to actions
// Implements biologically realistic motor control with population vector decoding,
// reinforcement learning, and competitive dynamics for action selection.

class MotorSystem {
public:
    // Configuration parameters for motor system behavior
    struct Config {
        // Exploration parameters
        float explorationRate;        // Probability of random action selection (0.0-1.0)
        float temperature;             // Softmax temperature parameter (higher = more exploration)
        
        // Noise and persistence parameters
        float internalNoise;          // Neural noise level (0.0-1.0)
        float actionPersistence;      // Time constant for action persistence (ms)
        
        // Reinforcement learning parameters
        float learningRate;            // TD learning rate (alpha)
        float rewardScale;             // Reward scaling factor
        float discountFactor;          // Temporal discount factor (gamma)
        
        // Eligibility traces
        float eligibilityDecay;        // Trace decay rate (lambda)
        
        // Action value initialization
        float initialActionValue;      // Initial Q-value for all actions
        
        // Validation and bounds
        float minExplorationRate;     // Minimum allowed exploration rate
        float maxExplorationRate;     // Maximum allowed exploration rate
        float minTemperature;          // Minimum temperature
        float maxTemperature;          // Maximum temperature
        float minLearningRate;         // Minimum learning rate
        float maxLearningRate;         // Maximum learning rate
        
        Config()
            : explorationRate(0.1f)
            , temperature(1.0f)
            , internalNoise(0.05f)
            , actionPersistence(100.0f)
            , learningRate(0.01f)
            , rewardScale(1.0f)
            , discountFactor(0.95f)
            , eligibilityDecay(0.9f)
            , initialActionValue(0.0f)
            , minExplorationRate(0.0f)
            , maxExplorationRate(1.0f)
            , minTemperature(0.01f)
            , maxTemperature(10.0f)
            , minLearningRate(0.0f)
            , maxLearningRate(1.0f) {}
    };
    
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity using population vector decoding,
    // competitive dynamics, and reinforcement learning.
    // Precondition: motorRegion must be valid and contain motor neuron populations
    // Postcondition: Returns a valid Action or nullptr if selection fails
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions for motor output
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preference based on reinforcement learning
    // Updates action value estimates using temporal difference learning
    // Precondition: action must be in availableActions, preference must be finite
    void setActionPreference(ActionType action, float preference);
    
    // Get action history for reinforcement learning
    const std::vector<const Action*>& getActionHistory() const;
    
    // Clear action history and reset reinforcement learning state
    void clearHistory();
    
    // Get current configuration
    const Config& getConfig() const;
    
    // Update configuration with validation
    // Validates parameter ranges before applying updates
    bool updateConfig(const Config& newConfig);
    
    // Get current action value estimates (for debugging/analysis)
    std::unordered_map<ActionType, float> getActionValues() const;
    
    // Record reward for the last selected action
    // Precondition: reward must be finite
    void recordReward(float reward);
    
    // Advance internal state by one timestep
    // Updates action persistence, eligibility traces, and internal state
    void advanceStep(float dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
