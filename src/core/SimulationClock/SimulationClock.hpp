#pragma once

#include "Types.hpp"

namespace nlm {

/**
 * @file SimulationClock.hpp
 * 
 * Deterministic simulation clock for neural system time management
 * 
 * This file provides a deterministic simulation clock that manages time
 * for neural simulations separately from real-time wall clock time. The
 * SimulationClock is a core component of the NLM system, providing
 * consistent and reproducible time tracking for all simulation steps
 * and neural computations.
 * 
 * Key features:
 * - Deterministic time progression independent of real-time
 * - Multiple time representations (step count, timestamp, timestep)
 * - Conversion utilities between time representations
 * - Support for both discrete and continuous simulation modes
 * - Real-time wall clock comparison for real-time mode
 * - Thread-safe operations for concurrent access
 * - Configurable timestep for different simulation granularities
 * 
 * @note Critical for reproducibility in neural simulations
 * @note All time values are stored internally as double-precision floats
 * @note Time is measured in milliseconds for consistency with other systems
 * @note Used extensively by Brain, NeuralRegion, and other simulation components
 * 
 * @see Config.hpp - Configuration options for simulation parameters
 * @see Brain.hpp - Integration with brain simulation
 * @see Types.hpp - Time-related type definitions
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class SimulationClock
 * 
 * Deterministic simulation clock for managing neural simulation time.
 * 
 * The SimulationClock provides a consistent time base for all neural
 * computations in the NLM system. Unlike wall-clock time, simulation
 * time is deterministic and reproducible, making it essential for
 * scientific experiments and debugging.
 * 
 * The clock maintains time in three related representations:
 * - Step count: Integer step number since simulation start
 * - Timestamp: Current simulation time in milliseconds
 * - Timestep: Duration of each simulation step in milliseconds
 * 
 * These representations are mathematically related:
 *   time = step × timestep
 *   step = time / timestep
 *   timestep = time / step
 * 
 * @note Used by all neural components to ensure temporal consistency
 * @note Critical for synaptic plasticity timing and neuromodulation
 * @note Essential for reproducible experiments
 * 
 * @see Types.hpp - Time type definitions
 * @see Brain.hpp - Integration with brain simulation
 * @see Config.hpp - Simulation timestep configuration
 * 
 * @author NLM Development Team
 */
class SimulationClock {
public:
    /**
     * @brief Constructor with custom timestep
     * 
     * Creates a new simulation clock with a specified timestep.
     * The timestep determines the duration of each simulation step
     * and affects the relationship between simulation steps and
     * simulation time.
     * 
     * @param timestep Duration of each simulation step in milliseconds
     *                 (default: 0.001 ms = 1 microsecond)
     * 
     * @pre timestep must be positive
     * @post Simulation time starts at 0.0
     * @post Step counter starts at 0
     * @post Time conversion functions are ready for use
     * 
     * @note Typical timestep values:
     *       - 0.001 ms: High-resolution neural dynamics
     *       - 1.0 ms: Biological timescale
     *       - 10.0 ms: Coarser integration
     * 
     * @see getTimestep() - For retrieving current timestep
     * @see TimestepDuration - Type definition for timestep
     */
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    /**
     * @brief Destructor
     * 
     * Cleans up simulation clock resources and performs any
     * necessary cleanup operations.
     */
    ~SimulationClock();
    
    // Disable copying, enable moving
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    /**
     * @brief Reset simulation clock to initial state
     * 
     * Resets the simulation clock to its initial state as if the
     * simulation were just starting. This is useful for:
     * - Experiment restarts
     * - State restoration
     * - Debugging sessions
     * - Testing scenarios
     * 
     * @post Simulation time set to 0.0
     * @post Step counter set to 0
     * @post Real-time elapsed reset to 0.0
     * @post All internal state reset
     * 
     * @see setStep() - For partial step resetting
     * @see setTime() - For time-only resetting
     * @see reset() - Main reset function
     */
    void reset();
    
