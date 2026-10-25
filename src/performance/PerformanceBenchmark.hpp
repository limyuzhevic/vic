#pragma once

/**
 * Performance Benchmarking Utilities
 * 
 * This module provides performance benchmarking utilities for NLM,
 * including system performance monitoring and comparison tools.
 */

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cmath>
#include <cstring>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace nlm {

/**
 * System performance information
 */
struct SystemInfo {
    // Hardware information
    size_t cpuCores;
    size_t cpuThreads;
    std::string cpuArchitecture;
    std::string cpuVendor;
    std::string cpuModel;
    
    // Memory information
    size_t totalMemory;
    size_t availableMemory;
    size_t cacheLineSize;
    
    // System information
    std::string osName;
    std::string kernelVersion;
    std::string platform;
    
    // Performance characteristics
    double cpuFrequency;
    bool hasSSE;
    bool hasSSE2;
    bool hasSSE3;
    bool hasSSSE3;
    bool hasSSE4_1;
    bool hasSSE4_2;
    bool hasAVX;
    bool hasAVX2;
    bool hasAVX512;
    bool hasNEON;
    
    SystemInfo()
        : cpuCores(0)
        , cpuThreads(0)
        , totalMemory(0)
        , availableMemory(0)
        , cacheLineSize(0)
        , cpuFrequency(0.0)
        , hasSSE(false)
        , hasSSE2(false)
        , hasSSE3(false)
        , hasSSSE3(false)
        , hasSSE4_1(false)
        , hasSSE4_2(false)
        , hasAVX(false)
        , hasAVX2(false)
        , hasAVX512(false)
        , hasNEON(false) {}
};

/**
 * Performance benchmark result
 */
struct BenchmarkResult {
    std::string name;
    double durationMs;
    double cpuUsagePercent;
    size_t memoryUsage;
    double energyEstimate;
    bool success;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
    
    BenchmarkResult()
        : durationMs(0.0)
        , cpuUsagePercent(0.0)
        , memoryUsage(0)
        , energyEstimate(0.0)
        , success(false)
        , timestamp(std::chrono::system_clock::now()) {}
};

/**
 * Performance comparison result
 */
struct ComparisonResult {
    std::string baselineName;
    std::string testName;
    double speedupFactor;
    double efficiency;
    double confidence;
    std::string statisticalSignificance;
    std::string recommendation;
    
    ComparisonResult()
        : speedupFactor(1.0)
        , efficiency(1.0)
        , confidence(0.0)
        , statisticalSignificance("")
        , recommendation("") {}
};

/**
 * Performance benchmark configuration
 */
struct BenchmarkConfig {
    std::string name;
    bool enabled;
    double maxDurationSeconds;
    size_t iterations;
    bool warmupRequired;
    size_t warmupIterations;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> parameters;
    
    BenchmarkConfig()
        : enabled(true)
        , maxDurationSeconds(60.0)
        , iterations(1)
        , warmupRequired(true)
        , warmupIterations(1) {}
};

/**
 * Performance benchmark suite
 */
class PerformanceBenchmarkSuite {
public:
    PerformanceBenchmarkSuite();
    ~PerformanceBenchmarkSuite();
    
    /**
     * Add benchmark to the suite
     */
    void addBenchmark(const std::string& name,
                      std::function<BenchmarkResult(const BenchmarkConfig&)> benchmarkFunc);
    
    /**
     * Run all benchmarks in the suite
     */
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    /**
     * Run specific benchmark by name
     */
    BenchmarkResult runBenchmark(const std::string& name);
    
    /**
     * Run benchmarks with specific configuration
     */
    std::vector<BenchmarkResult> runBenchmarks(const std::vector<std::string>& benchmarkNames);
    
    /**
     * Get benchmark names in the suite
     */
    std::vector<std::string> getBenchmarkNames() const;
    
    /**
     * Remove benchmark from suite
     */
    bool removeBenchmark(const std::string& name);
    
    /**
     * Clear all benchmarks
     */
    void clearBenchmarks();
    
