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
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison (for real-time mode)
    double getRealTimeElapsed() const;
    void setRealTimeStart();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
