#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <chrono>
#include <map>
#include <mutex>

namespace nlm {

// Advanced timing system with profiling and scheduling
class TimeSystem {
public:
    TimeSystem();
    ~TimeSystem();
    
    // Disable copying
    TimeSystem(const TimeSystem&) = delete;
    TimeSystem& operator=(const TimeSystem&) = delete;
    
    // Get global time system instance
    static TimeSystem& getInstance();
    
    // Core timing operations
    double getCurrentRealTime() const;
    Timestamp getCurrentSimulationTime() const;
    SimulationStep getCurrentStep() const;
    TimestepDuration getTimestep() const;
    
    // Control simulation time
    void setTimestep(TimestepDuration timestep);
    void advanceSimulation(double timeDelta = 0.0);
    void advanceSteps(SimulationStep steps);
    
    // Time profiling
    struct PerformanceMetric {
        std::string operation;
        std::chrono::high_resolution_clock::duration duration;
        std::map<std::string, std::string> context;
        Timestamp timestamp;
    };
    
    void recordPerformance(const std::string& operation, 
                          const std::chrono::high_resolution_clock::duration& duration,
                          const std::map<std::string, std::string>& context = {});
    
    std::vector<PerformanceMetric> getPerformanceMetrics(const std::string& operation = "",
                                                         Timestamp startTime = 0.0,
                                                         Timestamp endTime = std::numeric_limits<Timestamp>::max()) const;
    
    void clearPerformanceMetrics();
    
    // Time scheduling
    class Timer {
    public:
        Timer(const std::string& name, Timestamp delay, Timestamp period = 0.0);
        
        bool isReady(Timestamp currentTime) const;
        std::string getName() const { return name_; }
        Timestamp getDelay() const { return delay_; }
        Timestamp getPeriod() const { return period_; }
        
    private:
        std::string name_;
        Timestamp delay_;
        Timestamp period_;
        mutable Timestamp lastExecuted_;
    };
    
    void addTimer(const std::string& name, Timestamp delay, Timestamp period = 0.0);
    void removeTimer(const std::string& name);
    void updateTimers(Timestamp currentTime);
    
    // Time conversion utilities
    std::string formatTime(Timestamp time, const std::string& format = "%H:%M:%S") const;
    std::string formatDuration(std::chrono::high_resolution_clock::duration duration) const;
    
    // Time statistics
    struct TimeStatistics {
        Timestamp totalSimulationTime;
        SimulationStep totalSimulationSteps;
        size_t performanceMetricCount;
        double averageStepDuration;
        Timestamp minStepDuration;
        Timestamp maxStepDuration;
    };
    
    TimeStatistics getStatistics() const;
    
    // Time snapshot for debugging
    struct TimeSnapshot {
        Timestamp simulationTime;
        Timestamp realTime;
        SimulationStep step;
        TimestepDuration timestep;
        std::chrono::high_resolution_clock::time_point snapshotTime;
    };
    
    TimeSnapshot takeSnapshot() const;
    
    // Time range operations
    void setTimeRange(Timestamp startTime, Timestamp endTime);
    bool isTimeInRange(Timestamp time) const;
    Timestamp clampToRange(Timestamp time) const;
    
    // Time format constants
    static constexpr const char* DEFAULT_TIME_FORMAT = "%H:%M:%S.%f";
    static constexpr const char* DEFAULT_DATE_FORMAT = "%Y-%m-%d";
    static constexpr const char* DEFAULT_DATETIME_FORMAT = "%Y-%m-%d %H:%M:%S";
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor for singleton pattern
    TimeSystem();
};

} // namespace nlm