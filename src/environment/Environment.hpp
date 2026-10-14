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
// 
// The Environment class provides the interface for interaction with external
// world states and computational environments. It implements the basic
// reinforcement learning environment paradigm: agent observes, takes actions,
// receives rewards, and learns from experience.
// 
// Core components:
// - State space: Discrete or continuous representation of environment state
// - Action space: Set of possible actions the agent can take
// - Reward function: Scalar feedback for actions and states
// - Episode management: Start/reset, termination conditions
// - Transition dynamics: How actions change environment state
// 
// Biological inspiration:
// - Animal perception and action in natural environments
// - Embodied cognition and sensorimotor integration
// - Active perception and exploration behavior
// - Adaptation to changing environmental conditions
// 
// Phase 2 Implementation:
// - Real-time environment interaction with spiking neurons
// - Multimodal sensory input (vision, auditory, proprioceptive)
// - Motor output generation and execution
// - Reward prediction error computation
// - Integration with neuromodulatory systems
// - Developmental learning from environmental experience

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
    
    // Get environment description
    virtual const std::string& getDescription() const = 0;
    
    // Get environment parameters
    virtual float getParameter(const std::string& name) const = 0;
    virtual void setParameter(const std::string& name, float value) = 0;
    
    // Integration with neural systems
    virtual void setBrain(std::shared_ptr<class Brain> brain) = 0;
    virtual void updateWithNeurons(const std::vector<Neuron*>& neurons) = 0;
    
    // Environment analysis
    virtual std::vector<float> analyzeState() const = 0;
    virtual std::vector<Action> getAvailableActions() const = 0;
    
    // Environment-specific methods
    virtual bool isValidAction(const Action& action) const = 0;
    virtual float getActionCost(const Action& action) const = 0;
    virtual float getStateValue(const EnvironmentState& state) const = 0;
};
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
