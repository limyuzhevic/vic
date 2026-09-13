#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>

namespace nlm {

// Environment state
/**
 * @class EnvironmentState
 * @brief Encapsulates the current state of the environment
 * 
 * EnvironmentState represents the complete state information of the environment
 * at a given point in time. It includes terminal status and can be extended
 * to include other state variables in future phases.
 */
class EnvironmentState {
public:
    /**
     * @brief Construct a new EnvironmentState object
     * 
     * Initializes the environment state to a clean starting state.
     * The implementation details are hidden using the pImpl idiom.
     */
    EnvironmentState();
    
    /**
     * @brief Destroy the EnvironmentState object
     * 
     * Cleans up internal resources used to store state information.
     */
    ~EnvironmentState();
    
    /**
     * @brief Check if the current state is terminal (episode ending)
     * 
     * @return true if the episode has ended and cannot continue
     * @return false if the episode is still active
     * 
     * @note This is a placeholder for Phase 1 implementation
     */
    bool is_terminal() const;
    
    /**
     * @brief Set the terminal status of the environment state
     * 
     * @param terminal true if the state should be marked as terminal
     * 
     * @note This is a placeholder for Phase 1 implementation
     */
    void set_terminal(bool terminal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Environment interface
// PLACEHOLDER - Phase 2 will connect to real environments
/**
 * @class Environment
 * @brief Abstract base class for all simulated environments
 * 
 * Environment defines the interface that all concrete environment implementations
 * must satisfy. It provides the fundamental lifecycle for agent-environment interaction:
 * reset, step, and state management. This class is part of the reinforcement learning
 * environment interface, defining how an agent perceives and acts within its world.
 * 
 * The environment maintains:
 * - Current agent state (via EnvironmentState)
 * - Last received reward signal
 * - Episode termination status
 * 
 * Subclasses must implement all pure virtual methods to provide specific
 * environment dynamics and observation models.
 */
class Environment {
public:
    /**
     * @brief Construct a new Environment object
     * 
     * Initializes protected members (state_, lastReward_, done_) to default values.
     * Concrete implementations typically override this constructor to set up
     * environment-specific parameters.
     */
    Environment();
    
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of derived class resources.
     */
    virtual ~Environment() = default;
    
    /**
     * @brief Reset the environment to its initial state
     * 
     * Resets the environment to a starting configuration, returning the first
     * observation that the agent receives upon entering the new episode.
     * 
     * @return std::unique_ptr<SensoryInput> First observation after reset
     * @note This is a pure virtual method that must be implemented by subclasses
     */
    virtual std::unique_ptr<SensoryInput> reset() = 0;
    
    /**
     * @brief Execute an action in the environment and return the next observation
     * 
     * Applies the given action to the current environment state and returns the
     * resulting sensory observation. Also updates the environment's internal state
     * including reward and termination status.
     * 
     * @param action Action to execute in the environment
     * @return std::unique_ptr<SensoryInput> Observation resulting from the action
     * @note This is a pure virtual method that must be implemented by subclasses
     */
    virtual std::unique_ptr<SensoryInput> step(const Action& action) = 0;
    
    /**
     * @brief Get the current environment state
     * 
     * @return const EnvironmentState& Reference to the current environment state
     * 
     * @note This is a pure virtual method that must be implemented by subclasses
     */
    virtual const EnvironmentState& getState() const = 0;
    
    /**
     * @brief Get the reward received from the last action
     * 
     * @return float Reward value associated with the last executed action
     * @note Returns the reward most recently set by step() or reset()
     */
    virtual float getLastReward() const = 0;
    
    /**
     * @brief Check if the current episode is complete
     * 
     * @return true if the episode has ended and should be reset
     * @return false if the episode is still ongoing
     */
    virtual bool isDone() const = 0;
    
    /**
     * @brief Get the name of the environment
     * 
     * @return const char* String identifier for the environment type
     * Used for logging, debugging, and experiment tracking
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the size of the action space
     * 
     * @return size_t Number of possible discrete actions in this environment
     * Determines the output dimension of the agent's policy network
     */
    virtual size_t getActionSpaceSize() const = 0;
    
    /**
     * @brief Get the size of the observation space
     * 
     * @return size_t Dimensionality of the sensory observation vector
     * Determines the input dimension for the agent's perception system
     */
    virtual size_t getObservationSpaceSize() const = 0;
    
protected:
    /**
     * @brief Current environment state
     * 
     * Stores the complete state information of the environment at the current time step.
     * Implementation details are hidden via the pImpl idiom.
     */
    std::unique_ptr<EnvironmentState> state_;
    
    /**
     * @brief Reward from the last action
     * 
     * Updated by concrete environment implementations during step() or reset().
     */
    float lastReward_;
    
    /**
     * @brief Episode termination flag
     * 
     * Set to true when the episode reaches a terminal state.
     */
    bool done_;
};

// Simple grid-world environment for testing
// PLACEHOLDER - Phase 2 will have more sophisticated environments
/**
 * @class GridWorldEnvironment
 * @brief A simple 2D grid-based environment for testing and development
 * 
 * GridWorldEnvironment provides a basic navigation environment where an agent
 * moves on a discrete grid. The agent can move in four directions (forward,
 * backward, turn left, turn right, interact, wait) and receives sensory
 * input from vision, touch, internal state, and proprioception sensors.
 * 
 * Key features:
 * - Discrete grid world with configurable width and height
 * - Simple dynamics: agent moves one cell per action
 * - Boundary handling with wall collision detection
 * - Basic reward system: +1 for reaching goals, -1 for collisions
 * - Suitable for testing basic reinforcement learning algorithms
 * 
 * @note This is a placeholder implementation for Phase 1. Phase 2 will
 * provide more sophisticated environments like mazes, puzzles, and continuous worlds.
 */
class GridWorldEnvironment : public Environment {
public:
    /**
     * @brief Construct a new GridWorldEnvironment object
     * 
     * @param width Width of the grid world in cells (default: 10)
     * @param height Height of the grid world in cells (default: 10)
     * 
     * Creates a grid world with the specified dimensions, initializing
     * the agent's starting position to the center of the grid.
     */
    GridWorldEnvironment(int width = 10, int height = 10);
    
    /**
     * @brief Destroy the GridWorldEnvironment object
     * 
     * Cleans up internal resources and resets the grid world state.
     */
    ~GridWorldEnvironment() override;
    
    /**
     * @brief Reset the grid world to its initial state
     * 
     * @return std::unique_ptr<SensoryInput> First observation (vision sensor)
     * Resets the agent to the center of the grid and initializes all sensors
     * to their starting values.
     */
    std::unique_ptr<SensoryInput> reset() override;
    
    /**
     * @brief Execute an action in the grid world and get the next observation
     * 
     * @param action Action to execute (forward, backward, turn, interact, wait)
     * @return std::unique_ptr<SensoryInput> New observation after executing the action
     * Updates the agent's position and orientation based on the action,
     * processes collisions with walls and boundaries, and computes sensory inputs.
     */
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    
    /**
     * @brief Get the current grid world state
     * 
     * @return const EnvironmentState& Reference to the current state object
     */
    const EnvironmentState& getState() const override;
    
    /**
     * @brief Get the reward received from the last action
     * 
     * @return float Reward value: +1 for goals, -1 for collisions, 0 otherwise
     */
    float getLastReward() const override;
    
    /**
     * @brief Check if the episode is complete
     * 
     * @return true if the agent reached a goal or terminated
     * @return false if the episode is still ongoing
     */
    bool isDone() const override;
    
    /**
     * @brief Get the name of this environment
     * 
     * @return const char* Always returns "GridWorldEnvironment"
     */
    const char* getName() const override;
    
    /**
     * @brief Get the size of the action space
     * 
     * @return size_t Number of actions: 6 (forward, backward, turn left, turn right, interact, wait)
     */
    size_t getActionSpaceSize() const override;
    
    /**
     * @brief Get the size of the observation space
     * 
     * @return size_t Dimensionality of concatenated sensory inputs (vision + touch + internal + proprioception)
     */
    size_t getObservationSpaceSize() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
