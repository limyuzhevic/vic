// Performance monitoring and debugging tools for NLM

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Performance measurement utilities
class PerformanceProfiler {
public:
    struct Measurement {
        std::string name;
        std::chrono::duration<double> duration;
        uint64_t samples;
        double minTime;
        double maxTime;
        double avgTime;
        
        Measurement(const std::string& n) 
            : name(n), duration(0), samples(0), minTime(0), maxTime(0), avgTime(0) {}
    };
    
    PerformanceProfiler() : running_(false), totalDuration_(0) {}
    
    void start(const std::string& name);
    void stop(const std::string& name);
    void reset();
    
    std::vector<Measurement> getMeasurements() const;
    std::string getReport() const;
    double getTotalTime() const { return totalDuration_; }
    
    // Convenience methods
    class ScopedTimer {
    public:
        ScopedTimer(PerformanceProfiler& profiler, const std::string& name) 
            : profiler_(profiler), name_(name) {
            profiler_.start(name_);
        }
        
        ~ScopedTimer() {
            profiler_.stop(name_);
        }
    };
    
    ScopedTimer time(const std::string& name) {
        return ScopedTimer(*this, name);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool running_;
    double totalDuration_;
    std::mutex mutex_;
};

// Memory monitoring utilities
class MemoryMonitor {
public:
    struct MemoryStats {
        size_t totalMemory;
        size_t allocatedMemory;
        size_t peakMemory;
        size_t allocationCount;
        size_t deallocationCount;
        std::chrono::time_point<std::chrono::steady_clock> lastAllocation;
        
        MemoryStats() 
            : totalMemory(0), allocatedMemory(0), peakMemory(0), 
              allocationCount(0), deallocationCount(0) {}
    };
    
    MemoryMonitor();
    ~MemoryMonitor();
    
    void trackAllocation(size_t size, const std::string& source = "unknown");
    void trackDeallocation(size_t size, const std::string& source = "unknown");
    
    MemoryStats getStats() const;
    std::string getReport() const;
    
    // Get current memory usage (platform-specific implementation)
    static size_t getCurrentMemoryUsage();
    static size_t getPeakMemoryUsage();
    
    // Memory leak detection
    bool detectLeaks() const;
    std::vector<std::string> getLeaks() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time performance monitoring for simulations
class SimulationMonitor {
public:
    struct PerformanceMetrics {
        double simulationTime;           // Real time elapsed
        double wallClockTime;            // CPU time
        size_t totalSteps;               // Number of simulation steps
        size_t spikeCount;               // Total spikes generated
        size_t neuronCount;              // Number of active neurons
        size_t synapseCount;             // Number of synapses
        double avgStepsPerSecond;        // Average simulation speed
        double spikeRate;                // Average spikes per second
        double memoryUsage;              // Current memory usage (MB)
        double cpuUsage;                 // CPU usage percentage
        
        PerformanceMetrics()
            : simulationTime(0), wallClockTime(0), totalSteps(0), 
              spikeCount(0), neuronCount(0), synapseCount(0),
              avgStepsPerSecond(0), spikeRate(0), memoryUsage(0), cpuUsage(0) {}
    };
    
    SimulationMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    void recordStep(double realTime, double cpuTime, 
                   size_t spikeCount, size_t neuronCount,
                   size_t synapseCount, size_t steps);
    
    PerformanceMetrics getCurrentMetrics() const;
    std::string getPerformanceReport() const;
    std::vector<PerformanceMetrics> getHistory() const;
    
    // Performance alerts and thresholds
    void setStepTimeThreshold(double threshold); // seconds per step
    void setMemoryThreshold(size_t threshold);   // bytes
    void setCPUTimeThreshold(double threshold);   // seconds
    
    bool checkPerformanceThresholds() const;
    std::string getPerformanceAlerts() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network performance analyzer
class NeuralPerformanceAnalyzer {
public:
    struct NeuronPerformance {
        size_t neuronId;
        uint64_t spikeCount;
        double avgFiringRate;
        double maxFiringRate;
        double avgMembranePotential;
        double avgThreshold;
        std::vector<double> firingHistory; // Recent firing activity
        
        NeuronPerformance(size_t id) : neuronId(id), spikeCount(0), 
                                       avgFiringRate(0), maxFiringRate(0),
                                       avgMembranePotential(0), avgThreshold(0) {}
    };
    
