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
    // PLACEHOLDER for Phase 1
    bool is_terminal() const;
    void set_terminal(bool terminal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Environment interface
// PLACEHOLDER - Phase 2 will connect to real environments
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
// PLACEHOLDER - Phase 2 will have more sophisticated environments
class GridWorldEnvironment : public Environment {
public:
    GridWorldEnvironment(int width = 10, int height = 10);
    ~GridWorldEnvironment() override;
    
    std::unique_ptr<SensoryInput> reset() override;
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    const EnvironmentState& getState() const override;
    float getLastReward() const override;
    bool isDone() const override;
    const char* getName() const override;
    size_t getActionSpaceSize() const override;
    size_t getObservationSpaceSize() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
