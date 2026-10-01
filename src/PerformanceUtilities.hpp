// PerformanceUtilities.hpp
// Performance measurement and optimization tools for NLM

#ifndef NLM_PERFORMANCEUTILITIES_HPP
#define NLM_PERFORMANCEUTILITIES_HPP

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <map>

namespace nlm {

// Performance measurement types
struct PerformanceMetrics {
    std::string name;                 ///< Metric name
    double value;                     ///< Metric value
    std::string units;                ///< Units of measurement
    double min;                       ///< Minimum value (for ranges)
    double max;                       ///< Maximum value (for ranges)
    double mean;                      ///< Mean value (for statistics)
    double median;                    ///< Median value (for statistics)
    double stddev;                    ///< Standard deviation (for statistics)
    
    PerformanceMetrics(const std::string& n, double v, const std::string& u = "")
        : name(n), value(v), units(u), min(v), max(v), mean(v), median(v), stddev(0.0) {}
};

// Benchmark result structure
struct BenchmarkResult {
    std::string testName;             ///< Name of the benchmark test
    size_t neurons;                   ///< Number of neurons tested
    size_t steps;                     ///< Number of simulation steps
    double totalTime;                 ///< Total execution time in seconds
    double stepsPerSecond;            ///< Throughput (steps per second)
    double spikesPerStep;             ///< Average spikes per step
    double synapsesPerNeuron;         ///< Average synapses per neuron
    double memoryUsage;               ///< Estimated memory usage in MB
    std::vector<PerformanceMetrics> metrics; ///< Additional metrics
    
    BenchmarkResult(const std::string& name, size_t n, size_t s)
        : testName(name), neurons(n), steps(s), totalTime(0.0), stepsPerSecond(0.0),
          spikesPerStep(0.0), synapsesPerNeuron(0.0), memoryUsage(0.0) {}
};

// Profiler class for detailed performance analysis
class Profiler {
private:
    struct ProfileData {
        std::string operationName;
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::chrono::duration<double> duration;
        size_t iterationCount;
        
        ProfileData(const std::string& name) : operationName(name), iterationCount(1) {}
    };
    
    std::vector<ProfileData> profileHistory;
    std::string currentProfile;
    std::chrono::high_resolution_clock::time_point sessionStart;
    
public:
    Profiler();
    ~Profiler();
    
    void startProfile(const std::string& operation);
    void endProfile();
    void incrementIteration();
    
    void startSession();
    void endSession();
    
    const std::vector<ProfileData>& getProfileHistory() const;
    
    void clearHistory();
    
    double getTotalTime() const;
    double getAverageTime(const std::string& operation) const;
    double getMinTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    
    void printReport() const;
    void exportReport(const std::string& filename) const;
};

// Memory profiler for memory usage analysis
class MemoryProfiler {
private:
    struct MemorySnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t allocatedMemory;         ///< Memory allocated in bytes
        size_t totalMemory;            ///< Total memory used in bytes
        size_t peakMemory;             ///< Peak memory usage in bytes
        size_t objectCount;            ///< Number of objects
    };
    
    std::vector<MemorySnapshot> memoryHistory;
    
public:
    void takeSnapshot();
    const std::vector<MemorySnapshot>& getHistory() const;
    
    void printReport() const;
    void exportReport(const std::string& filename) const;
    
    double getAverageMemoryUsage() const;
    double getPeakMemoryUsage() const;
    size_t getCurrentMemoryUsage() const;
};

// Benchmark suite for systematic performance testing
class BenchmarkSuite {
private:
    std::string name;
    std::vector<BenchmarkResult> results;
    std::vector<std::pair<size_t, size_t>> configurations; ///< (neurons, steps)
    
public:
    BenchmarkSuite(const std::string& suiteName);
    
    void addConfiguration(size_t neurons, size_t steps);
    void addConfiguration(const std::pair<size_t, size_t>& config);
    
    const std::vector<BenchmarkResult>& getResults() const;
    
    void runBenchmarks();
    void runBenchmark(size_t neurons, size_t steps);
    
