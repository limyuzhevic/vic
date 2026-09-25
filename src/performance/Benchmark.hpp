#pragma once

/**
 * Benchmark.hpp - Performance benchmarking for neural simulation
 * 
 * Provides comprehensive performance benchmarking capabilities:
 * 
 * Features:
 * - Performance baseline establishment
 * - Regression detection and analysis
 * - Automated performance testing
 * - Statistical analysis of performance data
 * - Comparison between different configurations
 * - Performance profiling integration
 * - Continuous performance monitoring
 * - Performance threshold validation
 * - Benchmark reporting and visualization
 * 
 * Use cases:
 * - Performance regression testing
 * - Configuration optimization
 * - Hardware compatibility testing
 * - Performance capacity planning
 * - Continuous integration testing
 * 
 */

#ifndef NLM_BENCHMARK_HPP
#define NLM_BENCHMARK_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <exception>
#include "Performance.hpp"

namespace nlm {

/**
 * Benchmark result types
 */
enum class BenchmarkResultType {
    Throughput,
    Latency,
    MemoryUsage,
    CPUUtilization,
    SIMDUtilization,
    LockContention,
    CacheEfficiency,
    Custom
};

/**
 * Benchmark measurement units
 */
enum class BenchmarkUnit {
    OperationsPerSecond,
    Nanoseconds,
    Bytes,
    Percent,
    MegaBytesPerSecond,
    GigaOperationsPerSecond,
    MicroSeconds,
    Count
};

/**
 * Benchmark configuration
 */
struct BenchmarkConfig {
    std::string name;                      // Benchmark name
    std::string description;               // Benchmark description
    BenchmarkResultType resultType;         // Type of result to measure
    BenchmarkUnit unit;                     // Unit of measurement
    std::chrono::duration<double> duration; // Benchmark duration
    size_t iterations;                      // Number of iterations
    double warmupFactor;                    // Fraction of time for warmup (0.0-1.0)
    bool enableStatisticalAnalysis;         // Enable statistical analysis
    size_t minSamplesForAnalysis;           // Minimum samples for statistical analysis
    double confidenceLevel;                 // Confidence level for confidence intervals
    std::string performanceThreshold;      // Performance threshold identifier
    std::function<bool(double)> isPassThreshold; // Threshold validation function
    
    BenchmarkConfig()
        : resultType(BenchmarkResultType::Throughput)
        , unit(BenchmarkUnit::OperationsPerSecond)
        , duration(std::chrono::seconds(10))
        , iterations(100)
        , warmupFactor(0.1)
        , enableStatisticalAnalysis(true)
        , minSamplesForAnalysis(30)
        , confidenceLevel(0.95)
        , performanceThreshold("") {
        isPassThreshold = [](double value) { return value > 0.0; };
    }
};

/**
 * Benchmark data point
 */
struct BenchmarkDataPoint {
    std::chrono::high_resolution_clock::time_point timestamp;
    size_t iteration;
    double value;                           // Measured value
    double standardDeviation;               // Standard deviation (if statistical analysis enabled)
    std::chrono::duration<double> duration; // Actual measurement duration
    std::string threadId;                   // Thread ID
    size_t threadAffinity;                  // Thread affinity
    
    // Statistical fields
    double mean;                           // Mean value
    double variance;                       // Variance
    double skewness;                        // Skewness
    double kurtosis;                        // Kurtosis
    
    BenchmarkDataPoint(size_t iter, double val, const std::chrono::duration<double>& dur)
        : timestamp(std::chrono::high_resolution_clock::now())
        , iteration(iter)
        , value(val)
        , standardDeviation(0.0)
        , duration(dur)
        , threadId("main")
        , threadAffinity(0)
        , mean(val)
        , variance(0.0)
        , skewness(0.0)
        , kurtosis(0.0) {}
    
    double getValue() const { return value; }
    
    std::string getValueString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << value;
        return oss.str();
    }
    
    std::string getUnitString() const {
        switch (unit) {
            case BenchmarkUnit::OperationsPerSecond: return "ops/sec";
            case BenchmarkUnit::Nanoseconds: return "ns";
            case BenchmarkUnit::Bytes: return "bytes";
            case BenchmarkUnit::Percent: return "%";
            case BenchmarkUnit::MegaBytesPerSecond: return "MB/s";
            case BenchmarkUnit::GigaOperationsPerSecond: return "GOps";
            case BenchmarkUnit::MicroSeconds: return "µs";
            case BenchmarkUnit::Count: return "count";
            default: return "";
        }
    }
};

