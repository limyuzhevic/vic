#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>

namespace nlm {

// Benchmark categories
enum class BenchmarkCategory {
    CPU_PERFORMANCE,      // CPU performance benchmarks
    MEMORY_PERFORMANCE,   // Memory performance benchmarks
    THREAD_PERFORMANCE,   // Thread performance benchmarks
    IO_PERFORMANCE,       // I/O performance benchmarks
    SIMULATION_PERFORMANCE, // Simulation performance benchmarks
    HARDWARE_PERFORMANCE,  // Hardware-specific benchmarks
    SCALABILITY,          // Scalability benchmarks
    STRESS_TESTING,       // Stress testing
    LOAD_TESTING,         // Load testing
    REGRESSION_TESTING    // Regression testing
};

// Performance metrics for benchmarking
struct BenchmarkMetrics {
    std::string benchmarkName;
    BenchmarkCategory category;
    double executionTime;      // seconds
    double cpuTime;            // CPU seconds
    double userTime;           // User CPU time
    double systemTime;         // System CPU time
    size_t memoryUsage;        // Peak memory usage
    size_t allocatedMemory;    // Memory allocated
    size_t deallocatedMemory;  // Memory deallocated
    size_t operations;         // Number of operations
    double throughput;         // Operations per second
    double efficiency;         // Resource efficiency
    double scalability;        // Scaling factor
    std::chrono::high_resolution_clock::time_point timestamp;
    
    // Hardware-specific metrics
    double gpuUsage;
    double memoryBandwidth;
    double cacheHitRate;
    
    // Simulation-specific metrics
    size_t simulationSteps;
    size_t neuronCount;
    size_t synapseCount;
    double stepsPerSecond;
    double memoryPerStep;
};

// Benchmark result
struct BenchmarkResult {
    std::string benchmarkName;
    BenchmarkCategory category;
    std::vector<BenchmarkMetrics> metrics;
    double averageExecutionTime;
    double standardDeviation;
    double minimumTime;
    double maximumTime;
    double successRate;
    bool passed;
    std::string errorMessage;
};

// Benchmark comparison result
struct BenchmarkComparison {
    std::string baselineBenchmark;
    std::string testBenchmark;
    double performanceImprovement;  // Percentage improvement
    double executionTimeRatio;      // Time ratio (baseline/test)
    double memoryImprovement;       // Memory usage ratio
    bool isRegressed;               // Performance regression detected
    double confidenceLevel;         // Statistical confidence
    std::string analysis;           // Detailed analysis string
};

// Base benchmark interface
class Benchmark {
public:
    virtual ~Benchmark() = default;
    
    virtual std::string getName() const = 0;
    virtual BenchmarkCategory getCategory() const = 0;
    virtual void setup() = 0;
    virtual BenchmarkMetrics run() = 0;
    virtual void teardown() = 0;
    virtual bool validate() const = 0;
    
    virtual size_t getExpectedNeurons() const { return 1000; }
    virtual size_t getExpectedSteps() const { return 1000; }
    virtual std::string getDescription() const { return ""; }
};

// Benchmark comparison function type
using BenchmarkComparator = std::function<BenchmarkComparison(const BenchmarkMetrics&, const BenchmarkMetrics&)>;

// Benchmark suite management
class BenchmarkSuite {
private:
    std::string suiteName;
    std::vector<std::unique_ptr<Benchmark>> benchmarks;
    std::map<std::string, std::vector<BenchmarkMetrics>> baselineResults;
    std::mutex suiteMutex;
    
public:
    explicit BenchmarkSuite(const std::string& name);
    ~BenchmarkSuite() = default;
    
    void addBenchmark(std::unique_ptr<Benchmark> benchmark);
    void removeBenchmark(const std::string& name);
    bool hasBenchmark(const std::string& name) const;
    
    std::string getName() const;
    size_t getBenchmarkCount() const;
    std::vector<std::string> getBenchmarkNames() const;
    BenchmarkCategory getBenchmarkCategory(const std::string& name) const;
    
    BenchmarkMetrics runBenchmark(const std::string& name);
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    void setBaseline(const std::string& name, const BenchmarkMetrics& metrics);
    BenchmarkComparison compareWithBaseline(const std::string& name, const BenchmarkMetrics& testMetrics) const;
    std::vector<BenchmarkComparison> compareAllWithBaseline(const std::string& baselineName) const;
    
    void clearBaselines();
    bool hasBaseline(const std::string& name) const;
    
    std::vector<BenchmarkMetrics> getBaselineResults(const std::string& name) const;
    double calculateImprovement(const BenchmarkMetrics& baseline, const BenchmarkMetrics& test) const;
    bool detectRegression(const BenchmarkMetrics& baseline, const BenchmarkMetrics& test, double threshold) const;
};

// Main benchmark runner
class BenchmarkRunner {
private:
    std::string runnerName;
    std::vector<std::unique_ptr<BenchmarkSuite>> suites;
    std::map<std::string, BenchmarkComparator> comparators;
    std::atomic<bool> running;
    std::mutex runnerMutex;
    
    // Configuration
    double timeout;           // Maximum benchmark time
    size_t maxMemory;          // Maximum memory usage
    bool parallelExecution;    // Enable parallel benchmark execution
    size_t threadCount;        // Number of threads for parallel execution
    
public:
    explicit BenchmarkRunner(const std::string& name);
    ~BenchmarkRunner() = default;
    
    // Benchmark management
    void addSuite(std::unique_ptr<BenchmarkSuite> suite);
    void removeSuite(const std::string& name);
    bool hasSuite(const std::string& name) const;
    
    std::string getName() const;
    size_t getSuiteCount() const;
    std::vector<std::string> getSuiteNames() const;
    
    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& suiteName, const std::string& benchmarkName);
    std::vector<BenchmarkResult> runSuite(const std::string& suiteName);
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    // Benchmark comparison
    BenchmarkComparison compareBenchmarks(const std::string& suiteName, 
                                          const std::string& baselineName,
                                          const std::string& testName);
    std::vector<BenchmarkComparison> compareSuites(const std::string& baselineSuiteName, 
                                                   const std::string& testSuiteName);
    
    // Configuration
    void setTimeout(double timeout);
    void setMaxMemory(size_t maxMemory);
    void setParallelExecution(bool enable);
    void setThreadCount(size_t count);
    
    double getTimeout() const;
    size_t getMaxMemory() const;
    bool isParallelExecution() const;
    size_t getThreadCount() const;
    
    // Benchmark comparison setup
    void addComparator(const std::string& name, BenchmarkComparator comparator);
    BenchmarkComparator getComparator(const std::string& name) const;
    
    // Results management
    void clearResults();
    std::vector<BenchmarkResult> getResults(const std::string& suiteName, const std::string& benchmarkName) const;
    std::map<std::string, std::vector<BenchmarkResult>> getAllResults() const;
    
    // Stress and load testing
    std::vector<BenchmarkResult> runStressTest(const std::string& suiteName, 
                                               size_t iterationCount = 10);
    std::vector<BenchmarkResult> runLoadTest(const std::string& suiteName,
                                             size_t concurrentLoad = 5);
    
    // Scalability testing
    std::vector<BenchmarkResult> runScalabilityTest(const std::string& suiteName,
                                                     const std::vector<size_t>& neuronCounts,
                                                     const std::vector<size_t>& stepCounts);
    
    // Performance regression testing
    bool runRegressionTest(const std::string& suiteName, double regressionThreshold = 0.05);
    std::vector<BenchmarkComparison> detectRegressions(double regressionThreshold = 0.05) const;
};

} // namespace nlm