    void printResults() const;
    void exportResults(const std::string& filename) const;
    
    void analyzeTrends() const;
    void generateReport() const;
};

// Cache manager for performance optimization
class CacheManager {
private:
    struct CacheEntry {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t key;
        std::vector<float> data;
        size_t accessCount;
        double priority;
    };
    
    std::vector<CacheEntry> cache;
    size_t maxSize;
    double hitRate;
    
public:
    CacheManager(size_t size = 1000);
    
    bool get(const size_t& key, std::vector<float>& data);
    void put(const size_t& key, const std::vector<float>& data);
    void remove(const size_t& key);
    void clear();
    
    size_t getSize() const;
    double getHitRate() const;
    size_t getHitCount() const;
    size_t getMissCount() const;
    
    void printStats() const;
    void optimizeCache();
};

// SIMD utilities for vectorized operations
class SIMDUtils {
public:
    static void initialize();
    static bool isAvailable();
    static bool isAVX2Available();
    static bool isAVX512Available();
    
    // Vectorized mathematical operations
    static void vectorAdd(const float* a, const float* b, float* result, size_t size);
    static void vectorMultiply(const float* a, const float* b, float* result, size_t size);
    static void vectorScale(float* data, float scale, size_t size);
    static float vectorSum(const float* data, size_t size);
    static float vectorDotProduct(const float* a, const float* b, size_t size);
    
    // Neuron simulation optimizations
    static void simulateNeuronsOptimized(float* potentials, const float* currents,
                                         float threshold, float leak, size_t size, size_t steps);
    static void applySynapticInputsOptimized(float* potentials, const float* weights,
                                            const int* targets, float* inputs, size_t count);
    
    // Memory operations
    static void copyAligned(float* dest, const float* src, size_t size);
    static void zeroAligned(float* data, size_t size);
};

// Performance monitoring utilities
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    size_t totalOperations;
    double totalTime;
    size_t operationsPerSecond;
    double averageTimePerOperation;
    
public:
    PerformanceMonitor();
    
    void startTimer();
    void stopTimer();
    void incrementOperations(size_t count = 1);
    
    void reset();
    
    double getTotalTime() const;
    double getAverageTime() const;
    size_t getTotalOperations() const;
    double getOperationsPerSecond() const;
    
    void printStats() const;
    void exportStats(const std::string& filename) const;
};

// Performance analyzer for trend detection and optimization recommendations
class PerformanceAnalyzer {
public:
    static void analyzeBenchmarkResults(const std::vector<BenchmarkResult>& results);
    static std::string generateOptimizationRecommendations(const BenchmarkResult& result);
    static double calculatePerformanceScore(const BenchmarkResult& result);
    static bool detectPerformanceRegression(const BenchmarkResult& current, const BenchmarkResult& baseline);
    
    static void printTrendAnalysis(const std::vector<BenchmarkResult>& results);
    static void generatePerformanceReport(const std::vector<BenchmarkResult>& results, const std::string& filename);
};

// Global performance utilities
inline Profiler& getProfiler() {
    static Profiler profiler;
    return profiler;
}

inline MemoryProfiler& getMemoryProfiler() {
    static MemoryProfiler memoryProfiler;
    return memoryProfiler;
}

inline BenchmarkSuite& getBenchmarkSuite() {
    static BenchmarkSuite benchmarkSuite("Default Benchmark Suite");
    return benchmarkSuite;
}

inline CacheManager& getCacheManager() {
    static CacheManager cacheManager;
    return cacheManager;
}

inline PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor performanceMonitor;
    return performanceMonitor;
}

// Convenience functions
void startProfiling(const std::string& operation);
void endProfiling();
void incrementIterationCount();

void takeMemorySnapshot();

double getCurrentFPS();
double getAverageFPS();
size_t getTotalFrames();

void printPerformanceReport();
void exportPerformanceReport(const std::string& filename);

} // namespace nlm

#endif // NLM_PERFORMANCEUTILITIES_HPP