    /**
     * @brief Advance simulation clock by one timestep
     * 
     * Advances the simulation clock by exactly one timestep,
     * updating both the step counter and simulation time accordingly.
     * This is the primary function used by the main simulation loop.
     * 
     * @post Step counter incremented by 1
     * @post Simulation time increased by timestep
     * @post Real-time elapsed updated if in real-time mode
     * 
     * @note Called once per simulation step
     * @see advanceBy() - For advancing multiple steps
     * @see step() - Alias for advance()
     */
    void advance();
    
    /**
     * @brief Get current simulation step count
     * 
     * Retrieves the current simulation step count. Steps are integer
     * increments of simulation time, useful for discrete event
     * handling and state restoration.
     * 
     * @return Current simulation step count
     * 
     * @post Return value is stable during simulation
     * @complexity O(1)
     * 
     * @note Step count starts at 0
     * @see getTime() - For current simulation time
     * @see setStep() - For setting step count
     */
    SimulationStep getStep() const;
    
    /**
     * @brief Set simulation step count
     * 
     * Directly sets the simulation step count and adjusts simulation
     * time accordingly. This is a low-level operation typically used
     * for state restoration or testing.
     * 
     * @param step New simulation step count
     * 
     * @pre step must be non-negative
     * @post Simulation time adjusted to step * timestep
     * @post Real-time state may need to be updated
     * 
     * @note May invalidate real-time synchronization
     * @see getStep() - For getting current step
     * @see reset() - For complete reset
     */
    void setStep(SimulationStep step);
    
    /**
     * @brief Get current simulation time
     * 
     * Retrieves the current simulation time in milliseconds. Simulation
     * time represents the cumulative duration of the simulation from
     * its start point, independent of wall-clock time.
     * 
     * @return Current simulation time in milliseconds
     * 
     * @post Return value is stable during simulation
     * @complexity O(1)
     * 
     * @note Time starts at 0.0
     * @note Time increases monotonically
     * @see getStep() - For step count
     * @see setTime() - For setting time
     */
    Timestamp getTime() const;
    
    /**
     * @brief Set simulation time directly
     * 
     * Directly sets the simulation time, adjusting the step count
     * accordingly. This is useful for jumping to specific points
     * in the simulation or for testing scenarios.
     * 
     * @param time New simulation time in milliseconds
     * 
     * @pre time must be non-negative
     * @post Step count adjusted: step = floor(time / timestep)
     * @post Real-time state may need to be updated
     * 
     * @note May cause fractional step count
     * @see getTime() - For getting current time
     * @see stepToTime() - For time to step conversion
     */
    void setTime(Timestamp time);
    
    /**
     * @brief Get simulation timestep
     * 
     * Retrieves the current timestep, which determines the duration
     * of each simulation step. This is a constant value for a given
     * simulation clock instance.
     * 
     * @return Current simulation timestep in milliseconds
     * 
     * @post Return value is constant for the clock instance
     * @complexity O(1)
     * 
     * @see setTimestep() - For setting timestep
     * @see getTime() - For time calculation
     * @see TimestepDuration - Type definition
     */
    TimestepDuration getTimestep() const;
    
    /**
     * @brief Set simulation timestep
     * 
     * Changes the simulation timestep, which affects the relationship
     * between simulation steps and simulation time. This is a
     * low-level operation that requires careful consideration as it
     * affects all time calculations.
     * 
     * @param dt New timestep in milliseconds
     * 
     * @pre dt must be positive
     * @post Step count recalculated: step = floor(time / dt)
     * @post Time conversion functions updated
     * 
     * @note Changing timestep may affect simulation reproducibility
     * @see getTimestep() - For getting current timestep
     * @see getTime() - For time calculations
     */
    void setTimestep(TimestepDuration dt);
    
    /**
     * @brief Check if this is the first simulation step
     * 
     * Determines whether the current step is the first step (step 0)
     * of the simulation. This is useful for initialization code that
     * should only run once.
     * 
     * @return true if current step is 0, false otherwise
     * 
     * @complexity O(1)
     * 
     * @note Used for one-time initialization
     * @see isLastStep() - For checking last step
     */
    bool isFirstStep() const;
    
