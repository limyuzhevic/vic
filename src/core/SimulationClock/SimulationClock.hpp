#pragma once

#include "Types.hpp"

namespace nlm {

// Simulation clock for managing time in the neural simulation
// Provides deterministic time tracking separate from wall-clock time
// Enables consistent, reproducible simulation execution across different systems
// Supports both discrete (step-based) and continuous (time-based) time models

class SimulationClock {
public:
    // Create with specific timestep
    // Parameters: timestep - Time increment per simulation step (default: 0.001s = 1ms)
    // The timestep determines the granularity of time discretization
    // Typical values: 1ms for biological realism, 10ms for performance optimization
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~SimulationClock();
    
    // Disable copying, enable moving
    // Copying simulation clocks would share internal state
    // Moving is safe for resource management
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Time operations
    
    // Reset clock to initial state
    // Resets step counter to 0 and time to 0.0
    // Useful for starting/restarting simulations
    void reset();
    
    // Advance by one timestep
    // Increments step counter and adds timestep to current time
    // Called by main simulation loop to progress neural dynamics
    void advance();
    
    // Step counter
    
    // Get current simulation step number
    // Returns: Non-negative integer representing simulation iteration
    // Step 0 typically represents initial conditions
    SimulationStep getStep() const;
    
    // Set step number directly
    // Parameters: step - New step number
    // Note: This does not affect current time - use setTime() for that
    void setStep(SimulationStep step);
    
    // Current simulation time
    
    // Get current simulation time
    // Returns: Current time in seconds since simulation start
    // Typical range: [0.0, total_duration]
    Timestamp getTime() const;
    
    // Set current simulation time
    // Parameters: time - New time in seconds
    // Note: This does not affect step counter - use setStep() for that
    void setTime(Timestamp time);
    
    // Timestep management
    
    // Get timestep (time increment per step)
    // Returns: Time increment added to simulation time on each advance()
    TimestepDuration getTimestep() const;
    
    // Set timestep
    // Parameters: dt - New timestep in seconds
    // Note: Changing timestep affects future advances but not current time
    void setTimestep(TimestepDuration dt);
    
    // Convenience queries
    
    // Check if current step is the first step
    // Returns: true if step == 0, false otherwise
    // Useful for initialization code that should run only once
    bool isFirstStep() const;
    
    // Check if current step is the last step in a multi-step simulation
    // Parameters: totalSteps - Total number of steps in simulation
    // Returns: true if current step >= totalSteps - 1, false otherwise
    bool isLastStep(SimulationStep totalSteps) const;
    
    // Step to time conversion
    
    // Convert simulation step number to simulation time
    // Parameters: step - Step number to convert
    // Returns: Corresponding time in seconds
    Timestamp stepToTime(SimulationStep step) const;
    
    // Convert simulation time to nearest step number
    // Parameters: time - Time in seconds to convert
    // Returns: Nearest step number (floor division by timestep)
    SimulationStep timeToStep(Timestamp time) const;
    
    // Advance by multiple steps
    // Parameters: steps - Number of steps to advance (can be zero)
    // Efficiently advances time by steps * timestep
    void advanceBy(SimulationStep steps);
    
    // Get time at a future step from current position
    // Parameters: stepsAhead - Number of steps to look ahead
    // Returns: Time that will be reached after advancing stepsAhead steps
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison (for real-time mode)
    
    // Get elapsed real-time since simulation start
    // Returns: Real elapsed time in seconds (wall clock time)
    // Used for controlling simulation speed in real-time visualization
    double getRealTimeElapsed() const;
    
    // Start timing for real-time mode
    // Initializes internal timer for wall-clock time tracking
    // Should be called when starting real-time simulation execution
    void setRealTimeStart();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