/**
 * Benchmark statistics
 */
struct BenchmarkStatistics {
    std::string benchmarkName;              // Name of the benchmark
    BenchmarkResultType resultType;         // Type of result measured
    BenchmarkUnit unit;                     // Unit of measurement
    
    // Basic statistics
    double minValue{0.0};                   // Minimum value
    double maxValue{0.0};                   // Maximum value
    double mean{0.0};                       // Mean value
    double median{0.0};                     // Median value
    double standardDeviation{0.0};          // Standard deviation
    double variance{0.0};                   // Variance
    double skewness{0.0};                   // Skewness
    double kurtosis{0.0};                   // Kurtosis
    
    // Advanced statistics
    double geometricMean{0.0};              // Geometric mean
    double harmonicMean{0.0};               // Harmonic mean
    double mode{0.0};                       // Mode
    
    // Error metrics
    double confidenceInterval{0.0};         // Confidence interval width
    double standardError{0.0};              // Standard error of the mean
    double relativeError{0.0};              // Relative error
    
    // Performance metrics
    double coefficientOfVariation{0.0};     // Coefficient of variation (CV)
    double interquartileRange{0.0};         // Interquartile range
    double outlierPercentage{0.0};         // Percentage of outliers
    
    // Sample information
    size_t sampleCount{0};                   // Number of samples
    std::chrono::duration<double> totalDuration; // Total benchmark duration
    size_t iterationsCompleted{0};           // Number of completed iterations
    
    // Threshold analysis
    bool isPassThreshold{false};             // Passed threshold check
    double thresholdValue{0.0};             // Threshold value
    double thresholdDifference{0.0};        // Difference from threshold
    
    // Quality metrics
    bool isConverged{false};                 // Converged (stable results)
    double convergenceMetric{0.0};          // Convergence metric
    
    BenchmarkStatistics() = default;
    
    void reset() {
        minValue = maxValue = mean = median = standardDeviation = variance = 0.0;
        skewness = kurtosis = geometricMean = harmonicMean = mode = 0.0;
        confidenceInterval = standardError = relativeError = 0.0;
        coefficientOfVariation = interquartileRange = outlierPercentage = 0.0;
        sampleCount = 0;
        totalDuration = std::chrono::duration<double>::zero();
        iterationsCompleted = 0;
        isPassThreshold = false;
        thresholdValue = 0.0;
        thresholdDifference = 0.0;
        isConverged = false;
        convergenceMetric = 0.0;
    }
    
    double getCoefficientOfVariation() const {
        return mean > 0.0 ? (standardDeviation / mean) * 100.0 : 0.0;
    }
    
    double getQualityScore() const {
        double cv = getCoefficientOfVariation();
        double outlierPenalty = outlierPercentage / 100.0;
        double convergencePenalty = isConverged ? 0.0 : 1.0;
        
        double quality = 100.0;
        quality *= std::exp(-cv / 10.0);          // Penalize high CV
        quality *= (1.0 - outlierPenalty);       // Penalize outliers
        quality *= (1.0 - convergencePenalty);  // Penalize non-convergence
        
        return std::max(0.0, quality);
    }
};

/**
 * Benchmark comparison
 */
struct BenchmarkComparison {
    std::string baselineName;                // Baseline benchmark name
    std::string currentName;                 // Current benchmark name
    double performanceChange{0.0};           // Performance change percentage
    double throughputChange{0.0};            // Throughput change percentage
    double latencyChange{0.0};               // Latency change percentage
    double memoryChange{0.0};                // Memory change percentage
    std::string trend;                       // Trend: "improving", "degrading", "stable"
    
    bool isSignificantChange() const {
        return std::abs(performanceChange) > 5.0 || std::abs(throughputChange) > 5.0;
    }
    
    std::string getChangeDescription() const {
        if (!isSignificantChange()) {
            return "No significant change";
        }
        
        std::string result;
        if (performanceChange > 0) {
            result = "Improvement: " + std::to_string(performanceChange) + "% faster";
        } else {
            result = "Regression: " + std::to_string(std::abs(performanceChange)) + "% slower";
        }
        
        if (latencyChange != 0) {
            result += ", Latency: " + std::to_string(latencyChange) + "% " +
                     (latencyChange > 0 ? "higher" : "lower");
        }
        
        return result;
    }
};