    /**
     * @brief Check if this is the last simulation step
     * 
     * Determines whether the current step is the last step in a
     * finite-duration simulation. This is useful for cleanup code
     * that should run only at the simulation end.
     * 
     * @param totalSteps Total number of steps in the simulation
     * @return true if current step == totalSteps - 1, false otherwise
     * 
     * @pre totalSteps must be positive
     * @complexity O(1)
     * 
     * @note Used for simulation termination
     * @see isFirstStep() - For checking first step
     * @see advanceBy() - For advancing multiple steps
     */
    bool isLastStep(SimulationStep totalSteps) const;
    
    /**
     * @brief Convert simulation step to simulation time
     * 
     * Converts a simulation step number to the corresponding
     * simulation time in milliseconds. This is a linear conversion:
     * time = step × timestep
     * 
     * @param step Simulation step number
     * @return Simulation time corresponding to the step
     * 
     * @post time = step × timestep (with appropriate rounding)
     * @complexity O(1)
     * 
     * @note Useful for examining specific points in simulation
     * @see timeToStep() - For inverse conversion
     * @see getTime() - For current time
     */
    Timestamp stepToTime(SimulationStep step) const;
    
    /**
     * @brief Convert simulation time to simulation step
     * 
     * Converts simulation time in milliseconds to the corresponding
     * simulation step number. This is a linear conversion with
     * appropriate rounding for discrete steps.
     * 
     * @param time Simulation time in milliseconds
     * @return Simulation step corresponding to the time
     * 
     * @post step = floor(time / timestep)
     * @complexity O(1)
     
     * @note Uses floor division for discrete steps
     * @see stepToTime() - For inverse conversion
     * @see setTime() - For time setting
     */
    SimulationStep timeToStep(Timestamp time) const;
    
    /**
     * @brief Advance simulation by multiple steps
     * 
     * Advances the simulation clock by multiple steps, updating both
     * the step count and simulation time accordingly. This is useful
     * for skipping ahead in the simulation or implementing
     * faster-than-real-time mode.
     * 
     * @param steps Number of steps to advance (must be positive)
     * 
     * @pre steps must be positive
     * @post Step count increased by steps
     * @post Simulation time increased by steps × timestep
     * @post Real-time elapsed updated if in real-time mode
     * 
     * @see advance() - For single step advancement
     * @see getStep() - For current step
     * @see isLastStep() - For checking termination
     */
    void advanceBy(SimulationStep steps);
    
    /**
     * @brief Get time at a future step
     * 
     * Calculates the simulation time that would be reached if the
     * simulation were advanced by the specified number of steps.
     * This is useful for planning and visualization without
     * actually advancing the simulation.
     * 
     * @param stepsAhead Number of steps ahead to calculate time for
     * @return Simulation time after advancing stepsAhead steps
     * 
     * @post time = current_time + stepsAhead × timestep
     * @complexity O(1)
     * 
     * @see getTime() - For current time
     * @see timeAtFutureStep() - Alias for this function
     */
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    /**
     * @brief Get real-time elapsed since simulation start
     * 
     * Retrieves the amount of real-time (wall-clock time) that has
     * elapsed since the simulation started. This is useful for:
     * - Real-time mode implementation
     * - Performance monitoring
     * - Synchronization with external systems
     * - Debugging timing issues
     * 
     * @return Real-time elapsed in seconds
     * 
     * @post Return value increases monotonically
     * @complexity O(1)
     * 
     * @note Returns 0.0 if real-time mode is not active
     * @see setRealTimeStart() - For starting real-time mode
     * @see getTime() - For simulation time
     */
    double getRealTimeElapsed() const;
    
    /**
     * @brief Start real-time mode tracking
     * 
     * Starts tracking real-time wall clock time for synchronization
     * with simulation progression. In real-time mode, the simulation
     * runs at a rate that matches real-time (e.g., 1 second of
     * simulation time per second of real-time).
     * 
     * @post Real-time tracking initialized
     * @post getRealTimeElapsed() will return meaningful values
     * 
     * @note Use with advance() to implement real-time simulation
     * @see getRealTimeElapsed() - For elapsed real-time
     * @see advance() - For simulation stepping
     */
    void setRealTimeStart();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
