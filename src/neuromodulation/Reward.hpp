#pragma once

/**
 * @file Reward.hpp
 * @brief Reward signal system for reinforcement learning and behavior
 * 
 * This header defines the Reward class for the brain's reinforcement learning system.
 * The reward system accumulates and computes reward signals that drive learning,
 * motivation, and decision-making processes. It provides a unified interface for
 * handling reward signals across different timescales and computation methods.
 * 
 * Reward Theory:
 * - Reward signals encode the value and prediction of rewarding outcomes
 * - Reinforcement learning relies on accurate reward estimation for learning
 * - Reward systems interact with neuromodulatory systems (dopamine, curiosity)
 * - Reward computation involves temporal discounting and credit assignment
 * - Reward signals guide exploration vs. exploitation trade-offs
 * 
 * Reward Functions:
 * - Reward accumulation: Sums reward over time for stable learning signals
 * - Reward prediction: Estimates expected rewards for decision-making
 * - Reward computation: Computes instantaneous reward from environment state
 * - Reward history: Maintains temporal sequences of reward experiences
 * - Reward decay: Handles temporal discounting for future rewards
 * 
 * Integration Points:
 * - Connected to neuromodulation systems (dopamine, curiosity)
 * - Used by prediction error system for learning
 * - Affects attention and memory systems
 * - Influences planning and decision-making processes
 * - Drives exploration behavior through curiosity modulation
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Reward;
class Brain;
class Neuromodulator;

/**
 * @brief Reward signal accumulator and computation system
 * 
 * Implements the brain's reward system for reinforcement learning. The reward
 * system accumulates reward signals over time, computes reward prediction errors,
 * and provides reward history for learning and decision-making. It serves as a
 * central hub for reward processing and distribution throughout the brain model.
 * 
 * Reward System Architecture:
 * 1. Reward Accumulation: Sums instantaneous rewards into running total
 * 2. Reward History: Maintains temporal sequence of reward experiences
 * 3. Reward Computation: Computes reward from environmental state
 * 4. Reward Modulation: Adjusts reward signals based on context
 * 5. Reward Distribution: Shares reward signals with neuromodulatory systems
 * 
 * Reward Processing Pipeline:
 * 1. Environment -> Reward Computation -> Accumulation -> History
 * 2. History -> Prediction -> Prediction Error -> Neuromodulation
 * 3. Neuromodulation -> Plasticity -> Learning
 * 
 * Reward System Properties:
 * - Accumulation: Integrates reward over time for stable learning
 * - Temporal scaling: Handles rewards across different timescales
 * - History tracking: Maintains reward sequence for pattern discovery
 * - Modulation sensitivity: Responds to context and state changes
 * - Distribution: Shares reward information with learning systems
 * 
 * Reward Dynamics:
 * - Decay: Reward signals decay over time (temporal discounting)
 * - Saturation: Reward signals saturate at maximum levels
 * - Integration: Reward accumulates and integrates over time
 * - Modulation: Reward effectiveness depends on neuromodulatory state
 * 
 * Thread Safety: Reward system is thread-safe for concurrent access
 * from different brain components. Accumulation and computation
 * operations are protected with mutexes.
 * 
 * Memory Management: Reward system uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note Reward system is essential for reinforcement learning and
 *       motivation-driven behavior. Works in conjunction with
 *       neuromodulatory systems for comprehensive reward processing.
 * @see Neuromodulator for neuromodulatory reward processing
 * @see PredictionError for prediction error computation
 * @see Curiosity for exploration drive modulation
 */
class Reward {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a reward system with initial state. The reward implementation
     * uses the pImpl pattern for efficient memory usage and to reduce
     * compilation dependencies.
     */
    Reward();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of Reward's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~Reward();
    
    /**
     * @brief Get current accumulated reward value
     * 
     * Returns the current running total of reward accumulated by the
     * reward system. This represents the net reward signal available
     * for learning and decision-making.
     * 
     * @return Current accumulated reward value
     * 
     * @note Accumulated reward typically ranges from -1.0 to +1.0
     *       for numerical stability in learning algorithms.
     *       Values are updated through add() operations.
     */
    float getValue() const;
    
    /**
     * @brief Set current reward value
     * 
     * Sets the accumulated reward to an exact value. This is typically
     * used for initialization or direct reward manipulation rather
     * than gradual accumulation.
     * 
     * @param value New accumulated reward value
     * 
     * @note This method is less commonly used than add() for gradual
     *       accumulation. Direct setting may bypass temporal dynamics
     *       that are important for realistic reward processing.
     * 
     * @see add()
     * @see computeReward()
     */
    void setValue(float value);
    
