#ifndef NLM_SIMULATION_CLOCK_HPP
#define NLM_SIMULATION_CLOCK_HPP

#include <chrono>

namespace nlm {

class SimulationClock {
public:
    SimulationClock(double timestep = 0.001);
    ~SimulationClock();
    
    // Disable copying
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    
    // Enable moving
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Get current time
    Timestamp getCurrentTime() const;
    
    // Get current step
    SimulationStep getCurrentStep() const;
    
    // Step forward
    void step(SimulationStep steps = 1);
    
    // Get timestep
    double getTimestep() const;
    
    // Set timestep
    void setTimestep(double timestep);
    
    // Get wall clock time
    double getWallClockTime() const;
    
    // Reset to zero
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // NLM_SIMULATION_CLOCK_HPP