// PerformanceProfiler.h
// Core performance profiling infrastructure for NLM brain system

#ifndef NLM_PERFORMANCE_PROFILER_H
#define NLM_PERFORMANCE_PROFILER_H

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <memory>
#include <mutex>
#include <numeric>
#include <cmath>

// Forward declarations for NLM components
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class WorkingMemory;
    class EpisodicMemory;
    class PredictionSystem;
    class Neuromodulator;
}

// Performance metrics collection
struct PerformanceMetrics {
    std::string name;
    std::string category;
    
    // Timing metrics
    double minTime;           // Minimum time in microseconds
    double maxTime;           // Maximum time in microseconds
    double avgTime;           // Average time in microseconds
    double stddevTime;        // Standard deviation in microseconds
    double medianTime;        // Median time in microseconds
    double totalTime;         // Total time in microseconds
    
    // Statistical metrics
    size_t samples;           // Number of samples
    double percentile95;      // 95th percentile
    double percentile99;      // 99th percentile
    
    // Memory metrics
    size_t allocations;       // Number of memory allocations
    size_t deallocations;     // Number of memory deallocations
    size_t leaks;             // Estimated memory leaks
    size_t peakMemory;        // Peak memory usage
    size_t currentMemory;     // Current memory usage
    
    // CPU metrics
    uint64_t instructions;    // Instruction count
    uint64_t cacheMisses;     // Cache misses
    
    // Timestamp
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    PerformanceMetrics(const std::string& name = "", const std::string& category = "")
        : name(name), category(category), minTime(0.0), maxTime(0.0), avgTime(0.0), 
          stddevTime(0.0), medianTime(0.0), totalTime(0.0), samples(0), percentile95(0.0), 
          percentile99(0.0), allocations(0), deallocations(0), leaks(0), peakMemory(0), 
          currentMemory(0), instructions(0), cacheMisses(0) {}
};

// Individual profiling interval
struct ProfilingInterval {
    std::string name;
    std::string category;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    double duration;          // microseconds
    
    ProfilingInterval(const std::string& name = "", const std::string& category = "")
        : name(name), category(category), start(std::chrono::high_resolution_clock::now()), 
          end(std::chrono::high_resolution_clock::now()), duration(0.0) {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~ProfilingInterval() {
        end = std::chrono::high_resolution_clock::now();
        auto duration_us = std::chrono::duration<double, std::micro>(end - start).count();
        duration = duration_us;
    }
    
    void stop() {
        end = std::chrono::high_resolution_clock::now();
        auto duration_us = std::chrono::duration<double, std::micro>(end - start).count();
        duration = duration_us;
    }
};

// Memory tracking
struct MemoryAllocation {
    size_t size;
    void* address;
    std::string sourceFile;
    int sourceLine;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string allocationType;
    
    MemoryAllocation(size_t size, void* address, const std::string& sourceFile = "", 
                     int sourceLine = 0, const std::string& allocationType = "")
        : size(size), address(address), sourceFile(sourceFile), sourceLine(sourceLine), 
          timestamp(std::chrono::high_resolution_clock::now()), allocationType(allocationType) {}
};

// CPU profiling data
struct CPUMeasurement {
    uint64_t timestamp;
    uint64_t instructionsRetired;
    uint64_t cacheMissesL1;
    uint64_t cacheMissesL2;
    uint64_t cacheMissesL3;
    uint64_t cycles;
    uint64_t stalledCyclesFrontend;
    uint64_t stalledCyclesBackend;
    
    CPUMeasurement() : timestamp(0), instructionsRetired(0), cacheMissesL1(0), 
                      cacheMissesL2(0), cacheMissesL3(0), cycles(0), 
                      stalledCyclesFrontend(0), stalledCyclesBackend(0) {}
};

// Configuration for performance profiling
struct PerformanceProfilerConfig {
    bool enableTiming = true;                    // Enable timing measurements
    bool enableMemoryTracking = true;           // Enable memory tracking
    bool enableCPUTracking = false;             // Enable CPU instruction counting
    bool enableStatisticalAnalysis = true;       // Enable statistical analysis
    bool enableVisualOutput = true;             // Enable console output
    bool enableExport = true;                   // Enable file export
    bool nonIntrusiveMode = true;               // Minimize impact on performance
    bool profileSystemCalls = false;            // Profile system calls
    std::string exportPath = "./profiles";      // Directory for profile exports
    std::string exportFormat = "txt";          // txt, csv, json
    size_t maxMemorySamples = 1000;            // Maximum memory samples to track
    size_t maxCPUSamples = 1000;               // Maximum CPU samples to track
    bool trackLeaks = true;                     // Track memory leaks
    bool trackAllocations = true;               // Track memory allocations
    bool trackDeallocations = true;             // Track memory deallocations
    
