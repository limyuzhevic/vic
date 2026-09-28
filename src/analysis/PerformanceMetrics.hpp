#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../core/Types/Types.hpp"
#include "../experiments/ExperimentMetrics.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

/**
 * @brief System performance metrics class
 * 
 * This class provides comprehensive performance analysis capabilities for neural
 * network simulations, including efficiency computation, scalability metrics,
 * performance report generation, implementation comparison, and performance
 * profiling for bottleneck identification and optimization.
 * 
 * Designed for:
 * - Performance analysis - Bottleneck identification and optimization
 * - Benchmarking - Performance measurement and comparison
 * - Statistical analysis - Hypothesis testing and validation
 * - Machine learning - Performance as ML features
 * - Research visualization - Performance analysis for visualization
 */
class PerformanceMetrics {
public:
    /**
     * @brief Performance benchmark result
     */
    struct BenchmarkResult {
        std::string benchmarkName;
        float executionTime;      // seconds
        float memoryUsage;        // MB
        float cpuUsage;           // percentage
        float efficiency;         // normalized efficiency score
        std::map<std::string, float> detailedMetrics;
        SimulationStep stepsCompleted;
        float rewardPerStep;     // average reward per simulation step
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    /**
     * @brief Performance profile
     */
    struct PerformanceProfile {
        float cpuUtilization;           // average CPU usage
        float memoryUtilization;        // average memory usage
        float diskIO;                   // I/O operations per second
        float networkIO;                // network throughput
        std::map<std::string, float> systemMetrics;
        std::map<std::string, float> applicationMetrics;
        SimulationStep totalSteps;     // total simulation steps completed
        Timestamp totalSimulationTime; // total simulation time
        float stepsPerSecond;          // simulation speed
    };

    /**
     * @brief Implementation comparison result
     */
    struct ImplementationComparison {
        std::string implementationName;
        float executionTime;      // relative to baseline
        float memoryUsage;        // relative to baseline
        float efficiency;         // relative efficiency
        float scalabilityScore;   // scalability metric
        std::map<std::string, float> metrics;
        bool isBaseline;          // whether this is the baseline implementation
    };

    /**
     * @brief Performance bottleneck
     */
    struct PerformanceBottleneck {
        std::string componentName;
        float impactScore;           // 0-1 severity score
        std::string bottleneckType; // "memory", "cpu", "io", "algorithm"
        float estimatedSavings;      // potential performance improvement
        std::vector<std::string> suggestedSolutions;
    };

    /**
     * @brief Performance metrics configuration
     */
    struct MetricsConfig {
        bool enableTimeTracking;          // track execution time
        bool enableMemoryTracking;        // track memory usage
        bool enableCpuTracking;           // track CPU usage
        bool enableIoTracking;            // track I/O operations
        bool enableNetworkTracking;       // track network usage
        bool enableDetailedMetrics;       // enable detailed metrics collection
        float timeWindow;                 // time window for rolling metrics
        size_t maxHistorySize;            // maximum size of metrics history
        std::vector<std::string> customMetrics; // custom metric names to track
        
        MetricsConfig() 
            : enableTimeTracking(true), enableMemoryTracking(true), 
              enableCpuTracking(true), enableIoTracking(false), 
              enableNetworkTracking(false), enableDetailedMetrics(false),
              timeWindow(60.0f), maxHistorySize(1000) {}
    };

    /**
     * @brief Constructor
     * 
     * @param config Performance metrics configuration
     */
    PerformanceMetrics(const MetricsConfig& config = MetricsConfig());

    /**
     * @brief Destructor
     */
    ~PerformanceMetrics();

    /**
     * @brief Copy constructor
     */
    PerformanceMetrics(const PerformanceMetrics& other);

    /**
     * @brief Copy assignment operator
     */
    PerformanceMetrics& operator=(const PerformanceMetrics& other);

