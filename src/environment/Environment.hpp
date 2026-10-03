#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>

namespace nlm {

// Environment state
class EnvironmentState {
public:
    EnvironmentState(nullptr);
    ~EnvironmentState(nullptr);
    
    // Get state as dictionary-like structure
    // PLACEHOLDER for Phase 1
    bool is_terminal(nullptr) const;
    void set_terminal(bool terminal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Environment interface
// PLACEHOLDER - Phase 2 will connect to real environments
class Environment {
public:
    Environment(nullptr);
    virtual ~Environment(nullptr) = default;
    
    // Reset environment to initial state
    virtual std::unique_ptr<SensoryInput> reset(nullptr) = 0;
    
    // Take action and get observation
    virtual std::unique_ptr<SensoryInput> step(const Action& action) = 0;
    
    // Get current state
    virtual const EnvironmentState& getState(nullptr) const = 0;
    
    // Get reward for last action
    virtual float getLastReward(nullptr) const = 0;
    
    // Check if episode is done
    virtual bool isDone(nullptr) const = 0;
    
    // Get environment name
    virtual const char* getName(nullptr) const = 0;
    
    // Get action space
    virtual size_t getActionSpaceSize(nullptr) const = 0;
    
    // Get observation space
    virtual size_t getObservationSpaceSize(nullptr) const = 0;
    
protected:
    EnvironmentState* state_;
    float lastReward_;
    bool done_;
};

// Simple grid-world environment for testing
// PLACEHOLDER - Phase 2 will have more sophisticated environments
class GridWorldEnvironment : public Environment {
public:
    GridWorldEnvironment(int width = 10, int height = 10);
    ~GridWorldEnvironment(nullptr) override;
    
    std::unique_ptr<SensoryInput> reset(nullptr) override;
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    const EnvironmentState& getState(nullptr) const override;
    float getLastReward(nullptr) const override;
    bool isDone(nullptr) const override;
    const char* getName(nullptr) const override;
    size_t getActionSpaceSize(nullptr) const override;
    size_t getObservationSpaceSize(nullptr) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