    /**
     * Get system information
     */
    SystemInfo getSystemInfo() const;
    
    /**
     * Get performance summary
     */
    std::string getPerformanceSummary() const;
    
    /**
     * Export benchmarks to file
     */
    bool exportBenchmarks(const std::string& filename) const;
    
    /**
     * Import benchmarks from file
     */
     bool importBenchmarks(const std::string& filename);
    
    /**
     * Get benchmark count
     */
    size_t getBenchmarkCount() const;
    
    /**
     * Check if benchmark exists
     */
    bool hasBenchmark(const std::string& name) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance comparison suite
 */
class PerformanceComparisonSuite {
public:
    PerformanceComparisonSuite();
    ~PerformanceComparisonSuite();
    
    /**
     * Add baseline result for comparison
     */
    void addBaseline(const std::string& name, const BenchmarkResult& result);
    
    /**
     * Add test result to compare against baseline
     */
    void addTestResult(const std::string& baselineName, const std::string& testName,
                      const BenchmarkResult& testResult);
    
    /**
     * Compare test results against baseline
     */
    ComparisonResult compareResults(const std::string& baselineName,
                                   const std::string& testName);
    
    /**
     * Get all baseline names
     */
    std::vector<std::string> getBaselineNames() const;
    
    /**
     * Get test names for baseline
     */
    std::vector<std::string> getTestNames(const std::string& baselineName) const;
    
    /**
     * Get comparison results
     */
    ComparisonResult getComparisonResult(const std::string& baselineName,
                                        const std::string& testName) const;
    
    /**
     * Generate comparison report
     */
    std::string generateComparisonReport(const std::string& baselineName) const;
    
    /**
     * Export comparison results to file
     */
    bool exportComparisonResults(const std::string& filename) const;
    
    /**
     * Import comparison results from file
     */
    bool importComparisonResults(const std::string& filename);
    
    /**
     * Get baseline result
     */
    BenchmarkResult getBaselineResult(const std::string& name) const;
    
    /**
     * Get test result
     */
    BenchmarkResult getTestResult(const std::string& baselineName,
                                 const std::string& testName) const;
    
    /**
     * Remove baseline
     */
    bool removeBaseline(const std::string& name);
    
    /**
     * Remove test result
     */
    bool removeTestResult(const std::string& baselineName, const std::string& testName);
    
    /**
     * Clear all comparisons
     */
    void clearComparisons();
    
    /**
     * Get comparison count
     */
    size_t getComparisonCount() const;
    
    /**
     * Check if baseline exists
     */
    bool hasBaseline(const std::string& name) const;
    
