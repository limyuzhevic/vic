#pragma once

#include "Types.hpp"
#include <string>
#include <chrono>

namespace nlm {

// Comprehensive simulation clock for deterministic neural network simulation
// Provides accurate time tracking separate from wall-clock time
// Essential for reproducible scientific experiments and deterministic behavior

class SimulationClock {
public:
    // Create with specific timestep for the simulation
    // timestep: Simulation time increment between steps (e.g., 0.001 for 1ms)
    // Default timestep is 0.001 seconds (1 millisecond)
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~SimulationClock();
    
    // Disable copying to prevent multiple access to same clock
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    
    // Enable moving - transfer ownership of clock state
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Core clock operations
    void reset();                    // Reset to initial state (step 0, time 0)
    void advance();                   // Advance by one timestep
    
    // Step management
    SimulationStep getStep() const;   // Get current simulation step (integer)
    void setStep(SimulationStep step); // Set step directly (advanced usage)
    
    // Time management
    Timestamp getTime() const;       // Get current simulation time in seconds
    void setTime(Timestamp time);    // Set time directly (advanced usage)
    
    // Timestep configuration
    TimestepDuration getTimestep() const;  // Get the timestep duration
    void setTimestep(TimestepDuration dt); // Change timestep (resets time if needed)
    
    // Convenience queries
    bool isFirstStep() const;         // Check if currently at step 0
    bool isLastStep(SimulationStep totalSteps) const;  // Check if at final step
    
    // Step-time conversion utilities
    Timestamp stepToTime(SimulationStep step) const;      // Convert step to time
    SimulationStep timeToStep(Timestamp time) const;       // Convert time to step (rounded)
    
    // Multi-step operations
    void advanceBy(SimulationStep steps);  // Advance by multiple steps
    
    // Get time at future step from current position
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock integration (for real-time simulation mode)
    double getRealTimeElapsed() const;  // Get wall-clock time since start
    void setRealTimeStart();            // Initialize real-time wall clock
    
    // Performance monitoring
    double getSimulationSpeed() const;  // Steps per second (real-time)
    double getTimeRemaining(SimulationStep totalSteps) const;  // Estimated remaining time
    
    // Synchronized operations
    // Performs atomic advance of step and time
    void stepAndAdvance();             // Advance by one timestep atomically
    
    // Clock validation
    bool isSynchronized() const;        // Verify step and time are consistent
    
    // Serialization for checkpointing
    void saveState(std::ostream& out) const;   // Save clock state to stream
    void loadState(std::istream& in);         // Load clock state from stream
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal validation helpers
    static bool validateStepTimePair(SimulationStep step, Timestamp time, TimestepDuration timestep);
    
    // Time arithmetic helpers
    static Timestamp calculateTimeFromStep(SimulationStep step, TimestepDuration timestep);
    static SimulationStep calculateStepFromTime(Timestamp time, TimestepDuration timestep);
};

// Clock statistics and metrics
struct SimulationClockStats {
    SimulationStep currentStep;       // Current simulation step
    Timestamp currentTime;            // Current simulation time (seconds)
    TimestepDuration timestep;       // Timestep duration
    double wallClockElapsed;          // Real time elapsed (seconds)
    double simulationSpeed;           // Steps per second
    bool isRealTimeMode;              // Whether synchronized to wall clock
    
    // Get human-readable status string
    std::string getStatusString() const;
    
    // Get performance metrics
    double getStepsPerSecond() const { return simulationSpeed; }
    double getSecondsPerStep() const { return timestep; }
    double getTimeRemaining(SimulationStep totalSteps) const;
};

// Global clock accessor
SimulationClock& getGlobalSimulationClock();

// Clock synchronization utilities
class ClockSynchronizer {
public:
    ClockSynchronizer();
    ~ClockSynchronizer();
    
    // Enable/disable real-time mode
    void enableRealTimeMode();
    void disableRealTimeMode();
    bool isRealTimeMode() const;
    
    // Set target real-time speed (1.0 = real-time, 2.0 = 2x real-time, 0.5 = 0.5x real-time)
    void setRealTimeSpeed(double speed);
    double getRealTimeSpeed() const;
    
    // Get current clock reference
    const SimulationClock& getClock() const;
    SimulationClock& getClock();
    
    // Synchronize clock to specific step/time
    void setTime(SimulationStep step, Timestamp time);
    
    // Wait until specific step is reached (blocking)
    void waitUntilStep(SimulationStep step);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
