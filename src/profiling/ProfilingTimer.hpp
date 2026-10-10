#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace nlm {

// High-precision timing utilities
using Timestamp = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double, std::milli>;

// Performance measurement units
enum class PerformanceUnit {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds,
    Minutes,
    Hours
};

// Profiling timer classes for performance measurement
class ProfilingTimer {
public:
    ProfilingTimer(const std::string& name, bool autoStart = true);
    ~ProfilingTimer();
    
    // Start the timer
    void start();
    
    // Stop the timer and return elapsed time
    double stop();
    
    // Reset timer to zero
    void reset();
    
    // Check if timer is currently running
    bool isRunning() const { return running_; }
    
    // Get the name of this timer
    const std::string& getName() const { return name_; }
    
    // Get the elapsed time in current units
    double getElapsed() const;
    
    // Set the output unit for this timer
    void setUnit(PerformanceUnit unit) { unit_ = unit; }
    
    // Format elapsed time as string
    std::string formatElapsed() const;
    
    // Get the timestamp when timer was last started
    Timestamp getStartTime() const { return startTime_; }
    
    // Get the timestamp when timer was last stopped
    Timestamp getEndTime() const { return endTime_; }
    
    // Convert duration to human-readable string
    static std::string formatDuration(Duration duration, PerformanceUnit unit = PerformanceUnit::Milliseconds);
    
private:
    std::string name_;
    Timestamp startTime_;
    Timestamp endTime_;
    Duration accumulatedDuration_;
    bool running_;
    PerformanceUnit unit_;
};

// System benchmarking timer with multiple measurements
class BenchmarkTimer {
public:
    BenchmarkTimer(const std::string& name);
    
    // Start benchmark measurement
    void start() { timer_.start(); }
    
    // Stop benchmark measurement and record it
    void stop();
    
    // Add a separate measurement point
    void checkpoint(const std::string& label);
    
    // Get all recorded measurements
    const std::vector<std::pair<std::string, double>>& getMeasurements() const { return measurements_; }
    
    // Get the total time for the benchmark
    double getTotalTime() const { return timer_.getElapsed(); }
    
    // Get number of measurements
    size_t getMeasurementCount() const { return measurements_.size(); }
    
    // Export benchmark results to string
    std::string exportToString() const;
    
    // Export benchmark results to file
    bool exportToFile(const std::string& filename) const;
    
private:
    std::string name_;
    ProfilingTimer timer_;
    std::vector<std::pair<std::string, double>> measurements_;
    std::vector<Timestamp> timestamps_;
};

// CPU profiling timer for detailed performance analysis
class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start CPU profiling
    void startProfiling(const std::string& operationName);
    
    // Stop CPU profiling for current operation
    void stopProfiling();
    
    // Get current operation being profiled
    const std::string& getCurrentOperation() const { return currentOperation_; }
    
    // Check if currently profiling
    bool isProfiling() const { return isProfiling_; }
    
    // Get total CPU time across all profiled operations
    double getTotalCPUTime() const { return totalCPUTime_; }
    
    // Get number of profiling sessions
    size_t getProfilingCount() const { return profilingSessions_.size(); }
    
    // Export profiling data
    std::string exportProfilingData() const;
    
    // Clear all profiling data
    void clear();
    
private:
    struct ProfilingSession {
        std::string operationName;
        Timestamp startTime;
        Duration cpuTime;
        Duration wallTime;
        size_t callCount;
        
        ProfilingSession(const std::string& name) 
            : operationName(name), startTime(std::chrono::high_resolution_clock::now()), 
              cpuTime(0), wallTime(0), callCount(0) {}
    };
    
    std::string currentOperation_;
    bool isProfiling_;
    double totalCPUTime_;
    std::vector<ProfilingSession> profilingSessions_;
    std::mutex mutex_;
};

// Memory profiling timer for tracking memory usage
class MemoryProfiler {
public:
    MemoryProfiler();
    
