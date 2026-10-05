// Performance profiling and benchmarking infrastructure for NLM

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>

namespace nlm {

// Performance measurement result
struct PerformanceResult {
    std::string operationName;
    std::chrono::duration<double> wallTime;
    std::chrono::duration<double> cpuTime;
    size_t memoryUsage; // in bytes
    size_t peakMemoryUsage; // in bytes
    uint64_t instructionCount;
    uint64_t cacheMisses;
    float throughput; // operations per second
    
    PerformanceResult() : operationName(), wallTime(0), cpuTime(0), memoryUsage(0),
                         peakMemoryUsage(0), instructionCount(0), cacheMisses(0), throughput(0) {}
};

// Benchmarking configuration
struct BenchmarkConfig {
    std::string outputFile;
    std::string format; // "text", "json", "csv"
    std::chrono::seconds duration;
    uint32_t iterations;
    bool warmUp; // Run warm-up iterations
    bool detailed; // Collect detailed metrics
    bool parallel; // Run in parallel
    std::map<std::string, std::string> customParameters;
    
    BenchmarkConfig() : duration(60), iterations(100), warmUp(true), detailed(false), parallel(false) {}
};

// Stress testing configuration
struct StressTestConfig {
    std::chrono::seconds duration;
    float loadFactor; // 0.0 to 1.0 (target CPU/memory usage)
    std::string stressType; // "cpu", "memory", "io", "network"
    bool enableSimulation; // Run actual neural simulation
    bool checkpointFrequency; // Take checkpoints periodically
    std::string outputDirectory;
    
    StressTestConfig() : duration(300), loadFactor(0.8), enableSimulation(true), checkpointFrequency(true) {}
};

// Profiling configuration
struct ProfilingConfig {
    bool enableSampling; // Use sampling-based profiling
    float samplingRate; // 0.0 to 1.0
    bool trackMemory; // Track memory allocations
    bool trackThreads; // Track thread activity
    bool trackSystem; // Track system calls
    std::chrono::seconds recordingDuration;
    std::string outputFormat;
    
    ProfilingConfig() : enableSampling(true), samplingRate(0.1), trackMemory(true), 
                      trackThreads(true), trackSystem(false), recordingDuration(300) {}
};

// Visualization configuration
struct VisualizationConfig {
    std::string outputFile;
    std::string format; // "png", "svg", "pdf", "ascii"
    bool interactive; // Generate interactive visualization
    bool animation; // Generate animation
    std::chrono::milliseconds frameInterval;
    float scale; // Visualization scale
    std::string colorScheme; // Color palette
    bool showConnections; // Show neural connections
    bool showWeights; // Show synaptic weights
    bool showActivity; // Show neural activity
    
    VisualizationConfig() : format("png"), interactive(false), animation(false), 
                          frameInterval(100), scale(1.0), colorScheme("default"),
                          showConnections(true), showWeights(true), showActivity(true) {}
};

// Debugging configuration
struct DebugConfig {
    bool enableBreakpoints; // Enable debugger breakpoints
    bool enableWatchpoints; // Enable watchpoints
    bool enableLogging; // Enable debug logging
    std::string logLevel; // "debug", "info", "warning", "error"
    bool trackAllocation; // Track memory allocations
    bool trackExecution; // Track execution paths
    std::map<std::string, std::string> customSettings;
    
    DebugConfig() : enableBreakpoints(false), enableWatchpoints(false), enableLogging(true),
                   trackAllocation(true), trackExecution(true) {}
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Benchmark execution
    bool runBenchmark(const BenchmarkConfig& config, const std::string& testName = "benchmark");
    bool runStressTest(const StressTestConfig& config);
    bool runProfiling(const ProfilingConfig& config);
    bool runVisualization(const VisualizationConfig& config);
    bool runDebugMode(const DebugConfig& config);
    
    // Performance measurement utilities
    void startMeasurement(const std::string& operationName);
    PerformanceResult stopMeasurement();
    
    // Resource monitoring
    void startResourceMonitoring();
    std::map<std::string, double> getResourceUsage() const;
    void stopResourceMonitoring();
    
    // Performance data management
    bool saveResults(const std::string& filepath, const std::string& format = "json") const;
    bool loadResults(const std::string& filepath);
    
    // Analysis
    double calculateThroughput(const PerformanceResult& result) const;
    double calculateEfficiency(const PerformanceResult& result, const BenchmarkConfig& config) const;
    std::string generateReport(const std::vector<PerformanceResult>& results) const;
    
    // Configuration file operations
    bool saveBenchmarkConfig(const BenchmarkConfig& config, const std::string& filepath) const;
    bool loadBenchmarkConfig(BenchmarkConfig& config, const std::string& filepath);
    
    // Preset configurations
    static BenchmarkConfig createCpuBenchmarkConfig(std::chrono::seconds duration = std::chrono::seconds(60));
    static BenchmarkConfig createMemoryBenchmarkConfig(std::chrono::seconds duration = std::chrono::seconds(30));
    static BenchmarkConfig createScalabilityBenchmarkConfig(std::chrono::seconds duration = std::chrono::seconds(120));
    static StressTestConfig createMemoryStressConfig(float loadFactor = 0.9);
    static StressTestConfig createCpuStressConfig(float loadFactor = 0.8);
    static ProfilingConfig createDetailedProfilingConfig();
    static VisualizationConfig createNetworkVisualizationConfig(const std::string& outputFile = "network.png");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Performance measurement
    void recordPerformanceData(const PerformanceResult& result);
    double calculateStatistics(const std::vector<double>& values) const;
    
    // Resource monitoring
    void monitorResources(std::atomic<bool>& stopFlag);
    double getCurrentCpuUsage() const;
    double getCurrentMemoryUsage() const;
    
    // Output formatting
    std::string formatAsText(const PerformanceResult& result) const;
    std::string formatAsJson(const PerformanceResult& result) const;
    std::string formatAsCsv(const PerformanceResult& result) const;
};

} // namespace nlm