    /**
     * @brief Add delta reward to accumulated total
     * 
     * Increments the accumulated reward by the specified delta. This is
     * the primary method for updating reward signals in response to
     * environmental events or outcomes.
     * 
     * Reward Accumulation Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Add delta to current accumulated value
     * 3. Update reward history with new accumulated value
     * 4. Apply any decay or modulation effects
     * 5. Check for saturation or overflow conditions
     * 
     * @param delta Reward change to add (positive for reward, negative for punishment)
     * 
     * @note Thread-safe with mutex protection for concurrent access.
     *       Negative deltas represent punishment or aversive outcomes.
     *       Performance: O(1) for addition operation.
     * 
     * @see getValue()
     * @see getHistory()
     * @see reset()
     */
    void add(float delta);
    
    /**
     * @brief Reset accumulated reward to zero
     * 
     * Clears the accumulated reward total and optionally resets reward history.
     * This is used to start new reward episodes or cycles.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation affects the primary reward signal but not
     *       the underlying reward history used for pattern discovery.
     * 
     * @see add()
     * @see clearHistory()
     * @see getHistory()
     */
    void reset();
    
    /**
     * @brief Compute reward from environment state
     * 
     * Computes the instantaneous reward value based on the current
     * environmental state. This method implements the reward function
     * that maps environmental states to reward values.
     * 
     * Reward Computation Algorithm:
     * 1. Analyze current environmental state
     * 2. Apply reward function based on state characteristics
     * 3. Consider contextual factors (goals, history, neuromodulation)
     * 4. Return computed reward value
     * 
     * Reward Computation Factors:
     * - Primary rewards: Food, water, safety, social interaction
     * - Secondary rewards: Goal achievement, progress, learning
     * - Negative rewards: Threat, punishment, loss
     * - Contextual rewards: Social status, resource availability
     * 
     * @param observation Environmental state for reward computation
     * @return Computed reward value (typically -1.0 to +1.0)
     * 
     * @note This method implements the core reward function that maps
     *       environmental states to reward values. The specific computation
     *       depends on the agent's goals and environmental semantics.
     * 
     * @see add()
     * @see computeReward()
     * @see getHistory()
     */
    float computeReward(const class Observation& observation) const;
    
    /**
     * @brief Get reward history as vector of past values
     * 
     * Returns the complete history of reward values accumulated by the
     * reward system. This provides temporal context for reward processing
     * and is used for pattern discovery and prediction.
     * 
     * @return Vector of past reward values (most recent at end)
     * 
     * @note Reward history is updated with each add() operation.
     *       This provides temporal context for reward processing and
     *       pattern discovery in the reward system.
     * 
     * @see add()
     * @see clearHistory()
     * @see reset()
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear reward history
     * 
     * Removes all past reward values from the history. This is used
     * to reset the temporal memory of the reward system while
     * preserving the current accumulated reward value.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the temporal memory but preserves
     *       the current state of accumulated reward.
     * 
     * @see getHistory()
     * @see add()
     */
    void clearHistory();
    
    /**
     * @brief Get reward magnitude for neuromodulation
     * 
     * Returns the absolute value of the current reward (ignoring sign),
     * representing the strength or salience of the reward signal. This
     * is used by neuromodulatory systems to determine the appropriateness
     * of neuromodulator release.
     * 
     * @return Reward magnitude (absolute value of current reward)
     * 
     * @note Neuromodulators like dopamine respond more strongly to
     *       reward magnitude than to reward sign. This provides a measure
     *       of reward salience independent of reward valence.
     * 
     * @see getValue()
     * @see Neuromodulator
     */
    float getMagnitude() const;
    
    /**
     * @brief Get current developmental stage influence on reward
     * 
     * Returns how the current developmental stage affects reward processing.
     * Different developmental stages have different reward sensitivities
     * and processing characteristics.
     * 
     * @return Developmental stage multiplier for reward processing
     * 
     * @note Developmental stage influences reward sensitivity, valence
     *       processing, and reward-based learning mechanisms.
     *       Younger stages may have different reward priorities.
     * 
     * @see DevelopmentSystem
     * @see getValue()
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Set reward system parameters
     * 
     * Configures internal parameters for reward processing, such as
     * accumulation rate, decay factor, or saturation levels.
     * 
     * @param accumulationRate Rate at which rewards are accumulated
     * @param decayRate Rate at which reward effects decay over time
     * @param saturationLevel Maximum reward value before saturation
     * 
     * @note Thread-safe with mutex protection.
     *       These parameters affect reward processing dynamics and
     *       should be set before reward accumulation begins.
     */
    void setParameters(float accumulationRate, float decayRate, float saturationLevel);
    
    /**
     * @brief Get current reward system parameters
     * 
     * @return Vector containing current reward system parameters
     *         [accumulationRate, decayRate, saturationLevel]
     * 
     * @note Parameters include accumulation dynamics and processing limits.
     */
    std::vector<float> getParameters() const;
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     * 
     * Hidden implementation for Reward system.
     * Stores reward state including accumulated value, history,
     * parameters, and timing information.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm