#pragma once

/**
 * @file SimulationClock.hpp
 * @brief Simulation clock for managing time in the neural simulation
 * 
 * This file defines the SimulationClock class, which provides deterministic
 * time tracking separate from wall-clock time. The simulation clock manages
 * both discrete simulation steps and continuous time, ensuring accurate
 * temporal control of neural dynamics.
 * 
 * @author NLM Research Team
 * @date Phase 1 - Core Infrastructure
 * @version 1.0
 * 
 * @remarks This clock is essential for:
 * - Maintaining consistent timestep across all simulation components
 * - Converting between discrete steps and continuous time
 * - Real-time simulation mode control
 * - Ensuring reproducible experiments through deterministic timing
 */

#include "../Types/Types.hpp"

namespace nlm {

/**
 * @class SimulationClock
 * @brief Manages deterministic time tracking for neural simulations
 * 
 * The SimulationClock provides two complementary time representations:
 * discrete simulation steps (for efficient computation) and continuous
 * simulation time (for accurate temporal dynamics). It ensures that all
 * neural processes operate on a consistent temporal framework.
 * 
 * @details The clock serves as the temporal backbone of the NLM brain
 * simulation, coordinating:
 * - Step-based processing of spiking neurons
 * - Time-based calculations of synaptic transmission
 * - Real-time wall-clock synchronization (when enabled)
 * - Conversion between different time representations
 * 
 * @note The clock is designed to be independent of system wall-clock time,
 *       making experiments reproducible regardless of external factors like
 *       CPU frequency scaling or system load.
 * 
 * @warning All operations are thread-safe and can be called from multiple
 *         contexts during simulation execution.
 */
class SimulationClock {
public:
    /**
     * @brief Construct simulation clock with specific timestep
     * 
     * @param timestep Default timestep in seconds for simulation steps
     *                 (default: 0.001 = 1 millisecond)
     * @details The timestep determines the temporal resolution of the
     * simulation. Smaller timesteps provide more accurate integration of
     * neural dynamics but require more computational resources. Common
     * values:
     * - 0.001: Standard biological timescale (1ms)
     * - 0.01: Faster simulation, coarser resolution
     * - 0.1: Very fast simulation
     */
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    /**
     * @brief Destructor for cleanup
     * 
     * Properly deallocates the internal implementation (pImpl pattern)
     */
    ~SimulationClock();
    
    /**
     * @brief Disable copying to prevent multiple ownership of clock state
     * 
     * The simulation clock maintains internal state that should not be shared
     * between multiple objects. Copying would lead to inconsistent time
     * tracking and unpredictable behavior.
     */
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    
    /**
     * @brief Enable move construction for efficient transfer of ownership
     * 
     * Move construction allows the SimulationClock to be efficiently transferred
     * from one object to another, with proper handling of the internal pImpl
     * pointer to prevent double deletion.
     * 
     * @param other The SimulationClock to move from (will be in valid but
     *              unspecified state after move)
     */
    SimulationClock(SimulationClock&&) noexcept;
    
    /**
     * @brief Enable move assignment for efficient transfer of ownership
     * 
     * @param other The SimulationClock to move from
     * @return Reference to this SimulationClock after assignment
     * @note Properly handles self-assignment and cleans up existing resources
     */
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    /**
     * @brief Reset the simulation clock to initial state
     * 
     * This resets both the step counter and simulation time to their initial
     * values (0). Use this to start a new simulation or experiment from
     * scratch.
     * 
     * @remarks After reset, getStep() returns 0 and getTime() returns 0.0
     */
    void reset();
    
    /**
     * @brief Advance the simulation by one timestep
     * 
     * This method advances the simulation clock by exactly one timestep,
     * incrementing both the step counter and simulation time accordingly.
     * It should be called once per neural simulation step.
     * 
     * @remarks After calling advance():
     * - getStep() increments by 1
     * - getTime() increases by timestep
     * - The clock is now in a valid state for processing
     */
    void advance();
    
    /**
     * @brief Get the current simulation step number
     * 
     * @return The current simulation step (starts at 0, increments each
     *         call to advance())
     * @details The simulation step is a discrete integer value that serves
     * as the primary index for time-based arrays and operations. Each step
     * represents one complete cycle of neural processing.
     * 
     * @note This value is useful for:
     * - Indexing time-dependent arrays
     * - Controlling simulation loops
     * - Debugging and logging
     */
    SimulationStep getStep() const;
    
    /**
     * @brief Set the simulation step number directly
     * 
     * @param step The simulation step to set (must be non-negative)
     * @warning This bypasses the normal advance() mechanism and should be
     *         used with caution. Only set step values that make sense
     *         for the current simulation state.
     */
    void setStep(SimulationStep step);
    
