#pragma once

/**
 * Performance Testing Framework for NLM
 * 
 * Comprehensive performance testing framework providing regression testing,
 * profiling integration, and scalability analysis for NLM systems.
 * 
 * Key Features:
 * - Performance regression testing with baselines
 * - Performance profiling integration
 * - Performance regression detection
 * - Load testing and stress testing
 * - Scalability testing
 * - Bottleneck analysis
 * - Resource utilization tracking
 * 
 * Design Principles:
 * - Baseline-driven testing approach
 * - Statistical analysis of performance data
 * - Automated regression detection
 * - Multi-dimensional performance profiling
 * - Resource-aware testing
 * 
 * Biological Motivation:
 * Neural systems maintain performance homeostasis
 * This framework provides similar monitoring for neural computation performance
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <filesystem>
#include "TestFramework.hpp"
#include "PerformanceMonitor.hpp"

namespace nlm {
namespace testing {

/**
 * Performance metric categories
 */
enum class PerformanceMetric {
    CPU_USAGE,
    MEMORY_USAGE,
    MEMORY_ALLOCATION,
    STEP_DURATION,
    THROUGHPUT,
    SPIKE_RATE,
    NETWORK_ACTIVITY,
    DISK_IO,
    NETWORK_BANDWIDTH,
    LATENCY,
    REALTIME_FACTOR,
    CUSTOM
};

/**
 * Performance baseline configuration
 */
struct PerformanceBaseline {
    std::string name;
    std::string description;
    std::unordered_map<PerformanceMetric, double> metrics;
    std::unordered_map<PerformanceMetric, double> thresholds; // Allowable deviation
    std::unordered_map<PerformanceMetric, double> weights; // Relative importance
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string environment; // Hardware/software environment
    
    PerformanceBaseline(const std::string& name, const std::string& description)
        : name(name), description(description), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Performance regression result
 */
struct PerformanceRegression {
    std::string benchmarkName;
    PerformanceMetric metric;
    double baselineValue;
    double currentValue;
    double regressionPercent;
    double threshold;
    bool isRegressed;
    std::string regressionType; // "degradation", "improvement", "stable"
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string details;
    
    PerformanceRegression(const std::string& name, PerformanceMetric metric,
                         double baseline, double current, double threshold)
        : benchmarkName(name), metric(metric), baselineValue(baseline),
          currentValue(current), regressionPercent(0.0), threshold(threshold),
          isRegressed(false), timestamp(std::chrono::high_resolution_clock::now()) {
        calculateRegression();
    }
    
private:
    void calculateRegression() {
        if (baselineValue > 0) {
            regressionPercent = ((currentValue - baselineValue) / baselineValue) * 100.0;
        } else {
            regressionPercent = (currentValue - baselineValue) * 100.0;
        }
        
        isRegressed = (regressionPercent > threshold);
        
        if (regressionPercent > threshold) {
            regressionType = "degradation";
        } else if (regressionPercent < -threshold) {
            regressionType = "improvement";
        } else {
            regressionType = "stable";
        }
    }
};

/**
 * Load test configuration
 */
struct LoadTestConfig {
    std::string name;
    size_t concurrentUsers;
    double duration; // seconds
    std::vector<std::string> operations;
    std::unordered_map<std::string, double> weights;
    double rampUpTime; // seconds to reach full load
    double rampDownTime; // seconds to reduce to zero load
    bool isRealTime; // Test real-time performance under load
    
    LoadTestConfig(const std::string& name, size_t users, double duration)
        : name(name), concurrentUsers(users), duration(duration),
          rampUpTime(30.0), rampDownTime(30.0), isRealTime(false) {}
};

/**
 * Load test result
 */
struct LoadTestResult {
    std::string testName;
    size_t concurrentUsers;
    double duration;
    double totalOperations;
    double operationsPerSecond;
    double averageLatency;
    double p95Latency;
    double p99Latency;
    double errorRate;
    double throughput;
    double cpuUtilization;
    double memoryUsage;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::vector<double> latencyHistogram;
    
    LoadTestResult(const std::string& name, size_t users, double duration)
        : testName(name), concurrentUsers(users), duration(duration),
          totalOperations(0), operationsPerSecond(0), averageLatency(0),
          p95Latency(0), p99Latency(0), errorRate(0), throughput(0),
          cpuUtilization(0), memoryUsage(0), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Performance testing framework
 */
class PerformanceTestSuite {
public:
    PerformanceTestSuite();
    ~PerformanceTestSuite();
    
    // Baseline management
    void createBaseline(const std::string& name, const std::string& description);
    void addBaselineMetric(PerformanceMetric metric, double value);
    void setBaselineThreshold(PerformanceMetric metric, double threshold);
    void setBaselineWeight(PerformanceMetric metric, double weight);
    void saveBaseline(const std::string& filename) const;
    bool loadBaseline(const std::string& filename);
    
    // Performance benchmarking
    BenchmarkResult benchmark(const std::string& name,
                             std::function<void()> func,
                             size_t iterations = 100,
                             const std::string& unit = "ms");
    
    void profileOperation(const std::string& operationName,
                         std::function<void()> operation,
                         size_t iterations = 1000);
    
