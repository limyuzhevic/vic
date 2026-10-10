#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @file Environment.hpp
 * 
 * Environment management system for NLM
 * 
 * This file provides the environment management system for NLM, implementing:
 * - Environment state tracking
 * - Environment interface for agent interaction
 * - Simple grid-world environment for testing
 * - Episode management and reward tracking
 * - State observation and action processing
 * 
 * The environment system in NLM:
 * - **EnvironmentState**: Represents the state of the environment
 * - **Environment**: Abstract interface for environment interaction
 * - **GridWorldEnvironment**: Simple grid-world environment for testing
 * - **State management**: Tracks environment state and transitions
 * - **Episode management**: Manages episodes and rewards
 * 
 * Key features:
 * - **State tracking**: Tracks environment state and transitions
 * - **Episode management**: Manages episodes and rewards
 * - **Action processing**: Processes agent actions in the environment
 * - **State observation**: Observes environment state for the agent
 * - **Reward tracking**: Tracks rewards from environment interactions
 * 
 * Environment characteristics:
 * - **State representation**: Represents environment state
 * - **Action processing**: Processes agent actions
 * - **State observation**: Observes environment for the agent
 * - **Reward tracking**: Tracks environmental rewards
 * - **Episode management**: Manages episodes and termination
 * 
 * @note Critical for agent-environment interaction
 * @note Used extensively in agent-environment systems
 * @note Essential for learning and adaptation
 * @see AgentBrain.hpp - For brain integration
 * @see Action.hpp - For action execution
 * @see SensoryInput.hpp - For observation processing
 * @see Environment.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class EnvironmentState
 * 
 * Neural implementation of environment state tracking.
 * 
 * This class implements the environment state representation, tracking
 * the current state of the environment. It is essential for:
 * - **State tracking**: Tracks the current state of the environment
 * - **State management**: Manages environment state transitions
 * - **State observation**: Observes environment state for the agent
 * - **State processing**: Processes environment state
 * 
 * Environment state characteristics in NLM:
 * - **State representation**: Represents the environment state
 * - **State management**: Manages environment state
 * - **State observation**: Observes environment state
 * - **State processing**: Processes environment state
 * 
 * @note Critical for environment state tracking
 * @see Environment - For environment interface
 * @see SensoryInput - For observation processing
 * 
 * @author NLM Development Team
 */
class EnvironmentState {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default environment state instance with empty values.
     * The environment state is initialized with default values for
     * all state fields.
     */
    EnvironmentState();
    
    /**
     * @brief Destructor
     * 
     * Cleans up environment state resources and performs any necessary
     * cleanup operations. The environment state is left in a valid but
     * non-operational state after destruction.
     */
    ~EnvironmentState();
    
    /**
     * @brief Get state as dictionary-like structure
     * 
     * Returns the environment state as a dictionary-like structure.
     * This is a placeholder for Phase 1 implementation and is used for:
     * - State representation
     * - State description
     * - State analysis
     * - State processing
     */
    bool is_terminal() const;
    
