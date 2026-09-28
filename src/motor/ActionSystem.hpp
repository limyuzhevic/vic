#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// Action System
// Simple action selection from motor neurons
// Handles production of behavioral actions based on neural activity patterns

class ActionSystem {
public:
    ActionSystem();
    ~ActionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Produce an action based on current motor neuron activity
    std::unique_ptr<Action> produceAction();
    
    // Configure action selection parameters
    void configure(float motorActivationThreshold, float actionCoolDown,
                   float actionStallTime, float noveltyActionBias);
    
    // Get current action statistics
    size_t getTotalActionsProduced() const;
    size_t getActionsByType(ActionType type) const;
    float getActionSuccessRate() const;
    
    // Update action selection state
    void update(float timestep);
    
    // Reset action system state
    void reset();
    
    // Set action parameters for specific behaviors
    void setActionParameters(ActionType actionType, float weight, float urgency);
    
    // Get current motor neuron activity statistics
    float getAverageMotorActivation() const;
    size_t getActiveMotorNeurons() const;
    float getMotorActivationVariance() const;
    
    // Check if action production is stalled
    bool isActionStalled() const;
    
    // Force production of a specific action type
    std::unique_ptr<Action> forceAction(ActionType actionType);
    
    // Get action history for analysis
    const std::vector<std::unique_ptr<Action>>& getActionHistory() const;
    
    // Enable/disable specific action types
    void enableActionType(ActionType actionType, bool enabled);
    bool isActionTypeEnabled(ActionType actionType) const;
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