    void recordPerformanceMetric(PerformanceMetric metric, double value,
                                const std::string& context = "");
    
    // Regression testing
    void runRegressionTests();
    void compareWithBaseline();
    std::vector<PerformanceRegression> getRegressionResults() const;
    
    // Load testing
    void runLoadTest(const LoadTestConfig& config);
    LoadTestResult runSimulatedLoadTest(const LoadTestConfig& config);
    
    // Scalability testing
    void runScalabilityTests();
    void measureScalability(const std::vector<size_t>& userCounts, double duration);
    
    // Stress testing
    void runStressTests();
    void measureStressLimits(size_t targetLoad, double duration);
    
    // Performance reporting
    void generatePerformanceReport() const;
    void exportPerformanceData(const std::string& filename) const;
    
    // Configuration validation
    bool validatePerformanceConfiguration() const;
    
    // Resource monitoring
    ResourceAllocation getCurrentResourceAllocation() const;
    PerformanceStats getPerformanceStats(PerformanceMetric metric) const;
    
    // Integration with PerformanceMonitor
    void setupPerformanceMonitoring(PerformanceMonitor& monitor);
    
private:
    // Performance data management
    void updatePerformanceData(PerformanceMetric metric, double value,
                              const std::string& context = "");
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values, double mean) const;
    double calculatePercentile(const std::vector<double>& values, double percentile) const;
    
    // Bottleneck detection
    std::vector<std::string> detectBottlenecks() const;
    
    // Data file operations
    void writePerformanceData(std::ofstream& file) const;
    void readPerformanceData(std::ifstream& file);
    
    // Test execution utilities
    LoadTestResult executeLoadTest(const LoadTestConfig& config);
    void simulateLoad(std::function<void()> operation, size_t duration, size_t concurrentUsers);
    
    // Configuration validation
    bool validateBaselineConfiguration() const;
    bool validateTestConfiguration() const;
    
    // Performance monitoring integration
    void startPerformanceMonitoring(PerformanceMonitor& monitor);
    void stopPerformanceMonitoring();
    
    // Internal state
    PerformanceBaseline currentBaseline_;
    std::unordered_map<std::string, std::vector<double>> performanceData_;
    mutable std::shared_mutex dataMutex_;
    
    std::vector<PerformanceRegression> regressionResults_;
    std::vector<LoadTestResult> loadTestResults_;
    
    mutable PerformanceMonitor* activeMonitor_{nullptr};
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_{false};
    
    std::chrono::high_resolution_clock::time_point sessionStartTime_;
    std::unordered_map<PerformanceMetric, double> currentMetrics_;
};

/**
 * Benchmark suite for comprehensive performance measurements
 */
class BenchmarkSuite {
public:
    BenchmarkSuite() = default;
    ~BenchmarkSuite() = default;
    
    // Benchmark registration
    void registerBenchmark(const std::string& name,
                           const std::string& description,
                           std::function<void()> benchmarkFunction,
                           size_t iterations = 100,
                           const std::string& unit = "ms",
                           const std::string& category = "general");
    
    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& name) const;
    void runAllBenchmarks();
    
    // Benchmark categorization
    std::vector<std::string> getBenchmarksByCategory(const std::string& category) const;
    std::vector<std::string> getAllBenchmarks() const;
    
    // Benchmark reporting
    void generateBenchmarkReport() const;
    void exportBenchmarkResults(const std::string& filename) const;
    
    // Benchmark comparison
    void compareBenchmarks(const std::string& baselineFile) const;
    
private:
    // Benchmark structure
    struct Benchmark {
        std::string name;
        std::string description;
        std::function<void()> function;
        size_t iterations;
        std::string unit;
        std::string category;
        BenchmarkResult lastResult;
        std::chrono::high_resolution_clock::time_point registrationTime;
        
        Benchmark(const std::string& name, const std::string& description,
                 std::function<void()> func, size_t iterations = 100,
                 const std::string& unit = "ms", const std::string& category = "general")
            : name(name), description(description), function(func),
              iterations(iterations), unit(unit), category(category),
              registrationTime(std::chrono::high_resolution_clock::now()) {}
    };
    
    std::vector<Benchmark> benchmarks_;
    mutable std::mutex benchmarksMutex_;
};

/**
 * Performance profiling utility
 */
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    // Profiling operations
    void profileFunction(const std::string& name,
                        std::function<void()> func,
                        size_t iterations = 1000);
    
    void profileMemoryUsage(const std::string& name,
                           std::function<void()> func);
    
    void profileCPUUsage(const std::string& name,
                        std::function<void()> func);
    
    // Profiling report generation
    void generateProfileReport() const;
    void exportProfileData(const std::string& filename) const;
    
    // Performance bottleneck detection
    std::vector<std::string> detectPerformanceBottlenecks() const;
    
private:
    // Profiling data structure
    struct ProfileData {
        std::string operationName;
        std::string operationType; // "function", "memory", "cpu"
        double duration;
        double memoryUsed;
        double cpuUsage;
        size_t iterations;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string details;
    };
    
    std::vector<ProfileData> profileData_;
    mutable std::mutex profileMutex_;
};

} // namespace testing
} // namespace nlm