#ifndef NLM_CORE_SIMULATIONCLOCK_SIMULATIONCLOCK_HPP
#define NLM_CORE_SIMULATIONCLOCK_SIMULATIONCLOCK_HPP

#include <chrono>
#include <cstdint>

namespace nlm {

typedef uint64_t SimulationStep;
typedef double Timestamp;

class SimulationClock {
public:
    SimulationClock(Timestamp timestep = 1.0);
    ~SimulationClock();
    
    void setTimestep(Timestamp timestep);
    Timestamp getTimestep() const;
    
    SimulationStep advance();
    Timestamp getTime() const;
    SimulationStep getStep() const;
    
    Timestamp getCurrentTime() const { return currentTime; }
    SimulationStep getCurrentStep() const { return currentStep; }
    
    void reset();
    
    // Time utility functions
    static Timestamp now();
    static Timestamp toSeconds(std::chrono::high_resolution_clock::time_point timePoint);
    static std::chrono::high_resolution_clock::time_point fromSeconds(Timestamp seconds);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_CORE_SIMULATIONCLOCK_SIMULATIONCLOCK_HPP