/**
 * Benchmark suite for multiple related benchmarks
 */
class BenchmarkSuite {
public:
    struct SuiteConfig {
        std::string name;                      // Suite name
        std::string description;               // Suite description
        bool enableParallelExecution;           // Enable parallel benchmark execution
        size_t maxConcurrentBenchmarks;         // Maximum concurrent benchmarks
        double suiteDuration;                   // Suite duration (if 0, use individual benchmark durations)
        bool enableAutoCleanup;                 // Enable automatic cleanup
        std::string outputDirectory;            // Output directory for results
        
        SuiteConfig()
            : enableParallelExecution(true)
            , maxConcurrentBenchmarks(4)
            , suiteDuration(0.0)
            , enableAutoCleanup(true)
            , outputDirectory(".") {}
    };
    
    BenchmarkSuite(const SuiteConfig& config = SuiteConfig());
    ~BenchmarkSuite();
    
    // Disable copying
    BenchmarkSuite(const BenchmarkSuite&) = delete;
    BenchmarkSuite& operator=(const BenchmarkSuite&) = delete;
    
    /**
     * Add a benchmark to the suite
     */
    void addBenchmark(const std::string& name,
                     std::function<double(size_t)> benchmarkFunc,
                     const BenchmarkConfig& config = BenchmarkConfig());
    
    /**
     * Run all benchmarks in the suite
     */
    std::map<std::string, BenchmarkStatistics> runSuite();
    
    /**
     * Run a single benchmark
     */
    BenchmarkStatistics runBenchmark(const std::string& name);
    
    /**
     * Get benchmark statistics for a specific benchmark
     */
    BenchmarkStatistics getBenchmarkStats(const std::string& name) const;
    
    /**
     * Get all benchmark names
     */
    std::vector<std::string> getBenchmarkNames() const;
    
    /**
     * Compare benchmarks with a baseline
     */
    BenchmarkComparison compareWithBaseline(const std::string& baselineName, 
                                           const std::string& currentName);
    
    /**
     * Export benchmark results to JSON
     */
    std::string exportToJSON(const std::string& benchmarkName) const;
    
    /**
     * Export all benchmark results to JSON
     */
    std::string exportAllToJSON() const;
    
    /**
     * Export benchmark results to CSV
     */
    std::string exportToCSV(const std::string& benchmarkName) const;
    
    /**
     * Export all benchmark results to CSV
     */
    std::string exportAllToCSV() const;
    
    /**
     * Generate benchmark report
     */
    std::string generateReport(const std::string& benchmarkName) const;
    
    /**
     * Generate suite report
     */
    std::string generateSuiteReport() const;
    
    /**
     * Clear all benchmark results
     */
    void clearResults();
    
    /**
     * Check if benchmark exists
     */
    bool hasBenchmark(const std::string& name) const;
    
    /**
     * Remove a benchmark
     */
    void removeBenchmark(const std::string& name);
    
private:
    SuiteConfig config_;
    
    struct BenchmarkInfo {
        std::string name;
        std::function<double(size_t)> func;
        BenchmarkConfig config;
        BenchmarkStatistics stats;
        std::vector<BenchmarkDataPoint> dataPoints;
        
        BenchmarkInfo() = default;
        BenchmarkInfo(const std::string& n, std::function<double(size_t)> f, const BenchmarkConfig& c)
            : name(n), func(std::move(f)), config(c) {}
    };
    
    std::unordered_map<std::string, BenchmarkInfo> benchmarks_;
    mutable std::mutex mutex_;
    
    /**
     * Run a single benchmark iteration
     */
    BenchmarkDataPoint runBenchmarkIteration(BenchmarkInfo& benchmarkInfo, size_t iteration);
    
    /**
     * Calculate statistics for benchmark data points
     */
    BenchmarkStatistics calculateStatistics(const std::vector<BenchmarkDataPoint>& dataPoints) const;
    
