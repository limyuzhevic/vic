#pragma once

#include "Types.hpp"

namespace nlm {

// Simulation clock for managing time in the neural simulation
//
// The SimulationClock provides deterministic time tracking separate from wall-clock time.
// It is essential for reproducible simulations and synchronized processing across
// all brain components. The clock manages discrete simulation steps and continuous
// time, enabling precise temporal control in neural simulations.
//
// Time Management Principles:
// - Discrete time steps: Simulation progresses in fixed timesteps
// - Continuous time: Real-valued time for precise calculations
// - Deterministic behavior: Same sequence of steps across runs
// - Time conversions: Flexible conversion between step and time representations
// - Real-time mode: Optional synchronization with wall-clock time
//
// Use Cases:
// - Brain simulation: Time control for neural dynamics
// - Event scheduling: Precise timing of spike events and delays
// - Learning algorithms: Temporal credit assignment
// - Checkpointing: Recording and restoring simulation state
// - Performance analysis: Timing measurements and profiling
//
// Usage example:
// @code
// // Create simulation clock with 1ms timestep
// SimulationClock clock(0.001);
// 
// // Initialize simulation
// clock.setStep(0);
// clock.setTime(0.0);
// 
// // Main simulation loop
// while (!simulationComplete) {
//     // Advance one step
//     clock.advance();
//     
//     // Get current time for computations
//     Timestamp currentTime = clock.getTime();
//     SimulationStep currentStep = clock.getStep();
//     
//     // Time-based calculations
//     Timestamp futureTime = clock.timeAtFutureStep(100);  // 100 steps ahead
//     
//     // Real-time synchronization (if enabled)
//     double realTime = clock.getRealTimeElapsed();
//     
//     // Time-step processing (brain, plasticity, etc.)
//     brain->step(currentStep, currentTime);
// }
// @endcode
class SimulationClock {
public:
    /**
     * Constructor - creates simulation clock with specified timestep
     * 
     * Initializes the simulation clock with a fixed timestep that determines
     * the duration of each simulation step. The timestep controls the temporal
     * resolution and total simulation duration.
     * 
     * @param timestep Duration of each simulation step in seconds
     *                (default: 0.001 seconds = 1ms)
     * 
     * @throws May throw exceptions if timestep is invalid
     * 
     * @note Common timestep values:
     *       - 0.001: 1ms (neural time scale)
     *       - 0.01: 10ms (behavioral time scale)
     *       - 0.1: 100ms (cognitive time scale)
     */
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default

    /**
     * Destructor - cleans up clock resources
     */
    ~SimulationClock();

    // Disable copying, enable moving (for performance and memory management)
    SimulationClock(const SimulationClock&) = delete;                     ///< Copy constructor disabled (non-copyable)
    SimulationClock& operator=(const SimulationClock&) = delete;          ///< Copy assignment disabled (non-copyable)
    SimulationClock(SimulationClock&&) noexcept;                          ///< Move constructor
    SimulationClock& operator=(SimulationClock&&) noexcept;               ///< Move assignment

    // Time state management

    /**
     * Reset simulation clock to initial state
     * 
     * Resets the clock to the beginning of simulation:
     * - Step counter set to 0
     * - Simulation time set to 0.0
     * - Real-time tracking reset (if enabled)
     * - Wall-clock time comparison reset
     * 
     * Useful for:
     * - Starting new simulation episodes
     * - Testing and debugging
     * - Reproducible experiment restarts
     */
    void reset();

    /**
     * Advance clock by one timestep
     * 
     * Increments the step counter and updates simulation time:
     * - Simulation step +1
     * - Simulation time + timestep
     * - Real-time tracking updated (if enabled)
     * 
     * This is the main advancement operation called in simulation loops.
     */
    void advance();

    // Step counter management

    /**
     * Get current simulation step
     * 
     * @return Current simulation step number (integer)
     * 
     * @note Step 0 is the initial state before any advancement
     * @see advance()
     * @see setStep()
     */
    SimulationStep getStep() const;

    /**
     * Set simulation step directly
     * 
     * @param step New simulation step number
     * 
     * @note Use with caution - may cause time inconsistency
     * @see advance()
     */
    void setStep(SimulationStep step);

    // Current simulation time

    /**
     * Get current simulation time
     * 
     * @return Current simulation time in seconds (continuous time)
     * 
     * @note Returns step * timestep if time not explicitly set
     * @see getStep()
     * @see setTime()
     */
    Timestamp getTime() const;

    /**
     * Set current simulation time directly
     * 
     * @param time New simulation time in seconds
     * 
     * @note Use with caution - may cause step inconsistency
     * @see advance()
     */
    void setTime(Timestamp time);

    // Timestep management

    /**
     * Get simulation timestep
     * 
     * @return Duration of each simulation step in seconds
     */
    TimestepDuration getTimestep() const;

    /**
     * Set simulation timestep
     * 
     * @param dt New timestep in seconds
     * @throws std::invalid_argument if timestep is invalid (<= 0)
     * 
     * @note Changing timestep affects time calculations
     * @see getTimestep()
     */
    void setTimestep(TimestepDuration dt);

    // Convenience queries

    /**
     * Check if current step is the first step
     * 
     * @return true if step == 0, false otherwise
     */
    bool isFirstStep() const;

    /**
     * Check if current step is the last step for a given total
     * 
     * @param totalSteps Total number of steps in simulation
     * @return true if step == totalSteps - 11```, false otherwise
     */
    bool isLastStep(SimulationStep totalSteps) const;

    // Time-step conversion functions

    /**
     * Convert simulation step to simulation time
     * 
     * @param step Simulation step number
     * @return Simulation time = step * timestep
     * 
     * @note Fast computation: O(1
     */
    Timestamp stepToTime(SimulationStep step) const;

    /**
     * Convert simulation time to simulation step
     * 
     * @param time Simulation time in seconds
     * @return Nearest simulation step
     * 
     * @note Fast computation: O(1) time rounding
     */
    SimulationStep timeToStep(Timestamp time) const;

    // Time advancement operations

    /**
     * Advance clock by multiple steps
     * 
     * Efficiently advances the clock by a specified number of steps.
     * More efficient than repeated advance() calls.
     * 
     * @param steps Number of steps to advance (must be >= 0)
     * @throws std::invalid_argument if steps < 0
     * 
     * @see advance()
     */
    void advanceBy(SimulationStep steps);

    /**
     * Get time at a future step from current position
     * 
     * @param stepsAhead Number of steps ahead (must be >= 0)
     * @return Simulation time at future step
     * @return Current time if stepsAhead <= 0
     * 
     * @note Fast computation: direct formula
     */
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;

    // Real-time mode (for synchronized real-time simulations)

    /**
     * Get real-time wall clock time elapsed
     * 
     * Returns the wall-clock time that has passed since the simulation started
     * (if real-time mode is enabled). Useful for:
     * - Performance monitoring
     * - Real-time synchronization
     * - Simulation progress reporting
     * 
     * @return Wall-clock time elapsed in seconds (0.0 if not enabled)
     */
    double getRealTimeElapsed() const;

    /**
     * Set real-time start point
     * 
     * Initializes the real-time tracking for synchronized simulations.
     * When enabled, the clock can synchronize with real-time execution.
     * 
     * @note Real-time mode is optional and disabled by default
     */
    void setRealTimeStart();

private:
    struct Impl;
    Impl* pImpl;  ///< Pointer to implementation details (PIMPL idiom)
};

} // namespace nlm
