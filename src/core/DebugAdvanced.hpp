#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <sstream>
#include "Logger.hpp"

namespace nlm {

// Debug categories for structured logging
enum class DebugCategory {
    Performance,
    Memory,
    NeuralState,
    Network,
    Trace,
    PatternAnalysis,
    Benchmark,
    Custom
};

// Debug level for enhanced debugging
enum class DebugLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Performance measurement data
struct PerformanceMetrics {
    std::string name;
    std::chrono::duration<double> duration;
    double memory_usage;
    int cpu_cycles;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, double> custom_metrics;
};

// Memory tracking data
struct MemoryInfo {
    size_t total_allocated;
    size_t current_usage;
    size_t peak_usage;
    std::unordered_map<void*, size_t> allocations;
    std::chrono::system_clock::time_point timestamp;
    double leak_threshold_mb;
};

// Neural state snapshot
struct NeuralStateSnapshot {
    uint64_t timestep;
    std::string region_name;
    std::vector<double> neuron_states;
    std::vector<std::vector<double>> synapse_weights;
    std::chrono::system_clock::time_point timestamp;
    std::string correlation_id;
};

// Structured log entry
struct StructuredLogEntry {
    std::string correlation_id;
    std::string category;
    DebugLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> fields;
    std::optional<PerformanceMetrics> perf_data;
    std::optional<MemoryInfo> memory_data;
    std::optional<NeuralStateSnapshot> neural_state_data;
};

// Advanced logger interface with structured logging support
class IAdvancedLogger : public ILogger {
public:
    virtual ~IAdvancedLogger() = default;
    virtual void logStructured(const StructuredLogEntry& entry) = 0;
    virtual void setCategoryLevel(const std::string& category, DebugLevel level) = 0;
    virtual void addCategory(const std::string& category) = 0;
    virtual std::vector<std::string> getCategories() const = 0;
    virtual void enableJSONOutput(bool enable) = 0;
    virtual void setCorrelationID(const std::string& id) = 0;
    virtual std::string getCurrentCorrelationID() const = 0;
};

// Debug profiler for performance timing and benchmarking
class DebugProfiler {
public:
    DebugProfiler();
    ~DebugProfiler();
    
    // Start/stop profiling
    void startProfile(const std::string& name);
    void stopProfile(const std::string& name);
    PerformanceMetrics getLastProfile(const std::string& name) const;
    
    // Benchmark utilities
    void runBenchmark(const std::string& name, std::function<void()> func);
    std::vector<PerformanceMetrics> getBenchmarkResults() const;
    
    // Integration with logging
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory tracker for memory usage monitoring and leak detection
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Memory tracking
    void trackAllocation(void* ptr, size_t size);
    void trackDeallocation(void* ptr);
    MemoryInfo getMemoryInfo() const;
    
    // Leak detection
    std::vector<void*> detectLeaks() const;
    void setLeakThreshold(double threshold_mb);
    
    // Integration with logging
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural state snapshot for brain state tracking and comparison
class NeuralStateSnapshotter {
public:
    NeuralStateSnapshotter();
    ~NeuralStateSnapshotter();
    
    // Take snapshots
    void takeSnapshot(const std::string& region_name);
    NeuralStateSnapshot getLastSnapshot(const std::string& region_name) const;
    
    // Comparison
    double compareSnapshots(const std::string& region_name, 
                          const NeuralStateSnapshot& snapshot1,
                          const NeuralStateSnapshot& snapshot2) const;
    
    // Correlation tracking
    void setCorrelationID(const std::string& id);
    std::string getCurrentCorrelationID() const;
    
    // Integration with logging
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log analyzer for log aggregation and analysis
class LogAnalyzer {
public:
    LogAnalyzer();
    ~LogAnalyzer();
    
    // Log processing
    void addLogEntry(const StructuredLogEntry& entry);
    std::vector<StructuredLogEntry> analyzePatterns(const std::string& pattern) const;
    std::unordered_map<std::string, int> analyzeFrequency(const std::string& category) const;
    
    // Export capabilities
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    
    // Statistics
    size_t getTotalLogCount() const;
    double getErrorRate() const;
    std::chrono::duration<double> getLogFrequency(const std::string& category) const;
    
    // Integration with logging
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global debugging utilities
namespace Debug {
    // Global instances
    std::shared_ptr<DebugProfiler> getProfiler();
    std::shared_ptr<MemoryTracker> getMemoryTracker();
    std::shared_ptr<NeuralStateSnapshotter> getSnapshotter();
    std::shared_ptr<LogAnalyzer> getLogAnalyzer();
    
    // Global configuration
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger);
    void setGlobalCorrelationID(const std::string& id);
    
    // Debugging macros
    #define NLM_DEBUG_PERF(name) nlm::Debug::getProfiler()->startProfile(name)
    #define NLM_DEBUG_PERF_STOP(name) nlm::Debug::getProfiler()->stopProfile(name)
    #define NLM_DEBUG_BENCHMARK(name, func) nlm::Debug::getProfiler()->runBenchmark(name, func)
    #define NLM_TRACK_ALLOC(ptr, size) nlm::Debug::getMemoryTracker()->trackAllocation(ptr, size)
    #define NLM_TRACK_DEALLOC(ptr) nlm::Debug::getMemoryTracker()->trackDeallocation(ptr)
    #define NLM_TAKE_SNAPSHOT(region) nlm::Debug::getSnapshotter()->takeSnapshot(region)
    #define NLM_COMPARE_SNAPSHOTS(region, s1, s2) nlm::Debug::getSnapshotter()->compareSnapshots(region, s1, s2)
}

} // namespace nlm