    /**
     * Calculate statistical measures
     */
    struct StatisticalMeasures {
        double mean{0.0};
        double variance{0.0};
        double standardDeviation{0.0};
        double median{0.0};
        double skewness{0.0};
        double kurtosis{0.0};
        double geometricMean{0.0};
        double harmonicMean{0.0};
        
        StatisticalMeasures(double m, double v, double sd, double med, double skew, double kurt,
                          double gm, double hm)
            : mean(m), variance(v), standardDeviation(sd), median(med), skewness(skew), kurtosis(kurt),
              geometricMean(gm), harmonicMean(hm) {}
    };
    
    /**
     * Calculate statistical measures from data
     */
    StatisticalMeasures calculateStatisticalMeasures(const std::vector<double>& values) const;
    
    /**
     * Determine if data is converged
     */
    bool isConverged(const std::vector<double>& values, double threshold = 0.01) const;
    
    /**
     * Calculate confidence interval
     */
    double calculateConfidenceInterval(const std::vector<double>& values, double confidence = 0.95) const;
    
    /**
     * Detect outliers using IQR method
     */
    std::vector<bool> detectOutliers(const std::vector<double>& values) const;
    
    /**
     * Export results to file
     */
    void exportResultsToFile(const std::string& filename, const std::map<std::string, BenchmarkStatistics>& stats) const;
    
    /**
     * Create output directory if it doesn't exist
     */
    void ensureOutputDirectory() const;
};

/**
 * Global benchmark suite instance
 */
inline BenchmarkSuite& gBenchmarkSuite = BenchmarkSuite::getInstance();

/**
 * Convenience functions for common benchmarks
 */
namespace BenchmarkUtils {
    
    /**
     * Create a benchmark that measures memory pool performance
     */
    inline double memoryPoolBenchmark(size_t iterations) {
        // Simulate memory pool operations
        volatile double dummy = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            dummy += i * 1.0 / (i + 1.0);
        }
        return dummy;
    }
    
    /**
     * Create a benchmark that measures thread pool performance
     */
    inline double threadPoolBenchmark(size_t iterations) {
        // Simulate thread pool operations
        volatile double dummy = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            for (size_t j = 0; j < 100; ++j) {
                dummy += i * j * 0.001;
            }
        }
        return dummy;
    }
    
    /**
     * Create a benchmark that measures SIMD operations
     */
    inline double simdBenchmark(size_t iterations) {
        // Simulate SIMD operations
        volatile double dummy = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            for (size_t j = 0; j < 1024; ++j) {
                dummy += std::sin(j * 0.01) * std::cos(j * 0.02);
            }
        }
        return dummy;
    }
    
    /**
     * Create a benchmark that measures allocation/deallocation performance
     */
    inline double allocationBenchmark(size_t iterations) {
        // Simulate allocation/deallocation
        std::vector<double> memory;
        memory.reserve(iterations * 10);
        
        for (size_t i = 0; i < iterations; ++i) {
            memory.push_back(i * 1.0);
            if (i % 10 == 0) {
                memory.clear();
                memory.reserve(iterations * 10);
            }
        }
        
        return memory.size();
    }
    
    /**
     * Create a benchmark that measures concurrent operations
     */
    inline double concurrentBenchmark(size_t iterations) {
        // Simulate concurrent operations
        volatile double result = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            for (size_t j = 0; j < 10; ++j) {
                result += i * j * 0.0001;
            }
        }
        return result;
    }
    
    /**
     * Create a benchmark that measures event processing
     */
    inline double eventProcessingBenchmark(size_t iterations) {
        // Simulate event processing
        volatile double result = 0.0;
        for (size_t i = 0; i < iterations; ++i) {
            // Simulate event processing with variable delays
            for (size_t j = 0; j < 50 + (i % 20); ++j) {
                result += std::sin(i * 0.1 + j * 0.01);
            }
        }
        return result;
    }
}

/**
 * Convenience macros for benchmarking
 */
#define BENCHMARK(name, func, config) gBenchmarkSuite.addBenchmark(name, func, config)

#define BENCHMARK_SIMPLE(name, func) BENCHMARK(name, [func](size_t) { return func(); }, BenchmarkConfig())

#define BENCHMARK_MEASURE(name, func, iterations) BENCHMARK(name, [func](size_t it) { 
    return func(it); }, BenchmarkConfig{iterations: iterations})

} // namespace nlm

#endif // NLM_BENCHMARK_HPP