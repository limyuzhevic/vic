#pragma once

#include "Types.hpp"

namespace nlm {

// Simulation clock for managing time in the neural simulation
// Provides deterministic time tracking separate from wall-clock time

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
    Timestamp timeAtStep(SimulationStep targetStep) const;
    
    // Real time tracking
    void startRealTime();
    double getRealTime() const;
    void pauseRealTime();
    bool isRealTimeStarted() const;
    
    // Time since last real time operation
    double timeSinceRealStart() const;
    
    // Time since last real time operation with explicit timestamp
    double timeSinceRealStart(Timestamp timestamp) const;
    
    // Real time elapsed since clock start
    double getRealElapsed() const;
    
    // Calculate wall-time equivalent for a given simulation step
    double stepToWallTime(SimulationStep step, TimestepDuration dt) const;
    
    // Get number of wall-time seconds for simulation step
    double stepToWallSeconds(SimulationStep step, TimestepDuration dt) const;
    
    // Set real time mode
    void setRealTimeMode(bool enabled);
    bool isRealTimeMode() const;
};

} // namespace nlm