    /**
     * @brief Get the current simulation time in seconds
     * 
     * @return The current simulation time in seconds (floating-point)
     * @details The simulation time is a continuous value that represents
     * the actual elapsed time in the neural simulation. This is useful for:
     * - Time-based calculations
     * - Real-time synchronization
     * - Temporal analysis of neural activity
     * 
     * @note This value increases continuously as the simulation progresses,
     *       even though neural events occur at discrete step boundaries.
     */
    Timestamp getTime() const;
    
    /**
     * @brief Set the simulation time directly
     * 
     * @param time The simulation time to set in seconds
     * @warning This bypasses the normal temporal progression and should be
     *         used only for specific use cases like loading saved states
     *         or testing specific time points.
     */
    void setTime(Timestamp time);
    
    /**
     * @brief Get the configured timestep duration
     * 
     * @return The timestep in seconds that defines each simulation step
     * @details The timestep is the fundamental time unit of the simulation.
     * It determines the temporal resolution and affects both accuracy and
     * performance. Common values are 0.001 (1ms) for biological accuracy
     * or larger values for faster simulations.
     */
    TimestepDuration getTimestep() const;
    
    /**
     * @brief Set the timestep duration
     * 
     * @param dt The new timestep in seconds
     * @warning Changing the timestep during an active simulation can cause
     *         temporal inconsistencies. It's generally better to set the
     *         timestep during initialization and keep it constant throughout
     *         the simulation.
     */
    void setTimestep(TimestepDuration dt);
    
    /**
     * @brief Check if this is the first simulation step
     * 
     * @return true if getStep() == 0, false otherwise
     * @details Useful for initialization code that should only run once
     * at the beginning of a simulation.
     */
    bool isFirstStep() const;
    
    /**
     * @brief Check if this step would be the last in a fixed-length simulation
     * 
     * @param totalSteps The total number of steps expected for the simulation
     * @return true if this step is the last step (getStep() + 1 == totalSteps)
     * @details Useful for controlling simulation loops that run for a
     * specific number of steps, allowing for proper cleanup or logging
     * at the end of simulation.
     */
    bool isLastStep(SimulationStep totalSteps) const;
    
    /**
     * @brief Convert simulation step to simulation time
     * 
     * @param step The simulation step to convert
     * @return The simulation time in seconds corresponding to the given step
     * @details This linear conversion assumes constant timestep. The formula
     * is: time = step * timestep
     */
    Timestamp stepToTime(SimulationStep step) const;
    
    /**
     * @brief Convert simulation time to simulation step
     * 
     * @param time The simulation time to convert (seconds)
     * @return The nearest simulation step corresponding to the given time
     * @details This conversion uses integer division. The result may lose
     * precision due to floating-point to integer conversion.
     */
    SimulationStep timeToStep(Timestamp time) const;
    
    /**
     * @brief Advance by multiple steps efficiently
     * 
     * @param steps Number of steps to advance (must be non-negative)
     * @details This is more efficient than calling advance() repeatedly for
     * large numbers of steps, as it updates the time values in bulk rather
     * than one step at a time.
     */
    void advanceBy(SimulationStep steps);
    
    /**
     * @brief Get simulation time at a future step from current time
     * 
     * @param stepsAhead Number of steps into the future to calculate time for
     * @return The simulation time that will occur after advancing the specified
     *         number of steps
     * @details Useful for scheduling events or calculating time windows in
     * the future without actually advancing the simulation clock.
     */
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    /**
     * @brief Get elapsed real-time since simulation started
     * 
     * @return Elapsed real time in seconds (wall-clock time)
     * @details This provides access to real-world elapsed time, which is
     * particularly useful for:
     * - Real-time simulation mode control
     * - Performance monitoring
     * - Synchronization with external systems
     * 
     * @note This is only meaningful when real-time mode is enabled via
     *       setRealTimeStart(). Otherwise it returns the time since object
     *       construction or last reset.
     */
    double getRealTimeElapsed() const;
    
    /**
     * @brief Start real-time tracking for synchronized simulation
     * 
     * @details This starts tracking wall-clock time separately from simulation
     * time, enabling real-time simulation modes where neural processes
     * operate at human-perceptible speeds. Call this once at the beginning
     * of simulation if real-time mode is desired.
     * 
     * @note Real-time simulation allows users to observe neural dynamics
     *       in approximately real-time, which is useful for interactive
     *       demonstrations and visualization.
     */
    void setRealTimeStart();
    
private:
    /**
     * @brief Internal implementation pointer (pImpl pattern)
     * 
     * @details The SimulationClock uses the pImpl idiom to hide implementation
     * details and reduce compilation dependencies. All actual implementation
     * is contained in the private .cpp file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