    // Start memory profiling
    void startMemoryProfile(const std::string& context);
    
    // Stop memory profiling
    void stopMemoryProfile();
    
    // Get current memory usage
    static size_t getCurrentMemoryUsage();
    
    // Get peak memory usage
    size_t getPeakMemoryUsage() const { return peakMemoryUsage_; }
    
    // Get total allocated memory
    size_t getTotalAllocated() const { return totalAllocated_; }
    
    // Get number of allocation events
    size_t getAllocationCount() const { return allocationCount_; }
    
    // Export memory profile data
    std::string exportMemoryProfile() const;
    
private:
    struct MemorySnapshot {
        Timestamp timestamp;
        size_t memoryUsage;
        std::string context;
        
        MemorySnapshot(size_t usage, const std::string& ctx)
            : timestamp(std::chrono::high_resolution_clock::now()), 
              memoryUsage(usage), context(ctx) {}
    };
    
    size_t peakMemoryUsage_;
    size_t totalAllocated_;
    size_t allocationCount_;
    std::vector<MemorySnapshot> memorySnapshots_;
    std::mutex mutex_;
};

// Neural network simulation timing with detailed metrics
class NeuralSimulationTimer {
public:
    NeuralSimulationTimer(const std::string& simulationName);
    
    // Start neural simulation timing
    void startSimulation();
    
    // Stop neural simulation timing
    void stopSimulation();
    
    // Record spike processing time
    void recordSpikeProcessingTime(double time);
    
    // Record plasticity update time
    void recordPlasticityUpdateTime(double time);
    
    // Record memory operation time
    void recordMemoryOperationTime(double time);
    
    // Record neuron update time
    void recordNeuronUpdateTime(double time);
    
    // Get total simulation time
    double getSimulationTime() const { return simulationTimer_.getElapsed(); }
    
    // Get total spike processing time
    double getTotalSpikeProcessingTime() const { return totalSpikeProcessingTime_; }
    
    // Get total plasticity update time
    double getTotalPlasticityUpdateTime() const { return totalPlasticityUpdateTime_; }
    
    // Get total memory operation time
    double getTotalMemoryOperationTime() const { return totalMemoryOperationTime_; }
    
    // Get total neuron update time
    double getTotalNeuronUpdateTime() const { return totalNeuronUpdateTime_; }
    
    // Get number of spikes processed
    size_t getSpikeCount() const { return spikeCount_; }
    
    // Get number of plasticity updates
    size_t getPlasticityUpdateCount() const { return plasticityUpdateCount_; }
    
    // Export simulation metrics
    std::string exportSimulationMetrics() const;
    
private:
    std::string simulationName_;
    ProfilingTimer simulationTimer_;
    double totalSpikeProcessingTime_;
    double totalPlasticityUpdateTime_;
    double totalMemoryOperationTime_;
    double totalNeuronUpdateTime_;
    size_t spikeCount_;
    size_t plasticityUpdateCount_;
    std::mutex mutex_;
};

// Global registry for all active timers
class TimerRegistry {
public:
    static TimerRegistry& getInstance();
    
    // Register a new timer
    void registerTimer(std::shared_ptr<ProfilingTimer> timer);
    
    // Unregister a timer
    void unregisterTimer(const std::string& name);
    
    // Get timer by name
    std::shared_ptr<ProfilingTimer> getTimer(const std::string& name);
    
    // Get all registered timers
    const std::vector<std::shared_ptr<ProfilingTimer>>& getAllTimers() const { return timers_; }
    
    // Clear all timers
    void clear();
    
    // Export all timers data
    std::string exportAllTimers() const;
    
    // Start all timers
    void startAllTimers();
    
    // Stop all timers
    void stopAllTimers();
    
private:
    TimerRegistry() = default;
    std::vector<std::shared_ptr<ProfilingTimer>> timers_;
    std::mutex mutex_;
};

} // namespace nlm