    /**
     * @brief Move constructor
     */
    PerformanceMetrics(PerformanceMetrics&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    PerformanceMetrics& operator=(PerformanceMetrics&& other) noexcept;

    /**
     * @brief Get configuration
     * 
     * @return Current metrics configuration
     */
    MetricsConfig getConfig() const;

    /**
     * @brief Set configuration
     * 
     * @param config New metrics configuration
     */
    void setConfig(const MetricsConfig& config);

    /**
     * @brief Record benchmark result
     * 
     * @param benchmarkResult Benchmark result to record
     */
    void recordBenchmark(const BenchmarkResult& benchmarkResult);

    /**
     * @brief Record performance profile
     * 
     * @param profile Performance profile to record
     */
    void recordProfile(const PerformanceProfile& profile);

    /**
     * @brief Compute simulation efficiency
     * 
     * Computes simulation efficiency as a normalized score (0-1) based on
     * execution time, memory usage, and reward per step. Higher scores indicate
     * better efficiency.
     * 
     * @param benchmark Benchmark result to analyze
     * @return Efficiency score (0-1)
     */
    float computeEfficiency(const BenchmarkResult& benchmark);

    /**
     * @brief Compute scalability metrics
     * 
     * Computes scalability metrics by comparing performance across different
     * problem sizes or simulation scales. Returns speedup, efficiency,
     * and scalability scores.
     * 
     * @param benchmarks Vector of benchmark results at different scales
     * @return Scalability analysis results
     */
    std::map<std::string, float> computeScalability(
        const std::vector<BenchmarkResult>& benchmarks
    );

    /**
     * @brief Generate performance report
     * 
     * Generates a comprehensive performance report including summary statistics,
     * trends, comparisons, and recommendations. Can be generated in different
     * formats (text, JSON, CSV).
     * 
     * @param format Report format ("text", "json", "csv")
     * @return Performance report as string
     */
    std::string generateReport(const std::string& format = "text");

    /**
     * @brief Compare implementations
     * 
     * Compares multiple implementations based on performance metrics. Provides
     * detailed comparison including execution time, memory usage, efficiency,
     * and scalability scores.
     * 
     * @param implementations Vector of implementation comparison results
     * @param baselineName Name of baseline implementation
     * @return Comparison summary and rankings
     */
    std::map<std::string, float> compareImplementations(
        const std::vector<ImplementationComparison>& implementations,
        const std::string& baselineName = ""
    );

    /**
     * @brief Profile performance bottlenecks
     * 
     * Profiles the system to identify performance bottlenecks, including memory
     * usage patterns, CPU hotspots, I/O contention, and algorithmic inefficiencies.
     * Provides suggested solutions for each bottleneck.
     * 
     @return Vector of identified performance bottlenecks
     */
    std::vector<PerformanceBottleneck> profilePerformance();

    /**
     * @brief Analyze performance trends
     * 
     * Analyzes performance trends over time, including trends in execution time,
     * memory usage, and efficiency. Identifies patterns and outliers.
     * 
     * @param metricName Name of metric to analyze
     * @param timeWindow Time window for trend analysis
     * @return Trend analysis results
     */
    std::map<std::string, std::vector<float>> analyzeTrends(
        const std::string& metricName = "execution_time",
        float timeWindow = 300.0f
    );

    /**
     * @brief Generate optimization recommendations
     * 
     * Generates optimization recommendations based on performance analysis,
     * including specific suggestions for improving performance and reducing
     * resource usage.
     * 
     * @return Map of recommendations and their expected impact
     */
    std::map<std::string, std::string> generateOptimizationRecommendations();

    /**
     * @brief Export metrics
     * 
     * @param filepath Path to save metrics (JSON/CSV format)
     * @param format Output format
     * @return true if export was successful, false otherwise
     */
    bool exportMetrics(const std::string& filepath, const std::string& format = "json");

    /**
     * @brief Import metrics
     * 
     * @param filepath Path to metrics file
     * @return true if import was successful, false otherwise
     */
    bool importMetrics(const std::string& filepath);

    /**
     * @brief Get performance summary
     * 
     * @return String summary of performance metrics
     */
    std::string getPerformanceSummary() const;

    /**
     * @brief Get latest benchmark result
     * 
     * @return Latest benchmark result
     */
    BenchmarkResult getLatestBenchmark() const;

    /**
     * @brief Get latest performance profile
     * 
     * @return Latest performance profile
     */
    PerformanceProfile getLatestProfile() const;

    /**
     * @brief Reset metrics
     */
    void reset();

    /**
     * @brief Calculate overall performance score
     * 
     * @return Overall performance score (0-1, higher is better)
     */
    float calculatePerformanceScore() const;

    /**
     * @brief Generate performance heatmap
     * 
     * Generates a performance heatmap visualization showing performance
     * across different dimensions (time, metrics, implementations).
     * 
     * @param dimension Dimension for heatmap ("time", "metric", "implementation")
     * @return 2D vector of performance values for heatmap
     */
    std::vector<std::vector<float>> generatePerformanceHeatmap(
        const std::string& dimension = "time"
    );

    /**
     * @brief Compute performance variance
     * 
     * Computes statistical variance in performance metrics, including variance
     * and standard deviation across runs, time windows, or different metrics.
     * 
     * @param metricName Metric to compute variance for
     * @return Variance and standard deviation
     */
    std::map<std::string, float> computePerformanceVariance(
        const std::string& metricName = "execution_time"
    );

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
