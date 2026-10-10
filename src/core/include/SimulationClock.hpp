// Simulation clock interface
#ifndef NLM_CORE_SIMULATION_CLOCK_HPP
#define NLM_CORE_SIMULATION_CLOCK_HPP

#include <chrono>
#include <cstdint>

namespace nlm {
namespace core {

class SimulationClock {
public:
    SimulationClock();
    ~SimulationClock();
    
    void reset();
    void advance(double seconds);
    void advance(double days, double hours, double minutes, double seconds);
    
    double getCurrentTime() const;
    double getElapsedTime() const;
    
    uint64_t getDays() const;
    uint64_t getHours() const;
    uint64_t getMinutes() const;
    uint64_t getSeconds() const;
    uint64_t getMilliseconds() const;
    
    bool isLeapYear() const;
    uint8_t getDayOfYear() const;
    uint8_t getDayOfMonth() const;
    uint8_t getDayOfWeek() const;
    uint8_t getMonth() const;
    uint16_t getYear() const;
    
private:
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point currentTime;
    double elapsedSeconds;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_SIMULATION_CLOCK_HPP