    /**
     * Check if test result exists
     */
    bool hasTestResult(const std::string& baselineName, const std::string& testName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance profiler
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Start performance monitoring
     */
    void startMonitoring(const std::string& sessionName = "default");
    
    /**
     * Stop performance monitoring and collect results
     */
    BenchmarkResult stopMonitoring();
    
    /**
     * Get current performance metrics
     */
    BenchmarkResult getCurrentMetrics() const;
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const;
    
    /**
     * Get monitoring session name
     */
    std::string getSessionName() const;
    
    /**
     * Reset profiler state
     */
    void reset();
    
    /**
     * Get profiler version
     */
    static std::string getVersion();
    
    /**
     * Get system capabilities
     */
    static SystemInfo getSystemCapabilities();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance report generator
 */
class PerformanceReportGenerator {
public:
    PerformanceReportGenerator();
    ~PerformanceReportGenerator();
    
    /**
     * Generate report from benchmark suite
     */
    std::string generateReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate report from comparison suite
     */
    std::string generateReport(const PerformanceComparisonSuite& suite) const;
    
    /**
     * Generate report from profiler results
     */
    std::string generateReport(const BenchmarkResult& result) const;
    
    /**
     * Generate detailed benchmark report
     */
    std::string generateDetailedBenchmarkReport(const BenchmarkResult& result) const;
    
    /**
     * Generate statistical analysis report
     */
    std::string generateStatisticalReport(const std::vector<BenchmarkResult>& results) const;
    
    /**
     * Generate performance trend report
     */
    std::string generateTrendReport(const std::vector<BenchmarkResult>& results) const;
    
    /**
     * Generate system information report
     */
    std::string generateSystemReport(const SystemInfo& systemInfo) const;
    
    /**
     * Generate benchmark comparison report
     */
    std::string generateBenchmarkComparisonReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate performance optimization report
     */
    std::string generateOptimizationReport(const PerformanceBenchmarkSuite& suite,
                                           const PerformanceComparisonSuite& comparisons) const;
    
    /**
     * Export report to file
     */
    bool exportReport(const std::string& content, const std::string& filename) const;
    
    /**
     * Get available report formats
     */
    std::vector<std::string> getAvailableFormats() const;
    
    /**
     * Validate report content
     */
    bool validateReport(const std::string& content, const std::string& format) const;
    
    /**
     * Generate markdown report
     */
    std::string generateMarkdownReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate HTML report
     */
    std::string generateHTMLReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate PDF report
     */
    std::string generatePDFReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate LaTeX report
     */
    std::string generateLaTeXReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate JSON report
     */
    std::string generateJSONReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate CSV report
     */
    std::string generateCSVReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate XML report
     */
    std::string generateXMLReport(const PerformanceBenchmarkSuite& suite) const;
    
    /**
     * Generate comparison report
     */
    std::string generateComparisonReport(const PerformanceComparisonSuite& suite) const;
    
    /**
     * Generate optimization recommendation report
     */
    std::string generateOptimizationRecommendationReport(const PerformanceBenchmarkSuite& suite,
                                                         const PerformanceComparisonSuite& comparisons) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatTime(double seconds) const;
    std::string formatMemory(size_t bytes) const;
    std::string formatPercentage(double value) const;
    std::string formatSpeedup(double factor) const;
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSV(const std::string& text) const;
    std::string escapeXML(const std::string& text) const;
    std::string escapeLaTeX(const std::string& text) const;
    
    std::string generateMarkdownHeader(const std::string& title) const;
    std::string generateMarkdownTable(const std::vector<std::vector<std::string>>& data) const;
    std::string generateMarkdownCodeBlock(const std::string& code, const std::string& language) const;
    
    std::string generateHTMLHeader(const std::string& title) const;
    std::string generateHTMLTable(const std::vector<std::vector<std::string>>& data) const;
    std::string generateHTMLChart(const std::vector<double>& data, const std::string& title) const;
    
    std::string generateJSONObject(const std::unordered_map<std::string, std::string>& data) const;
    std::string generateCSVRow(const std::vector<std::string>& columns) const;
    
    std::string generateLaTeXDocument(const std::string& title, const std::string& content) const;
    
    std::string generateOptimizationSummary(const PerformanceBenchmarkSuite& suite,
                                            const PerformanceComparisonSuite& comparisons) const;
};

/**
 * Performance monitoring utilities
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    /**
     * Start monitoring specific process
     */
    bool startMonitoring(pid_t pid = getpid());
    
    /**
     * Stop monitoring
     */
    void stopMonitoring();
    
    /**
     * Get current system metrics
     */
    struct SystemMetrics {
        double cpuUsagePercent;
        size_t memoryUsage;
        size_t memoryPeak;
        double diskIORead;
        double diskIOWrite;
        size_t networkBytesSent;
        size_t networkBytesReceived;
        double temperature;
        std::chrono::system_clock::time_point timestamp;
        
        SystemMetrics()
            : cpuUsagePercent(0.0)
            , memoryUsage(0)
            , memoryPeak(0)
            , diskIORead(0.0)
            , diskIOWrite(0.0)
            , networkBytesSent(0)
            , networkBytesReceived(0)
            , temperature(0.0)
            , timestamp(std::chrono::system_clock::now()) {}
    };
    
    SystemMetrics getCurrentMetrics() const;
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const;
    
    /**
     * Get monitoring PID
     */
    pid_t getPID() const;
    
    /**
     * Get monitoring start time
     */
    std::chrono::system_clock::time_point getStartTime() const;
    
    /**
     * Get monitoring duration
     */
    double getDurationSeconds() const;
    
    /**
     * Reset monitoring
     */
    void reset();
    
    /**
     * Get monitoring statistics
     */
    std::string getStatistics() const;
    
    /**
     * Export metrics to file
     */
    bool exportMetrics(const std::string& filename) const;
    
    /**
     * Import metrics from file
     */
    bool importMetrics(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance validation utilities
 */
class PerformanceValidator {
public:
    PerformanceValidator();
    ~PerformanceValidator();
    
    /**
     * Validate benchmark configuration
     */
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double confidence;
        
        ValidationResult()
            : valid(true)
            , confidence(1.0) {}
    };
    
    ValidationResult validateBenchmarkConfig(const BenchmarkConfig& config) const;
    
    /**
     * Validate benchmark result
     */
    ValidationResult validateBenchmarkResult(const BenchmarkResult& result) const;
    
    /**
     * Validate comparison result
     */
    ValidationResult validateComparisonResult(const ComparisonResult& result) const;
    
    /**
     * Validate system requirements
     */
    ValidationResult validateSystemRequirements() const;
    
    /**
     * Check if system meets performance requirements
     */
    ValidationResult checkPerformanceRequirements(const std::string& requirementsFile) const;
    
    /**
     * Get performance benchmarks
     */
    std::vector<std::string> getPerformanceBenchmarks() const;
    
    /**
     * Set performance benchmarks
     */
    void setPerformanceBenchmarks(const std::vector<std::string>& benchmarks);
    
    /**
     * Get performance thresholds
     */
    std::unordered_map<std::string, double> getPerformanceThresholds() const;
    
    /**
     * Set performance thresholds
     */
    void setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds);
    
    /**
     * Validate performance against thresholds
     */
    ValidationResult validatePerformance(const std::string& metric, double value) const;
    
    /**
     * Generate performance report
     */
    std::string generateValidationReport(const ValidationResult& result) const;
    
    /**
     * Export validation results
     */
    bool exportValidationResults(const ValidationResult& result, const std::string& filename) const;
    
    /**
     * Import validation results
     */: 
    bool importValidationResults(const std::string& filename, ValidationResult& result);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Main performance testing interface
 */
class PerformanceTestFramework {
public:
    PerformanceTestFramework();
    ~PerformanceTestFramework();
    
    /**
     * Initialize framework
     */
    bool initialize();
    
    /**
     * Cleanup framework
     */
    void cleanup();
    
    /**
     * Get benchmark suite
     */
    PerformanceBenchmarkSuite& getBenchmarkSuite();
    
    /**
     * Get comparison suite
     */
    PerformanceComparisonSuite& getComparisonSuite();
    
    /**
     * Get profiler
     */
    PerformanceProfiler& getProfiler();
    
    /**
     * Get report generator
     */
    PerformanceReportGenerator& getReportGenerator();
    
    /**
     * Get performance monitor
     */
    PerformanceMonitor& getPerformanceMonitor();
    
    /**
     * Get performance validator
     */
    PerformanceValidator& getPerformanceValidator();
    
    /**
     * Run complete performance test suite
     */
    std::vector<BenchmarkResult> runCompleteTestSuite();
    
    /**
     * Generate complete performance report
     */
    std::string generateCompleteReport();
    
    /**
     * Export performance data
     */
    bool exportPerformanceData(const std::string& outputDir);
    
    /**
     * Import performance data
     */
    bool importPerformanceData(const std::string& inputDir);
    
    /**
     * Get framework status
     */
    std::string getFrameworkStatus() const;
    
    /**
     * Check if framework is initialized
     */
    bool isInitialized() const;
    
    /**
     * Get framework version
     */
    static std::string getVersion();
    
    /**
     * Get framework build info
     */
    static std::string getBuildInfo();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm