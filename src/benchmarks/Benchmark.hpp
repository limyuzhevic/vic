#pragma once

#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <map>
#include <cstdint>

namespace nlm {
namespace benchmark {

// Performance metrics collection
struct BenchmarkMetrics {
    std::string name;
    std::string description;
    
    // Time metrics
    double wallClockTime;      // Total wall-clock time (seconds)
    double cpuTime;           // CPU time (seconds)
    double minTime;           // Minimum single-run time
    double maxTime;           // Maximum single-run time
    double avgTime;           // Average time
    double stddevTime;        // Standard deviation
    
    // Computational metrics
    uint64_t operations;      // Number of operations
    uint64_t totalSpikes;     // Total spikes generated
    uint64_t totalSteps;      // Total simulation steps
    double opsPerSecond;      // Operations per second
    double flopsPerSecond;    // Floating-point operations per second
    double spikesPerSecond;   // Spikes per second
    
    // Memory metrics
    size_t memoryBytes;       // Memory usage (bytes)
    size_t peakMemoryBytes;   // Peak memory usage (bytes)
    size_t allocationCount;   // Number of memory allocations
    size_t deallocationCount; // Number of memory deallocations
    double memoryBandwidth;   // Memory bandwidth (bytes/s)
    
    // Cache metrics
    uint64_t cacheHits;       // Cache hits
    uint64_t cacheMisses;     // Cache misses
    double cacheHitRatio;     // Cache hit ratio
    
    // Thread metrics
    uint64_t activeThreads;   // Number of active threads
    double threadUtilization; // Thread utilization ratio
    double synchronizationOverhead; // Thread synchronization overhead
    
    // SIMD metrics
    uint64_t simdOperations;  // SIMD operations performed
    uint64_t scalarOperations; // Scalar operations performed
    double simdSpeedup;       // SIMD vs scalar speedup
    double vectorizationEfficiency; // SIMD vectorization efficiency
    
    // Network metrics
    size_t neuronCount;       // Number of neurons
    size_t synapseCount;      // Number of synapses
    size_t regionCount;       // Number of regions
    double connectionDensity; // Connection density
    double averageFanout;     // Average fanout
    
    // Scalability metrics
    double timeComplexity;    // Time complexity coefficient
    double spaceComplexity;   // Space complexity coefficient
    
    BenchmarkMetrics(const std::string& n = "") 
        : name(n), wallClockTime(0.0), cpuTime(0.0), minTime(0.0), maxTime(0.0), 
          avgTime(0.0), stddevTime(0.0), opsPerSecond(0.0), flopsPerSecond(0.0),
          spikesPerSecond(0.0), memoryBytes(0), peakMemoryBytes(0), 
          allocationCount(0), deallocationCount(0), memoryBandwidth(0.0),
          cacheHits(0), cacheMisses(0), cacheHitRatio(0.0), activeThreads(0),
          threadUtilization(0.0), synchronizationOverhead(0.0), simdOperations(0),
          scalarOperations(0), simdSpeedup(0.0), vectorizationEfficiency(0.0),
          neuronCount(0), synapseCount(0), regionCount(0), connectionDensity(0.0),
          averageFanout(0.0), timeComplexity(0.0), spaceComplexity(0.0) {}
};

// Statistical analysis utilities
class Statistics {
public:
    static double mean(const std::vector<double>& values);
    static double variance(const std::vector<double>& values);
    static double standardDeviation(const std::vector<double>& values);
    static double median(const std::vector<double>& values);
    static double percentile(const std::vector<double>& values, double p);
    static std::vector<double> movingAverage(const std::vector<double>& values, size_t windowSize);
    
    // Statistical significance testing
    static double confidenceInterval(const std::vector<double>& values, double confidence = 0.95);
    static bool isSignificant(double pValue, double alpha = 0.05);
};

// Performance measurement framework
class PerformanceMonitor {
private:
    std::atomic<uint64_t> operationCount{0};
    std::atomic<double> totalMemoryUsage{0.0};
    std::atomic<uint64_t> cacheHits{0};
    std::atomic<uint64_t> cacheMisses{0};
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point cpuStartTime;
    
public:
    PerformanceMonitor() {
        startTime = std::chrono::high_resolution_clock::now();
        cpuStartTime = std::chrono::high_resolution_clock::now();
    }
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        cpuStartTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop(BenchmarkMetrics& metrics);
    
    void incrementOperations() { operationCount.fetch_add(1); }
    void incrementMemoryUsage(size_t bytes) { totalMemoryUsage.fetch_add(static_cast<double>(bytes)); }
    void recordCacheHit() { cacheHits.fetch_add(1); }
    void recordCacheMiss() { cacheMisses.fetch_add(1); }
    
    double getElapsedWallClock() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    }
    
    double getElapsedCPU() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - cpuStartTime).count();
    }
    
    double getCacheHitRatio() {
        uint64_t hits = cacheHits.load();
        uint64_t misses = cacheMisses.load();
        uint64_t total = hits + misses;
        return total > 0 ? static_cast<double>(hits) / total : 0.0;
    }
};

// Memory profiler
class MemoryProfiler {
public:
    struct AllocationInfo {
        size_t size;
        void* address;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string allocationSite;
    };
    
private:
    std::vector<AllocationInfo> allocations;
    std::mutex mutex;
    
public:
    void recordAllocation(size_t size, void* address, const std::string& site);
    void recordDeallocation(void* address);
    double calculateMemoryBandwidth(size_t totalBytes, double timeSeconds);
    size_t getPeakMemoryUsage() const;
};n
// Flame graph profiler
class FlameGraphProfiler {
public:
    struct ProfileEvent {
        std::string functionName;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::string threadId;
    };
    
private:
    std::vector<ProfileEvent> events;
    std::mutex mutex;
    
public:
    void startProfile(const std::string& functionName, const std::string& threadId = "main");
    void endProfile(const std::string& functionName, const std::string& threadId = "main");
    void generateFlameGraph(const std::string& outputPath);
    std::vector<ProfileEvent> getEvents() const { return events; }
};

// CPU cache efficiency analyzer
class CacheEfficiencyAnalyzer {
public:
    struct CacheMetrics {
        uint64_t L1Hits;
        uint64::misses;
        uint64_t L1Misses;
        uint64_t L2Hits;
        uint64_t L2Misses;
        uint64_t L3Hits;
        uint64_t L3Misses;
        uint64_t instructions;
        uint64_t branches;
        uint64_t branchMisses;
        double IPC; // Instructions per cycle
        double L1MissRate;
        double L2MissRate;
        double L3MissRate;
        double branchMissRate;
    };
    
    CacheMetrics analyzeCacheEfficiency(const std::string& benchmarkName);
    double calculateCacheEfficiency(const CacheMetrics& metrics);
};

// Scalability tester
class ScalabilityTester {
public:
    struct ScalabilityResult {
        std::string testName;
        double neurons;
        double regions;
        double connectionDensity;
        double timePerStep;
        double memoryPerNeuron;
        double scalabilityFactor;
    };
    
    std::vector<ScalabilityResult> testNeuronScalability(const std::vector<size_t>& neuronCounts);
    std::vector<ScalabilityResult> testRegionScalability(const std::vector<size_t>& regionCounts);
    std::vector<ScalabilityResult> testConnectionDensityScalability(const std::vector<double>& densities);
    double calculateTimeComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& times);
    double calculateSpaceComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& memoryUsage);
};

// Comparative benchmarking
class ComparativeBenchmarker {
public:
    struct ComparisonResult {
        std::string testName;
        std::string referenceImplementation;
        std::string currentImplementation;
        double speedup;
        double memoryImprovement;
        double qualityScore;
        double regressionRisk;
    };
    
    std::vector<ComparisonResult> compareWithReference(const std::string& referencePath);
    double calculateRegressionRisk(const std::vector<BenchmarkMetrics>& metrics);
    double calculateQualityScore(const BenchmarkMetrics& metrics);
};

// Automated benchmark runner
class BenchmarkRunner {
private:
    std::vector<BenchmarkMetrics> results;
    std::string outputDirectory;
    std::string benchmarkName;
    bool generateReports;
    bool generateCharts;
    
public:
    BenchmarkRunner(const std::string& outputDir, const std::string& name, bool reports = true, bool charts = false);
    
    void addResult(const BenchmarkMetrics& metrics);
    void runBenchmarkSuite();
    void saveResults(const std::string& format = "json");
    void generateReport();
    void generateCharts();
    
    std::vector<BenchmarkMetrics> getResults() const { return results; }
    
    // Core benchmark suites
    void runCoreBenchmarks();
    void runScalabilityBenchmarks();
    void runComparativeBenchmarks();
    void runAdvancedMetricsBenchmarks();
    void runIntegrationBenchmarks();
};

// Flame graph utilities
namespace flamegraph {
    struct FlameGraphNode {
        std::string name;
        double selfTime;
        double totalTime;
        std::vector<FlameGraphNode> children;
        std::string threadId;
    };
    
    FlameGraphNode buildFlameGraph(const std::vector<ProfileEvent>& events);
    void exportFlameGraph(const FlameGraphNode& root, const std::string& outputPath);
    void generateSVG(const FlameGraphNode& root, const std::string& outputPath);
}

// Result formatter and reporter
class ResultFormatter {
public:
    static std::string formatMetrics(const BenchmarkMetrics& metrics);
    static std::string formatComparison(const ComparativeBenchmarker::ComparisonResult& result);
    static std::string formatScalability(const ScalabilityTester::ScalabilityResult& result);
    static void saveToCSV(const std::vector<BenchmarkMetrics>& metrics, const std::string& path);
    static void saveToJSON(const std::vector<BenchmarkMetrics>& metrics, const std::string& path);
    static void saveToMarkdown(const std::vector<BenchmarkMetrics>& metrics, const std::string& path);
};

} // namespace benchmark
} // namespace nlm

#include "Benchmark.inl"