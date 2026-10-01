#pragma once

#include "Types.hpp"

namespace nlm {

/**
 * @brief Manages time in neural network simulations
 * 
 * Provides deterministic time tracking for neural network simulations,
 * separate from real-world (wall-clock) time. The simulation clock allows
 * precise control over simulation timing and enables real-time simulation
 * modes where simulation time advances in sync with wall-clock time.
 * 
 * Key features:
 * - Separate simulation time from real time
 * - Configurable timestep duration
 * - Step-based time management
 * - Real-time synchronization capability
 * - Deterministic time tracking for reproducible simulations
 * 
 * @note Uses internal Pimpl idiom for efficient compilation
 * @see Types Types.hpp for type definitions
 * @see SimulationStep Step counter type
 * @see Timestamp Time value type
 * @see TimestepDuration Time step duration type
 * 
 * Example usage:
 * @code
 * SimulationClock clock(0.001); // 1ms timestep
 * clock.advance(); // Move to next timestep
 * Timestamp currentTime = clock.getTime();
 * @endcode
 */
class SimulationClock {
public:
    /**
     * @brief Constructor with configurable timestep
     * 
     * Creates a new simulation clock with the specified timestep duration.
     * The clock starts at step 0 and time 0.0.
     * 
     * @param timestep Duration of each simulation timestep (defaults to 0.001 seconds/1ms)
     * @throws std::invalid_argument if timestep is less than or equal to zero
     * 
     * @note Common timestep values:
     * - 0.001 (1ms) for real-time simulation
     * - 0.01 (10ms) for faster simulation
     * - 0.1 (100ms) for accelerated simulation
     */
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    /**
     * @brief Destructor
     */
    ~SimulationClock();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * SimulationClock objects cannot be copied to prevent resource management issues
     * and maintain time consistency.
     */
    SimulationClock(const SimulationClock&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    SimulationClock& operator=(const SimulationClock&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of internal resources from another SimulationClock object.
     * The source object becomes unusable after the move operation.
     * 
     * @param other The SimulationClock object to move from
     */
    SimulationClock(SimulationClock&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of internal resources from another SimulationClock object.
     * The source object becomes unusable after the move operation.
     * 
     * @param other The SimulationClock object to move from
     * @return Reference to this SimulationClock object
     */
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    /**
     * @brief Reset the simulation clock to initial state
     * 
     * Resets the simulation clock to step 0 and time 0.0. This is useful for
     * restarting simulations or running multiple simulation scenarios with
     * the same clock configuration.
     * 
     * @note Does not affect timestep configuration or real-time state
     * 
     * @see getStep() Get current step count
     * @see getTime() Get current simulation time
     */
    void reset();
    
    /**
     * @brief Advance the simulation by one timestep
     * 
     * Increments the step counter and advances simulation time by the current
     * timestep duration. This is the primary operation for driving the simulation.
     * 
     * @note If in real-time mode, this may block to maintain sync with wall-clock time
     * 
     * @see advanceBy() Advance by multiple timesteps
     * @see getStep() Get current step count
     * @see getTime() Get current simulation time
     * 
     * @throws std::runtime_error if real-time mode would be violated
     */
    void advance();
    
    /**
     * @brief Get the current simulation step number
     * 
     * Returns the zero-based step counter indicating how many timesteps
     * have been advanced since the last reset. Useful for tracking simulation
     * progress and synchronizing with other simulation components.
     * 
     * @return SimulationStep Current step number (0 for initial state)
     * 
     * @see reset() Reset step counter
     * @see advance() Advance to next step
     */
    SimulationStep getStep() const;
    
    /**
     * @brief Set the current simulation step
     * 
     * Sets the step counter to the specified value. This can be useful for:
     * - Setting a specific simulation state
     * - Jump testing (set step, run simulation)
     * - Reproducing simulation states from saved checkpoints
     * 
     * @param step The step number to set (must be non-negative)
     * @throws std::invalid_argument if step is negative
     * 
     * @note Does not affect simulation time unless timeToStep() is used
     * 
     * @see getStep() Get current step count
     */
    void setStep(SimulationStep step);
    
    /**
     * @brief Get the current simulation time
     * 
     * Returns the current simulation time in seconds. The time value is
     * calculated as step_number × timestep, starting from 0.0 for step 0.
     * 
     * @return Timestamp Current simulation time in seconds
     * 
     * @note Always consistent with getStep() and getTimestep()
     * 
     * @see getStep() Get current step count
     * @see getTimestep() Get timestep duration
     * @see stepToTime() Convert step number to time
     */
    Timestamp getTime() const;
    
    /**
     * @brief Set the current simulation time
     * 
     * Sets the simulation time to the specified value. The step counter
     * is automatically adjusted to maintain consistency with the timestep.
     * 
     * @param time The simulation time in seconds to set
     * @throws std::invalid_argument if time is negative
     * 
     * @note Step count is calculated as floor(time / timestep)
     * 
     * @see getTime() Get current simulation time
     * @see timeToStep() Convert time to step number
     */
    void setTime(Timestamp time);
    
    /**
     * @brief Get the timestep duration
     * 
     * Returns the duration of each simulation timestep. This value is
     * constant throughout the lifetime of the clock and is specified
     * in the constructor.
     * 
     * @return TimestepDuration The duration of each timestep in seconds
     * 
     * @note Smaller timesteps provide higher temporal resolution but
     *       increase computational overhead
     */
    TimestepDuration getTimestep() const;
    
    /**
     * @brief Set the timestep duration
     * 
     * Changes the duration of each simulation timestep. This affects
     * how simulation time advances and the relationship between steps
     * and time values.
     * 
     * @param dt The new timestep duration in seconds
     * @throws std::invalid_argument if dt is less than or equal to zero
     * 
     * @note Does not affect current step or time values
     *       Subsequent timestep advances use the new duration
     */
    void setTimestep(TimestepDuration dt);
    
    /**
     * @brief Check if currently at the first simulation step
     * 
     * Determines whether the simulation is at step 0 (the initial state).
     * Useful for initialization logic and conditional execution based
     * on simulation progress.
     * 
     * @return bool true if at step 0, false otherwise
     * 
     * @see reset() Reset to first step
     * @see getStep() Get current step count
     */
    bool isFirstStep() const;
    
    /**
     * @brief Check if at the last step of a simulation
     * 
     * Determines whether the current step is the last step in a simulation
     * with the specified total number of steps. Useful for termination
     * conditions and progress reporting.
     * 
     * @param totalSteps The total number of steps in the simulation
     * @return bool true if current step is the last step (totalSteps - 1), false otherwise
     * @throws std::invalid_argument if totalSteps is less than 1
     * 
     * @see getStep() Get current step count
     * @see isFirstStep() Check if at first step
     */
    bool isLastStep(SimulationStep totalSteps) const;
    
    /**
     * @brief Convert step number to simulation time
     * 
     * Performs a direct calculation of simulation time for a given step number,
     * without modifying the clock's state. This is useful for:
     * - Computing time at specific steps
     * - Synchronizing with other simulation components
     * - Analyzing temporal patterns in simulation data
     * 
     * @param step The step number to convert
     * @return Timestamp Simulation time corresponding to the step
     * @throws std::invalid_argument if step is negative
     * 
     * @note Formula: time = step × timestep
     */
    Timestamp stepToTime(SimulationStep step) const;
    
    /**
     * @brief Convert simulation time to step number
     * 
     * Performs a direct calculation of step number for a given simulation time,
     * without modifying the clock's state. Uses floor division to ensure
     * each timestep maps to a single step.
     * 
     * @param time The simulation time to convert
     * @return SimulationStep Step number corresponding to the time
     * @throws std::invalid_argument if time is negative
     * 
     * @note Formula: step = floor(time / timestep)
     */
    SimulationStep timeToStep(Timestamp time) const;
    
    /**
     * @brief Advance the simulation by multiple timesteps
     * 
     * Efficiently advances the simulation by the specified number of timesteps.
     * This is more efficient than calling advance() multiple times for large
     * timestep increments.
     * 
     * @param steps Number of timesteps to advance (must be non-negative)
     * @throws std::invalid_argument if steps is negative
     * 
     * @note If in real-time mode, may block to maintain sync with wall-clock time
     *       The total elapsed time will be steps × timestep
     */
    void advanceBy(SimulationStep steps);
    
    /**
     * @brief Get the simulation time at a future step
     * 
     * Computes the simulation time that will be reached after advancing
     * by the specified number of steps from the current position.
     * 
     * @param stepsAhead Number of steps to advance from now (must be non-negative)
     * @return Timestamp Future simulation time
     * @throws std::invalid_argument if stepsAhead is negative
     * 
     * @note Formula: time = current_time + steps_ahead × timestep
     */
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    /**
     * @brief Get the elapsed real-time since simulation started
     * 
     * Returns the wall-clock time that has passed since the simulation clock
     * was started in real-time mode. This is useful for:
     * - Monitoring real-time performance
     * - Implementing real-time mode
     * - Analyzing computational speed
     * 
     * @return double Elapsed real-time in seconds (0.0 if not in real-time mode)
     * 
     * @note Only meaningful if in real-time mode (setRealTimeStart() called)
     */
    double getRealTimeElapsed() const;
    
    /**
     * @brief Start real-time tracking
     * 
     * Begins tracking elapsed real-time from this point forward. After calling
     * this, getRealTimeElapsed() will return the wall-clock time elapsed since
     * this call, and advance() will respect real-time timing constraints.
     * 
     * @note Real-time mode affects the behavior of advance() to maintain
     *       synchronization with wall-clock time
     */
    void setRealTimeStart();
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the SimulationClock class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
