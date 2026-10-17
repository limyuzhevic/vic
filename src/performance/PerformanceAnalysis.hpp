// Performance benchmarking and profiling for NLM
// Comprehensive performance analysis for power users

#ifndef NLM_PERFORMANCE_ANALYSIS_HPP
#define NLM_PERFORMANCE_ANALYSIS_HPP

#include "core/Config/Config.hpp"
#include "experiments/Metrics.hpp"
#include <chrono>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <unordered_map>

namespace nlm {

// Performance measurement utilities
struct BenchmarkResult {
    std::string benchmarkName;
    double wallClockTime;          // Total wall clock time
    double cpuTime;               // CPU time only
    double userTime;              // User CPU time
    double systemTime;            // System CPU time
    size_t memoryPeak;            // Peak memory usage in bytes
    size_t memoryAllocated;       // Total allocated memory
    double instructionsPerSecond; // Instruction throughput
    double flopsPerSecond;        // Floating point operations per second
    double cacheMissRate;         // Cache miss rate
    int threadCount;              // Number of threads used
    double efficiency;            // Parallel efficiency
    
    BenchmarkResult() : wallClockTime(0.0), cpuTime(0.0), userTime(0.0), systemTime(0.0),
                       memoryPeak(0), memoryAllocated(0), instructionsPerSecond(0.0),
                       flopsPerSecond(0.0), cacheMissRate(0.0), threadCount(1), efficiency(0.0) {}
};

// Detailed profiling information
struct ProfileInfo {
    std::string functionName;
    double timeSpent;             // Time spent in function
    double cpuTimeSpent;          // CPU time in function
    uint64_t callCount;           // Number of calls
    uint64_t memoryAllocated;     // Memory allocated
    uint64_t cacheMisses;         // Cache misses
    double cpuUsage;              // CPU usage percentage
    double memoryUsage;           // Memory usage in MB
    std::vector<double> times;    // Detailed timing data
    std::map<std::string, double> metrics; // Additional metrics
    
    ProfileInfo(const std::string& name = "") : functionName(name), timeSpent(0.0), 
                                               cpuTimeSpent(0.0), callCount(0), 
                                               memoryAllocated(0), cacheMisses(0),
                                               cpuUsage(0.0), memoryUsage(0.0) {}
};

// Memory tracking for leak detection
struct MemoryAllocation {
    void* address;
    size_t size;
    std::string allocationSite;
    uint64_t timestamp;
    int callStack[10];
    int stackDepth;
};

// CPU profiling data
struct CPUBenchmark {
    double timePerStep;           // Average time per simulation step
    double stepsPerSecond;        // Simulation steps per second
    double cpuUtilization;        // CPU utilization percentage
    double instructionThroughput; // Instructions per second
    double branchMispredictionRate; // Branch misprediction rate
    std::vector<double> stepTimes; // Detailed step timing
};

// Network profiling
struct NetworkProfile {
    double bytesSent;             // Total bytes sent
    double bytesReceived;         // Total bytes received
    double packetsSent;           // Total packets sent
    double packetsReceived;       // Total packets received
    double networkLatency;        // Network latency
    double bandwidth;             // Network bandwidth
    double messageSize;           // Average message size
};

// GPU profiling
struct GPUBenchmark {
    double kernelExecutionTime;   // Total kernel execution time
    size_t globalMemoryUsed;      // Global memory usage
    size_t sharedMemoryUsed;      // Shared memory usage
    size_t localMemoryUsed;       // Local memory usage
    double memoryBandwidth;       // Memory bandwidth
    double computeCapability;     // GPU compute capability
    double utilization;           // GPU utilization percentage
    std::vector<double> kernelTimes; // Individual kernel times
};

// Main performance analysis class
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Start benchmarking
    void startBenchmark(const std::string& name);
    void stopBenchmark(const std::string& name);
    
    // Detailed profiling
    void startProfiling(const std::string& functionName);
    void stopProfiling(const std::string& functionName);
    
    // Memory profiling
    void trackMemoryAllocation(void* address, size_t size, const std::string& site);
    void trackMemoryDeallocation(void* address);
    bool detectMemoryLeaks();
    std::vector<MemoryAllocation> getMemoryLeaks();
    
    // CPU profiling
    CPUBenchmark getCPUBenchmark() const;
    std::map<std::string, ProfileInfo> getProfileData() const;
    
    // Network profiling
    void trackNetworkTraffic(double bytesSent, double bytesReceived, double latency);
    NetworkProfile getNetworkProfile() const;
    
    // GPU profiling
    void trackGPUMetrics(const GPUBenchmark& metrics);
    GPUBenchmark getGPUBenchmark() const;
    
    // Performance metrics
    BenchmarkResult getBenchmarkResult(const std::string& name) const;
    std::vector<BenchmarkResult> getAllBenchmarkResults() const;
    
    // Comparison and analysis
    void compareBenchmarks(const std::string& benchmark1, const std::string& benchmark2);
    void generatePerformanceReport(const std::string& outputFile = "performance_report.html");
    
    // Configuration
    void configure(int threadCount = -1, bool enableProfiling = true, 
                   bool enableMemoryTracking = true, bool enableCPUTiming = true);
    
    // Results management
    void saveResults(const std::string& filename) const;
    void loadResults(const std::string& filename);
    void clearResults();
    
    // Experiment integration
    void attachToBrainSimulation(std::shared_ptr<Brain> brain);
    void attachToExperiment(std::shared_ptr<Experiment> experiment);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Specialized benchmarking functions
BenchmarkResult benchmarkSimulation(const std::string& name, 
                                    std::function<void()> simulationFunction,
                                    const Config& config = Config());

BenchmarkResult benchmarkNeuronOperations(size_t neuronCount, size_t steps);
BenchmarkResult benchmarkSynapseOperations(size_t synapseCount, size_t steps);
BenchmarkResult benchmarkMemoryAllocation(size_t size, size_t iterations);
BenchmarkResult benchmarkThreading(int threadCount, size_t workload);

// Performance comparison utilities
class PerformanceComparator {
public:
    PerformanceComparator();
    ~PerformanceComparator();
    
    void addBenchmark(const std::string& name, const BenchmarkResult& result);
    void compareAll();
    void generateComparisonReport(const std::string& outputFile = "comparison_report.html");
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values, double mean) const;
    double calculateConfidenceInterval(const std::vector<double>& values, double confidence = 0.95) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_ANALYSIS_HPP