    /**
     * @brief Set terminal state
     * 
     * Sets the terminal state of the environment. This is used for:
     * - Terminal state setting
     * - Environment termination
     * - Episode management
     * - Episode termination
     */
    void set_terminal(bool terminal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class Environment
 * 
 * Neural implementation of environment interface.
 * 
 * This class implements the environment interface, providing an abstract
 * interface for environment interaction. It is essential for:
 * - **Environment interaction**: Interacts with the environment
 * - **Action processing**: Processes agent actions in the environment
 * - **State observation**: Observes environment state for the agent
 * - **Reward tracking**: Tracks rewards from environment interactions
 * - **Episode management**: Manages episodes and termination
 * 
 * Environment characteristics in NLM:
 * - **State tracking**: Tracks environment state and transitions
 * - **Action processing**: Processes agent actions
 * - **State observation**: Observes environment for the agent
 * - **Reward tracking**: Tracks environmental rewards
 * - **Episode management**: Manages episodes and termination
 * 
 * @note Placeholder implementation for Phase 1
 * @see EnvironmentState - For environment state
 * @see SensoryInput - For observation processing
 * @see Action - For action execution
 * 
 * @author NLM Development Team
 */
class Environment {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default environment instance with default parameters.
     * The environment is initialized with default settings for
     * environment interaction.
     */
    Environment();
    
    /**
     * @brief Destructor
     * 
     * Cleans up environment resources and performs any necessary
     * cleanup operations. The environment is left in a valid but
     * non-operational state after destruction.
     */
    virtual ~Environment() = default;
    
    /**
     * @brief Reset environment to initial state
     * 
     * Resets the environment to its initial state. This is used for:
     * - Environment reset
     * - Episode initialization
     * - State reset
     * - Environment management
     */
    virtual std::unique_ptr<SensoryInput> reset() = 0;
    
    /**
     * @brief Take action and get observation
     * 
     * Takes an action and gets the observation from the environment.
     * This is used for:
     * - Action processing
     * - Observation generation
     * - Environment interaction
     * - Action processing
     */
    virtual std::unique_ptr<SensoryInput> step(const Action& action) = 0;
    
    /**
     * @brief Get current state
     * 
     * Returns the current state of the environment. This is used for:
     * - State observation
     * - State analysis
     * - State processing
     * - State management
     */
    virtual const EnvironmentState& getState() const = 0;
    
    /**
     * @brief Get reward for last action
     * 
     * Returns the reward for the last action. This is used for:
     * - Reward tracking
     * - Reward management
     * - Reward analysis
     * - Reward processing
     */
    virtual float getLastReward() const = 0;
    
    /**
     * @brief Check if episode is done
     * 
     * Checks if the episode is done. This is used for:
     * - Episode management
     * - Episode termination
     * - Episode control
     * - Episode processing
     */
    virtual bool isDone() const = 0;
    
    /**
     * @brief Get environment name
     * 
     * Returns the name of the environment. This is used for:
     * - Environment identification
     * - Environment naming
     * - Environment description
     * - Environment analysis
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get action space size
     * 
     * Returns the size of the action space. This is used for:
     * - Action space analysis
     * - Action space management
     * - Action space processing
     * - Action space control
     */
    virtual size_t getActionSpaceSize() const = 0;
    
    /**
     * @brief Get observation space size
     * 
     * Returns the size of the observation space. This is used for:
     * - Observation space analysis
     * - Observation space management
     * - Observation space processing
     * - Observation space control
     */
    virtual size_t getObservationSpaceSize() const = 0;
    
protected:
    /** Environment state */
    EnvironmentState* state_;
    
    /** Last reward received */
    float lastReward_;
    
    /** Episode termination flag */
    bool done_;
};

/**
 * @class GridWorldEnvironment
 * 
 * Neural implementation of simple grid-world environment.
 * 
 * This class implements a simple grid-world environment for testing
 * purposes. It is essential for:
 * - **Grid-world simulation**: Simulates grid-world environments
 * - **Testing**: Provides a testing environment
 * - **Simulation**: Simulates agent-environment interaction
 * - **Environment development**: Develops environment interfaces
 * 
 * Grid-world characteristics in NLM:
 * - **Grid-world simulation**: Simulates grid-world environments
 * - **Testing**: Provides testing environments
 * - **Simulation**: Simulates agent-environment interaction
 * - **Development**: Develops environment interfaces
 * 
 * @note Placeholder implementation for Phase 2
 * @see Environment - For environment interface
 * @see Action - For action execution
 * @see SensoryInput - For observation processing
 * 
 * @author NLM Development Team
 */
class GridWorldEnvironment : public Environment {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default grid-world environment instance with default parameters.
     * The grid-world environment is initialized with default settings for
     * grid-world simulation.
     */
    GridWorldEnvironment(int width = 10, int height = 10);
    
    /**
     * @brief Destructor
     * 
     * Cleans up grid-world environment resources and performs any necessary
     * cleanup operations. The grid-world environment is left in a valid but
     * non-operational state after destruction.
     */
    ~GridWorldEnvironment() override;
    
    /**
     * @brief Reset environment to initial state
     * 
     * Resets the grid-world environment to its initial state. This is used for:
     * - Environment reset
     * - Episode initialization
     * - State reset
     * - Environment management
     */
    std::unique_ptr<SensoryInput> reset() override;
    
    /**
     * @brief Take action and get observation
     * 
     * Takes an action and gets the observation from the grid-world environment.
     * This is used for:
     * - Action processing
     * - Observation generation
     * - Environment interaction
     * - Action processing
     */
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    
    /**
     * @brief Get current state
     * 
     * Returns the current state of the grid-world environment. This is used for:
     * - State observation
     * - State analysis
     * - State processing
     * - State management
     */
    const EnvironmentState& getState() const override;
    
    /**
     * @brief Get reward for last action
     * 
     * Returns the reward for the last action in the grid-world environment.
     * This is used for:
     * - Reward tracking
     * - Reward management
     * - Reward analysis
     * - Reward processing
     */
    float getLastReward() const override;
    
    /**
     * @brief Check if episode is done
     * 
     * Checks if the episode is done in the grid-world environment.
     * This is used for:
     * - Episode management
     * - Episode termination
     * - Episode control
     * - Episode processing
     */
    bool isDone() const override;
    
    /**
     * @brief Get environment name
     * 
     * Returns the name of the grid-world environment. This is used for:
     * - Environment identification
     * - Environment naming
     * - Environment description
     * - Environment analysis
     */
    const char* getName() const override;
    
    /**
     * @brief Get action space size
     * 
     * Returns the size of the action space in the grid-world environment.
     * This is used for:
     * - Action space analysis
     * - Action space management
     * - Action space processing
     * - Action space control
     */
    size_t getActionSpaceSize() const override;
    
    /**
     * @brief Get observation space size
     * 
     * Returns the size of the observation space in the grid-world environment.
     * This is used for:
     * - Observation space analysis
     * - Observation space management
     * - Observation space processing
     * - Observation space control
     */
    size_t getObservationSpaceSize() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
