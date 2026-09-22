#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

/**
 * @class Reward
 * @brief Reward signal system for reinforcement learning and value computation
 * 
 * The Reward class implements the agent's reward processing system, managing
 * reward accumulation, computation, and history tracking. It serves as the
 * interface between environmental rewards and the agent's learning systems,
 * providing both immediate reward values and historical reward patterns.
 * 
 * Key functions of the Reward system:
 * - Accumulates reward signals from environmental interactions
 * - Provides immediate reward values for action selection
 * - Maintains reward history for learning and analysis
 * - Supports reward computation from environmental states
 * - Facilitates reward prediction error calculations for learning
 * 
 * The reward system is essential for:
 * - Reinforcement learning through reward prediction errors
 * - Value-based decision making
 * - Motivational state modulation (dopamine levels)
 * - Performance evaluation and optimization
 * 
 * @note This class is part of Phase 1 implementation. Phase 2 will implement
 *       biologically-realistic reward computation based on environmental
 *       states, action outcomes, and learned value functions.
 * 
 * @see Neuromodulator
 * @see Dopamine
 * @see PredictionError
 * @see Observation
 * @see AgentBrain
 * @see applyRewardModulation()
 */
class Reward {
public:
    /**
     * @brief Constructor: Creates a new Reward system instance
     * 
     * Initializes the reward system with zero accumulated reward and
     * prepares it for processing environmental reward signals. The system
     * starts with an empty reward history and zero current value.
     * 
     * @post Reward system initialized with zero value and empty history
     * @note Default construction sets up an empty reward system ready for
     *       processing environmental rewards. Rewards can be added via the
     *       add() method or set directly via setValue().
     * @see Reward()
     * @see add(float)
     * @see getValue()
     */
    Reward();
    
    /**
     * @brief Destructor: Cleans up the Reward system resources
     * 
     * Performs final cleanup of the reward system's internal state
     * and any allocated resources.
     * 
     * @post All reward system resources are released
     * @note Virtual destructor ensures proper cleanup through base class pointers.
     */
    ~Reward();
    
    /**
     * @brief Get the current accumulated reward value
     * 
     * Returns the current total reward value accumulated by the agent
     * from environmental interactions. This value represents the agent's
     * overall reward history up to the current point in time.
     * 
     * @return float Current accumulated reward value
     * @post None
     * @note This value is continuously updated as the agent interacts with
     *       the environment and receives rewards. It can be reset using the
     *       reset() method.
     * @see setValue(float)
     * @see add(float)
     * @see reset()
     */
    float getValue() const;
    
    /**
     * @brief Set the current accumulated reward value directly
     * 
     * Directly sets the accumulated reward value, typically used for
     * testing, simulation, or controlled experiments. In natural operation,
     * reward values are accumulated over time through interactions with
     * the environment.
     * 
     * @param value New accumulated reward value
     * @post Reward value is set to the specified value
     * @note This method overrides natural reward accumulation. Use with caution
     *       as it may bypass important environmental feedback mechanisms.
     * @see getValue()
     * @see add(float)
     */
    void setValue(float value);
    
    /**
     * @brief Add a reward delta to the accumulated reward
     * 
     * Accumulates an additional reward amount to the current total. This
     * is the primary method for processing environmental rewards as they
     * occur during agent-environment interactions.
     * 
     * @param delta Reward amount to add (can be positive for gains or negative for losses)
     * @post Accumulated reward is increased by the specified delta
     * @note Positive delta values represent rewards (pleasurable outcomes),
     *       negative delta values represent punishments (aversive outcomes).
     *       This method maintains the running total of all environmental rewards.
     * @see getValue()
     * @see setValue(float)
     * @see reset()
     */
    void add(float delta);
    
    /**
     * @brief Reset the reward system to initial state
     * 
     * Resets all internal state variables and accumulated reward to zero.
     * This prepares the reward system for a new episode or testing scenario,
     * clearing the memory of all previous reward history.
     * 
     * @post All accumulated reward and history data cleared, value reset to zero
     * @note This method is useful for starting fresh learning episodes or
     *       for testing purposes. Configuration parameters are preserved
     *       across resets.
     * @see Reward()
     * @see getValue()
     * @see add(float)
     */
    void reset();
    
    /**
     * @brief Compute reward from an environmental observation
     * 
     * Computes a reward value based on the current environmental observation
     * and the agent's learned value functions. This method implements the
     * reward function that maps states and actions to expected rewards.
     * 
     * @param observation Reference to the current environmental observation
     *                    containing state information for reward computation
     * @return float Computed reward value based on observation
     * @post Reward value is computed based on observation
     * @note This is the core reward computation method that implements the
     *       agent's learned reward function. In Phase 1, this is a placeholder
     *       that returns zero. Phase 2 will implement biologically-realistic
     *       reward computation based on action outcomes, prediction errors,
     *       and learned values.
     * @see Reward()
     * @see add(float)
     * @see computeReward()
     */
    float computeReward(const class Observation& observation) const;
    
    /**
     * @brief Get the complete reward history
     * 
     * Returns a reference to the internal reward history vector containing
     * all reward values that have been accumulated over time. This provides
     * access to the temporal sequence of rewards for analysis and learning.
     * 
     * @return const std::vector<float>& Reference to the reward history vector
     * @post None
     * @note The history vector contains all reward values added since the
     *       last reset(). It can be used for statistical analysis, pattern
     *       detection, and reward prediction error calculations.
     * @see clearHistory()
     * @see add(float)
     * @see reset()
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear the reward history vector
     * 
     * Clears all historical reward data from the internal history vector.
     * This removes all previously recorded reward values while preserving
     * the current accumulated reward value.
     * 
     * @post Reward history vector is emptied, all historical data removed
     * @note This method is useful for memory management or when historical
     *       data is no longer needed. The current accumulated reward value
     *       remains unchanged.
     * @see getHistory()
     * @see reset()
     */
    void clearHistory();
    
private:
    /**
     * @brief Implementation details for the Reward system
     * 
     * Pimpl idiom implementation details for the Reward class. This structure
     * contains all the internal implementation details including:
     * - Accumulated reward value storage
     * - Historical reward data storage
     * - Reward computation buffers and temporary variables
     * - Internal state management for reward processing
     * 
     * @note Using the Pimpl idiom keeps the header file clean and allows
     *       for easier implementation changes without affecting the interface.
     * @see Reward()
     * @see ~Reward()
     * @see getValue()
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation details structure
     * 
     * Holds a pointer to the actual implementation of the Reward system.
     * This is part of the Pimpl idiom pattern used to separate interface from
     * implementation and reduce compilation dependencies.
     * 
     * @note The pointer is managed by the implementation to ensure proper
     *       lifecycle management. Users should not access this directly.
     * @see Impl
     */
    Impl* pImpl;
};

} // namespace nlm