    // Brain component profiling
    bool profileBrainInitialization = true;
    bool profileBrainStep = true;
    bool profileNeuronLIF = true;
    bool profileSynapticTransmission = true;
    bool profileMemorySystems = true;
    bool profilePrediction = true;
    bool profileNeuromodulation = true;
    bool profilePlasticity = true;
    bool profileDevelopment = true;
    
    // Filtering
    std::vector<std::string> includeCategories; // Categories to include
    std::vector<std::string> excludeCategories; // Categories to exclude
    
    PerformanceProfilerConfig() {}
};

// Core performance profiler class
class PerformanceProfiler {
private:
    PerformanceProfilerConfig config;
    
    // Thread safety
    mutable std::mutex dataMutex;
    
    // Profiling data storage
    std::vector<PerformanceInterval> activeIntervals;
    std::vector<ProfilingInterval> completedIntervals;
    std::vector<PerformanceMetrics> metrics;
    std::vector<MemoryAllocation> memoryAllocations;
    std::vector<MemoryAllocation> memoryDeallocations;
    std::vector<CPUMeasurement> cpuMeasurements;
    
    // Statistics
    double totalProfileTime;
    size_t totalOperations;
    
    // Internal helper methods
    void calculateStatistics(PerformanceMetrics& metrics) const;
    void exportMetrics(const std::string& filename) const;
    void printHeader(const std::string& title) const;
    void printMetrics(const PerformanceMetrics& metrics) const;
    bool shouldProfile(const std::string& category) const;
    
    // CPU measurement helpers
    void measureCPU();
    
public:
    PerformanceProfiler(const PerformanceProfilerConfig& config = PerformanceProfilerConfig());
    ~PerformanceProfiler();
    
    // Core profiling interface
    ProfilingInterval* startInterval(const std::string& name, const std::string& category = "general");
    void stopInterval(ProfilingInterval* interval);
    void endInterval(ProfilingInterval* interval);
    
    // Metrics collection
    void recordMetric(const PerformanceMetrics& metric);
    void recordTiming(const std::string& name, const std::string& category, 
                     double durationMicroseconds);
    
    // Memory tracking
    void recordMemoryAllocation(size_t size, void* address, const std::string& sourceFile = "", 
                               int sourceLine = 0, const std::string& allocationType = "");
    void recordMemoryDeallocation(void* address);
    void analyzeMemoryLeaks();
    
    // Brain system profiling
    void profileBrainInitialization(nlm::Brain* brain);
    void profileBrainStep(nlm::Brain* brain, size_t steps);
    void profileNeuronLIF(nlm::Neuron* neuron);
    void profileSynapticTransmission(nlm::Synapse* synapse);
    void profileMemoryOperation(const std::string& operation, size_t size, double duration);
    
    // Statistical operations
    PerformanceMetrics getAggregatedMetrics(const std::string& category) const;
    void printSummaryReport() const;
    void printDetailedReport() const;
    
    // Export functionality
    void exportToFile(const std::string& filename);
    void exportToCSV(const std::string& filename) const;
    void exportToJSON(const std::string& filename) const;
    
    // Configuration
    void setConfig(const PerformanceProfilerConfig& newConfig);
    const PerformanceProfilerConfig& getConfig() const { return config; }
    
    // Utility methods
    void clear();
    size_t getActiveIntervalCount() const { return activeIntervals.size(); }
    size_t getMetricCount() const { return metrics.size(); }
    double getTotalProfileTime() const { return totalProfileTime; }
    
    // Singleton pattern for global access
    static PerformanceProfiler& getInstance(const PerformanceProfilerConfig& config = PerformanceProfilerConfig());
    static void cleanupInstance();
};

// Global function for easy profiling
namespace PerformanceProfilerUtils {
    template<typename Func>
    auto timeFunction(const std::string& name, const std::string& category, Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double, std::micro>(end - start).count();
        PerformanceProfiler::getInstance().recordTiming(name, category, duration);
        
        return result;
    }
    
    template<typename Func>
    auto profileScope(const std::string& name, const std::string& category, Func&& func) {
        ProfilingInterval interval(name, category);
        return func();
    }
}

#endif // NLM_PERFORMANCE_PROFILER_H