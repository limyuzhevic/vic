#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <chrono>

namespace nlm {

/**
 * SleepState: Manages sleep/rest cycles for memory consolidation and replay
 * 
 * Key mechanisms:
 * - Detects sleep/wake transitions based on neuromodulatory signals
 * - Manages sleep phase timing and duration
 * - Coordinates memory replay during sleep periods
 * - Controls consolidation windows
 * - Integrates with neuromodulation for sleep timing
 * 
 * Integration:
 * - Works with MemoryReplay for sleep-time replay
 * - Provides sleep phase boundaries for other systems
 * - Can be controlled by neuromodulatory signals
 * - Maintains sleep/rest state consistency
 * - Provides sleep statistics
 */

class SleepState {
public:
    SleepState();
    ~SleepState();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update sleep state (called every step)
    void update(SimulationStep currentStep);
    
    // Check if we are currently in sleep phase
    bool isSleeping(SimulationStep currentStep) const;
    
    // Get sleep phase start time
    SimulationStep getSleepStart(SimulationStep currentStep) const;
    
    // Get sleep phase end time
    SimulationStep getSleepEnd(SimulationStep currentStep) const;
    
    // Force sleep (for testing or controlled sleep)
    void forceSleep(bool sleep);
    
    // Set sleep phase parameters
    void setSleepPhaseDuration(size_t duration) { sleepPhaseDuration_ = duration; }
    void setWakePhaseDuration(size_t duration) { wakePhaseDuration_ = duration; }
    void setDutyCycle(float duty) { dutyCycle_ = duty; }
    
    // Enable/disable sleep state management
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get sleep statistics
    size_t getSleepDuration(SimulationStep currentStep) const;
    size_t getWakeDuration(SimulationStep currentStep) const;
    float getSleepPercentage(SimulationStep currentStep) const;
    
    // Get sleep state
    bool isCurrentlySleeping() const { return isSleeping_; }
    SimulationStep getLastSleepStart() const { return lastSleepStart_; }
    SimulationStep getLastWakeStart() const { return lastWakeStart_; }
    
    // Check if system is ready (has brain reference)
    bool isReady() const { return brain_ != nullptr; }
    
    // Reset sleep state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Sleep phase parameters
    size_t sleepPhaseDuration_;
    size_t wakePhaseDuration_;
    float dutyCycle_;
    bool enabled_;
    
    // Sleep state
    bool isSleeping_;
    SimulationStep lastSleepStart_;
    SimulationStep lastWakeStart_;
    
    // Statistics
    size_t totalSleepTime_;
    size_t totalWakeTime_;
    
    // Calculate sleep/wake transitions
    bool shouldTransitionToSleep(SimulationStep currentStep) const;
    bool shouldTransitionToWake(SimulationStep currentStep) const;
    
    // Calculate sleep/wake boundaries
    SimulationStep calculateSleepStart(SimulationStep currentStep) const;
    SimulationStep calculateWakeStart(SimulationStep currentStep) const;
    
    // Update sleep/wake transitions
    void updateSleepWakeTransitions(SimulationStep currentStep);
};

} // namespace nlm