    struct SynapsePerformance {
        size_t synapseId;
        size_t sourceId;
        size_t targetId;
        SynapticWeight avgWeight;
        double avgTransmissionDelay;
        uint64_t transmissionCount;
        double avgTransmissionSuccessRate;
        
        SynapsePerformance(size_t id, size_t src, size_t tgt)
            : synapseId(id), sourceId(src), targetId(tgt), avgWeight(0),
              avgTransmissionDelay(0), transmissionCount(0), avgTransmissionSuccessRate(0) {}
    };
    
    NeuralPerformanceAnalyzer();
    
    // Update analyzer with neuron state
    void updateNeuronPerformance(size_t neuronId, const NeuronState& state, 
                                double timestep, uint64_t spikeCount);
    
    // Update analyzer with synapse state
    void updateSynapsePerformance(size_t synapseId, size_t sourceId, size_t targetId,
                                 SynapticWeight weight, double delay, 
                                 bool transmissionSuccess);
    
    // Get performance analysis
    std::vector<NeuronPerformance> getTopNeuronsByFiringRate(size_t limit = 10) const;
    std::vector<NeuronPerformance> getTopNeuronsByVariability(size_t limit = 10) const;
    std::vector<SynapsePerformance> getTopSynapsesByUsage(size_t limit = 10) const;
    
    // Get performance insights
    std::string getPerformanceInsights() const;
    std::string getBottleneckAnalysis() const;
    std::string getOptimizationRecommendations() const;
    
    // Reset analysis
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Debug logging and event tracking
class DebugLogger {
public:
    struct LogEvent {
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        std::string level;           // DEBUG, INFO, WARN, ERROR
        std::string source;          // Component name
        std::string message;
        std::map<std::string, std::string> metadata;
        
        LogEvent(const std::string& level, const std::string& source, 
                const std::string& message)
            : level(level), source(source), message(message) {}
    };
    
    DebugLogger();
    
    // Event logging
    void log(const std::string& level, const std::string& source, 
             const std::string& message, 
             const std::map<std::string, std::string>& metadata = {});
    
    // Specialized logging methods
    void debug(const std::string& source, const std::string& message);
    void info(const std::string& source, const std::string& message);
    void warn(const std::string& source, const std::string& message);
    void error(const std::string& source, const std::string& message);
    
    // Performance event logging
    void logStepPerformance(const std::string& source, double stepTime, 
                           size_t stepsPerSecond);
    void logMemoryUsage(const std::string& source, size_t memoryBytes);
    void logNeuralActivity(const std::string& source, size_t activeNeurons,
                          size_t totalSpikeCount);
    
    // Event querying
    std::vector<LogEvent> getEvents(const std::string& level = "", 
                                   const std::string& source = "",
                                   const std::chrono::time_point<std::chrono::steady_clock>& 
                                       startTime = std::chrono::steady_clock::time_point(),
                                   const std::chrono::time_point<std::chrono::steady_clock>& 
                                       endTime = std::chrono::steady_clock::time_point()) const;
    
    // Report generation
    std::string getLogReport() const;
    std::string getEventSummary() const;
    
    // Log management
    void clear();
    void saveToFile(const std::string& filepath) const;
    void loadFromFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Unified performance and debugging interface
class PerformanceDebugger {
public:
    PerformanceDebugger();
    
    // Performance profiling
    PerformanceProfiler& getProfiler();
    MemoryMonitor& getMemoryMonitor();
    SimulationMonitor& getSimulationMonitor();
    NeuralPerformanceAnalyzer& getNeuralAnalyzer();
    DebugLogger& getDebugLogger();
    
    // Convenience methods for common operations
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    
    void logSimulationStep(size_t step, double realTime, double cpuTime);
    void logNeuralState(size_t step, const Brain& brain);
    
    // Report generation
    std::string generatePerformanceReport() const;
    std::string generateDebugReport() const;
    std::string generateOptimizationReport() const;
    
    // Configuration
    void enableRealTimeMonitoring(bool enable);
    void setReportInterval(size_t interval); // seconds between reports
    void setAlertThresholds(double stepTime, size_t memory, double cpu);
    
    // Control
    void reset();
    void pause();
    void resume();
    bool isMonitoring() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
