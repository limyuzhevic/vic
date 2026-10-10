// Simulation clock system for NLM
#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace nlm {

using SimulationStep = uint64_t;
using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;

class SimulationClock {
public:
    SimulationClock(double timestep = 0.001);
    ~SimulationClock() = default;
    
    // Advance simulation by one timestep
    void advance();
    
    // Get current simulation time
    Timestamp getTime() const;
    
    // Get current simulation step
    SimulationStep getStep() const;
    
    // Get timestep
    double getTimestep() const;
    
    // Set timestep
    void setTimestep(double timestep);
    
    // Reset simulation
    void reset();
    
    // Check if simulation has started
    bool isRunning() const;
    
    // Get real time elapsed
    double getRealTimeElapsed() const;
    
    // Get simulation time elapsed
    double getSimulationTimeElapsed() const;
    
    // Get formatted time string
    std::string getFormattedTime() const;
    
    // Get formatted simulation time
    std::string getFormattedSimulationTime() const;
    
    // Create a copy of the current clock state
    SimulationClock copy() const;
    
    // Restore clock state from another clock
    void restore(const SimulationClock& other);
    
private:
    Timestamp currentTime_;
    SimulationStep currentStep_;
    double timestep_;
    bool running_;
    TimePoint startTime_;
    Duration realTimeElapsed_;
    
    void updateRealTime();
    void updateSimulationTime();
};

} // namespace nlm