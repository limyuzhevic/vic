// Benchmark.hpp - Performance benchmarking tools
#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <functional>
#include <numeric>
#include <algorithm>
#include <random>

namespace nlm {

/**
 * Benchmark - Performance benchmarking tools for NLM
 * 
 * Provides comprehensive performance benchmarking including:
 * - Performance comparison and benchmarking
 * - Performance regression testing
 * - Scalability analysis
 * - Benchmark suite management
 * - Statistical analysis of performance data
 * - Benchmark result visualization and reporting
 * 
 * Features:
 * - Run multiple benchmarks with different configurations
 * - Statistical analysis of benchmark results
 * - Performance regression detection
 * - Scalability analysis across different input sizes
 * - Benchmark warm-up and cooldown management
 * - Result comparison and validation
 * 
 * Integration with existing systems:
 * - Batch processing for comparative analysis
 * - Existing logging system for benchmark output
 * - Configuration system for benchmark settings
 * 
 * Output formats:
 * - Console output with formatted benchmark reports
 * - File output in various formats (JSON, CSV, HTML)
 * 
 * @see PerformanceProfiler
 * @see Visualization
 */
class Benchmark {
public:
    /**
     * Benchmark result
     */
    struct BenchmarkResult {
        std::string benchmarkName;
        double duration;              // milliseconds
        double cpuTime;               // milliseconds
        double memoryUsage;           // bytes
        double throughput;            // operations per second
        double quality;               // 0.0 to 1.0
        std::unordered_map<std::string, double> metrics;
        std::chrono::steady_clock::time_point timestamp;
        bool passed;
        std::string errorMessage;
    };

    /**
     * Benchmark configuration
     */
    struct BenchmarkConfig {
        std::string name;
        std::function<void()> setup;       // Setup function
        std::function<void()> teardown;    // Teardown function
        std::function<void()> benchmark;   // Actual benchmark function
        int warmupIterations = 5;
        int benchmarkIterations = 10;
        int cooldownIterations = 5;
        double timeoutSeconds = 60.0;
        bool autoScale = false;
        double minExpectedDuration = 0.0;
        double maxExpectedDuration = 1000.0;
        bool verbose = false;
        std::string resultFile;
        std::string comparisonBaseline;
        bool enableStatistics = true;
        bool enableRegressionDetection = true;
    };

    /**
     * Benchmark statistics
     */
    struct BenchmarkStats {
        double mean;
        double median;
        double stdDev;
        double min;
        double max;
        double iqr; // Interquartile range
        double p95; // 95th percentile
        double p99; // 99th percentile
        double skew;
        double kurtosis;
    };

    /**
     * Benchmark constructor
     */
    Benchmark();
    
    /**
     * Benchmark constructor with configuration
     */
    Benchmark(const BenchmarkConfig& config);
    
    /**
     * Benchmark destructor
     */
    ~Benchmark();
    
    /**
     * Add benchmark to suite
     */
    void addBenchmark(const BenchmarkConfig& config);
    
    /**
     * Run single benchmark
     */
    BenchmarkResult runBenchmark(const BenchmarkConfig& config);
    
    /**
     * Run benchmark suite
     */
    std::vector<BenchmarkResult> runSuite();
    
    /**
     * Run benchmark with specific name
     */
    BenchmarkResult runBenchmark(const std::string& name);
    
    /**
     * Get benchmark results
     */
    const std::vector<BenchmarkResult>& getResults() const { return results; }
    
    /**
     * Clear all results
     */
    void clearResults();
    
    /**
     * Calculate benchmark statistics
     */
    BenchmarkStats calculateStats(const std::vector<double>& values) const;
    
    /**
     * Compare benchmark with baseline
     */
    std::string compareWithBaseline(const BenchmarkResult& current,
                                   const BenchmarkResult& baseline) const;
    
    /**
     * Detect performance regression
     */
    bool detectRegression(const BenchmarkResult& current,
                        const BenchmarkResult& baseline) const;
    
    /**
     * Generate benchmark report
     */
    std::string generateReport(const std::string& format = "auto") const;
    
    /**
     * Export benchmark data to file
     */
    bool exportData(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Generate benchmark suite report
     */
    std::string generateSuiteReport(const std::string& format = "auto") const;
    
    /**
     * Get benchmark suite statistics
     */
    std::string getSuiteStatistics() const;
    
    /**
     * Set benchmark timeout
     */
    void setTimeout(double timeoutSeconds) { defaultConfig.timeoutSeconds = timeoutSeconds; }
    
    /**
     * Enable auto-scaling
     */
    void enableAutoScale() { defaultConfig.autoScale = true; }

private:
    std::vector<BenchmarkConfig> benchmarkSuite;
    std::vector<BenchmarkResult> results;
    BenchmarkConfig defaultConfig;
    
    mutable std::mutex dataMutex;
    
    void runSingleIteration(const BenchmarkConfig& config, int iteration);
    double measureDuration(const std::function<void()>& func) const;
    bool validateResult(const BenchmarkResult& result) const;
    
    // Suppress warnings about unused private members
    Benchmark(const Benchmark&) = delete;
    Benchmark& operator=(const Benchmark&) = delete;
};

} // namespace nlm