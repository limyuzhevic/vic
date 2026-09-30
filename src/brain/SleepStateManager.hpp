#pragma once

#include "SleepState.hpp"
#include <string>
#include <map>

namespace nlm {

// Forward declaration to avoid circular dependency
class Brain;

// SleepStateManager: Manages sleep state transitions and regulation
class SleepStateManager {
public:
    SleepStateManager();
    ~SleepStateManager();
    
    // Initialize with brain reference and configuration
    void initialize(Brain* brain);
    
    // Update sleep state machine
    void update(float dt, float currentWakeTime);
    
    // Force transition to a specific state
    void transitionToState(SleepState newState, float transitionDuration = 1.0f);
    
    // Get current sleep state
    SleepState getCurrentState() const { return currentState_; }
    
    // Check if currently in a sleep state (not Wake)
    bool isSleeping() const { return currentState_ != SleepState::Wake; }
    
    // Get state characteristics
    const SleepStateCharacteristics& getStateCharacteristics(SleepState state) const;
    
    // Sleep pressure management
    void addSleepPressure(float pressure);  // Increase homeostatic pressure
    void releaseSleepPressure(float pressure);  // Decrease pressure (during wake)
    float getSleepPressure() const { return sleepPressure_; }
    
    // Sleep debt management
    void addSleepDebt(float debt);  // Accumulate sleep debt
    void reduceSleepDebt(float debt);  // Reduce debt (during sleep)
    float getSleepDebt() const { return sleepDebt_; }
    
    // Circadian regulation
    void setCircadianPhase(float phase) { circadianPhase_ = phase; }
    void setCircadianPeriod(float period) { circadianPeriod_ = period; }
    float getCircadianPhase() const { return circadianPhase_; }
    float getCircadianPeriod() const { return circadianPeriod_; }
    float getCircadianSleepWindow() const { return getCircadianSleepProbability(0.5f); }
    
    // State transition control
    void setStateTransitionSpeed(float speed) { transitionSpeed_ = speed; }
    void setMinimumStateDuration(float duration) { minimumStateDuration_ = duration; }
    
    // Integration with other systems
    void integrateWithNeuromodulation();  // Apply neuromodulation based on state
    void integrateWithMemoryReplay();     // Coordinate memory replay with sleep states
    void integrateWithWorkingMemory();    // Manage working memory during sleep states
    
    // State-specific processing
    void processQuietSleep();             // Implement QuietSleep state behavior
    void processDeepSleep();              // Implement DeepSleep state behavior  
    void processREMSleep();               // Implement REMSleep state behavior
    void processWake();                   // Implement Wake state behavior
    
    // Memory replay coordination
    void scheduleMemoryReplay();           // Schedule replay based on current state
    void executeMemoryReplay();            // Execute replay for current state
    
    // Checkpointing support
    std::string serialize() const;        // Serialize sleep state for checkpointing
    bool deserialize(const std::string& data);  // Deserialize from checkpoint
    
    // Statistics and monitoring
    float getTimeInCurrentState() const { return timeInCurrentState_; }
    float getTotalSleepTime() const { return totalSleepTime_; }
    float getSleepEfficiency() const { return sleepEfficiency_; }
    size_t getMemoryReplayCount() const { return memoryReplayCount_; }
    
    // Configuration
    void configure(const std::map<std::string, float>& settings);
    
private:
    // State machine
    SleepState currentState_;
    SleepState targetState_;
    float transitionProgress_;           // 0.0 to 1.0 for smooth interpolation
    float transitionTimer_;              // Timer for current transition
    
    // Timing and rhythm
    float timeInCurrentState_;           // Time spent in current state
    float totalSleepTime_;               // Total sleep time accumulated
    float circadianPhase_;               // Current phase of circadian rhythm (0-2π)
    float circadianPeriod_;              // Period of circadian rhythm in seconds
    float wakeTime_;                     // Current wake time for homeostatic regulation
    
    // Homeostatic regulation
    float sleepPressure_;               // Accumulated sleep pressure
    float sleepDebt_;                   // Outstanding sleep debt
    float wakeDuration_;                // Current wake duration
    
    // Control parameters
    float transitionSpeed_;             // Speed of state transitions (0.0 to 1.0)
    float minimumStateDuration_;        // Minimum time to stay in each state
    float timeSinceWake_;               // Time since last wake period
    
    // Statistics
    float sleepEfficiency_;            // Quality of sleep (0.0 to 1.0)
    size_t memoryReplayCount_;          // Number of memory replays executed
    float lastReplayTime_;              // Time of last replay
    
    // State-specific timing
    float quietSleepTimer_;             // Timer for QuietSleep duration
    float deepSleepTimer_;              // Timer for DeepSleep duration
    float remSleepTimer_;               // Timer for REMSleep duration
    
    // Integration state
    Brain* brain_;                      // Reference to brain
    bool neuromodulationIntegrated_;    // Whether neuromodulation is set up for current state
    bool memoryReplayActive_;          // Whether memory replay is active
    
    // State characteristic cache
    std::map<SleepState, SleepStateCharacteristics> stateCharacteristics_;
    
    // State transition timing
    void updateStateTransitions(float dt);  // Update state transition timers
    bool shouldTransitionToNextState() const;  // Check if should transition
    SleepState getNextState() const;  // Get next state in cycle
    
    // Helper functions
    void updateCircadianPhase(float dt);      // Update circadian phase
    float getCircadianSleepProbability(float time) const;  // Get sleep probability based on circadian rhythm
    float getHomeostaticSleepPressure() const;  // Get sleep pressure from homeostatic regulation
    void updateSleepPressure(float dt);       // Update sleep pressure over time
    
    // State-specific implementations
    void onStateEnter(SleepState newState);   // Called when entering a state
    void onStateExit(SleepState oldState);    // Called when exiting a state
    void onStateUpdate(SleepState current, SleepState target, float progress);  // Called during transition
};

} // namespace nlm
