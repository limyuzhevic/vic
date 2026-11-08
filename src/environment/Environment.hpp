#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>

namespace nlm {

// Environment state
class EnvironmentState {
public:
    EnvironmentState();
    ~EnvironmentState();
    
    // Get state as dictionary-like structure
    // Phase 6: Real environment state implementation
    bool is_terminal() const;
    void set_terminal(bool terminal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Environment interface
// Phase 6: Real environment implementations (SimpleWorld, GridWorld)
class Environment {
public:
    Environment();
    virtual ~Environment() = default;
    
    // Reset environment to initial state
    virtual std::unique_ptr<SensoryInput> reset() = 0;
    
    // Take action and get observation
    virtual std::unique_ptr<SensoryInput> step(const Action& action) = 0;
    
    // Get current state
    virtual const EnvironmentState& getState() const = 0;
    
    // Get reward for last action
    virtual float getLastReward() const = 0;
    
    // Check if episode is done
    virtual bool isDone() const = 0;
    
    // Get environment name
    virtual const char* getName() const = 0;
    
    // Get action space
    virtual size_t getActionSpaceSize() const = 0;
    
    // Get observation space
    virtual size_t getObservationSpaceSize() const = 0;
    
protected:
    EnvironmentState* state_;
    float lastReward_;
    bool done_;
};

// Simple grid-world environment for testing
// Phase 6: Advanced grid-world with complex object interactions
