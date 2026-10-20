#pragma once

#include "Types.hpp"

namespace nlm {

/**
 * @class SimulationClock
 * @brief Deterministic simulation time manager separate from real-time clock
 * @details The SimulationClock provides a stable, predictable time base for
 * neural simulations that is completely independent of real-time wall clock.
 * This is crucial for reproducible simulations where time must be deterministic
 * regardless of system load or execution speed variations.
 * 
 * Time Representation:
 * - Time: Floating point values in simulation units (typically milliseconds)
 * - Step: Integer simulation steps (incremented by timestep)
 * - Timestep: Duration of each simulation step
 * 
 * Key Features:
 * - Discrete time steps with configurable duration
 * - Bidirectional conversion between step and time
 * - Real-time mode for synchronized external events
 * - Independent advancement from external timing sources
 * - Critical period tracking for developmental processes
 * 
 * Thread Safety: Not thread-safe for concurrent modification. Multiple threads
 * should use the same SimulationClock instance with external synchronization,
 * or maintain separate instances for different simulation contexts.
 * 
 * Performance: O(1) for all operations. Conversion functions use simple
 * arithmetic operations. Minimal memory overhead (~16 bytes per instance).
 * 
 * Accuracy: Floating-point operations may accumulate small errors over
 * millions of steps. For critical applications, use integer steps internally
 * with floating-point conversion for display/output.
 * 
 * Usage Patterns:
 * @code
 * // Create clock with 1ms timesteps
 * SimulationClock clock(0.001); // 1ms default
 * 
 * // Advance simulation
 * while (clock.getStep() < totalSteps) {
 *     double currentTime = clock.getTime();
 *     double timestep = clock.getTimestep();
 *     
 *     // ... simulation logic ...
 *     
 *     clock.advance();
 * }
 * 
 * // Convert between representations
 * SimulationStep step = 1000;
 * double time = clock.stepToTime(step);
 * SimulationStep back = clock.timeToStep(time);
 * 
 * // Check step conditions
 * if (clock.isFirstStep()) {
 *     // Initialize on first step
 * }
 * @endcode
 * 
 * Time Management:
 * - Time can be set arbitrarily (for time travel or specific start conditions)
 * - Stepping can be paused or resumed using setStep()
 * - Real-time mode allows synchronization with external clocks
 * - Essential for reproducibility in scientific simulations
 * 
 * @see SimulationStep
 * @see Timestamp
 * @see TimestepDuration
 * @see Brain
 */
class SimulationClock {
public:
    // Create with specific timestep
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~SimulationClock();
    
    // Disable copying, enable moving
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Time operations
    void reset();
    void advance();
    
    // Step counter
    SimulationStep getStep() const;
    void setStep(SimulationStep step);
    
    // Current simulation time
    Timestamp getTime() const;
    void setTime(Timestamp time);
    
    // Timestep
    TimestepDuration getTimestep() const;
    void setTimestep(TimestepDuration dt);
    
    // Convenience queries
    bool isFirstStep() const;
    bool isLastStep(SimulationStep totalSteps) const;
    
    // Step to time conversion
    Timestamp stepToTime(SimulationStep step) const;
    SimulationStep timeToStep(Timestamp time) const;
    
    // Advance by multiple steps
    void advanceBy(SimulationStep steps);
    
    // Get time at a future step from now
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison (for real-time mode)
    double getRealTimeElapsed() const;
    void setRealTimeStart();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
