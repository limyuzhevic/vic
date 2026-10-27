// Performance monitoring and profiling tools for NLM
// Comprehensive performance measurement and analysis capabilities

#pragma once

#include "core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <memory>

using namespace nlm;

// Performance metrics collector
struct PerformanceMetrics {
    double wallClockTime;
    double cpuTime;
    size_t totalSimulations;
    float totalReward;
    float avgFiringRate;
    float memoryUsageMB;
    size_t neuronsCount;
    size_t synapsesCount;
    size_t totalSpikes;
    std::vector<double> timeSteps;
    std::vector<float> firingRates;
    std::vector<float> rewards;
    std::vector<float> neuromodLevels;
    
    PerformanceMetrics() : wallClockTime(0), cpuTime(0), totalSimulations(0),
                          totalReward(0), avgFiringRate(0), memoryUsageMB(0),
                          neuronsCount(0), synapsesCount(0), totalSpikes(0) {}
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();

    // Start profiling
    void startProfile(const std::string& experimentName);
    void stopProfile();
    void pauseProfile();
    void resumeProfile();

    // Record measurements
    void recordTimePoint(const std::string& event, double time);
    void recordMetric(const std::string& metric, double value);
    void recordMemoryUsage(size_t memoryBytes);
    void recordSimulationStep(float reward, float firingRate, float neuromodLevel);

    // Get results
    PerformanceMetrics getResults() const;
    std::string getProfileSummary() const;
    std::string getResultsJSON() const;

    // Export results
    bool exportToCSV(const std::string& filename) const;
    bool exportToJSON(const std::string& filename) const;
    bool exportToHTML(const std::string& filename) const;

    // Analysis methods
    double getAverageFiringRate() const;
    double getStandardDeviationFiringRate() const;
    double getAverageReward() const;
    double getRewardImprovement() const;
    double getPerformanceScaling() const;
    std::vector<std::pair<double, double>> getTimeSeriesData() const;
    std::map<std::string, double> getStatisticalAnalysis() const;

    // Benchmark comparisons
    void addBenchmark(const std::string& benchmarkName, const PerformanceMetrics& metrics);
    std::string getBenchmarkReport() const;
    double getBenchmarkScore(const std::string& benchmarkName) const;

    // Profile analysis
    struct TimeProfile {
        std::string eventName;
        double duration;
        double percentage;
    };

    std::vector<TimeProfile> analyzeTimeDistribution() const;
    std::string getOptimizationSuggestions() const;

    // Configuration
    void setSamplingInterval(size_t interval);
    void enableRealTimeReporting(bool enable);
    void setMaxTimePoints(size_t maxPoints);

    // Reset
    void reset();
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    double calculateThroughput(double time, size_t steps) const;
    double calculateEfficiency(double time, double theoreticalMax) const;
    std::string generateHTMLReport() const;
    std::string generateCSVReport() const;
    std::string generateJSONReport() const;
};

// Real-time performance monitor
class RealTimePerformanceMonitor {
public:
    RealTimePerformanceMonitor();
    ~RealTimePerformanceMonitor();

    // Start monitoring
    void startMonitoring(const std::string& identifier);
    void stopMonitoring();
    bool isMonitoring() const;

    // Get current metrics
    double getCurrentFPS() const;
    double getCurrentMemoryUsageMB() const;
    float getCurrentFiringRate() const;
    size_t getCurrentStep() const;
    double getElapsedTime() const;

    // Monitoring setup
    void setUpdateInterval(double interval);
    void setMaxHistorySize(size_t size);
    void enableMetricsCollection(bool enable);

    // Data access
    std::vector<double> getFiringRateHistory() const;
    std::vector<double> getMemoryUsageHistory() const;
    std::vector<double> getTimeHistory() const;
    std::vector<std::string> getEventLog() const;

    // Callbacks
    using PerformanceCallback = std::function<void(const PerformanceMetrics& metrics)>;
    void setPerformanceCallback(PerformanceCallback callback);

    // Configuration
    void configureForExperiment(const std::string& experimentName, size_t neuronCount, size_t maxSteps);

    // Control
    void pauseMonitoring();
    void resumeMonitoring();
    void resetMonitoring();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void updateMetrics(double time);
    void logEvent(const std::string& event);
};

// Performance benchmark suite
class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark();

    // Run standard benchmarks
    PerformanceMetrics runPerformanceBenchmark(const std::string& benchmarkName,
                                              const std::string& description);

    // Run comparison benchmarks
    struct BenchmarkResult {
        std::string name;
        std::string description;
        PerformanceMetrics metrics;
        double relativeScore;
    };

    std::vector<BenchmarkResult> runComparisonBenchmark(const std::string& baselineBenchmark,
                                                        const std::vector<std::string>& testBenchmarks);

    // Run scaling benchmarks
    struct ScalingResult {
        size_t neuronCount;
        double timePerStep;
        double throughput;
        double efficiency;
    };

    std::vector<ScalingResult> runScalingBenchmark(size_t startNeurons, size_t maxNeurons,
                                                  size_t stepSize);

    // Run memory benchmarks
    struct MemoryResult {
        size_t neuronCount;
        size_t memoryUsageBytes;
        double memoryPerNeuron;
    };

    std::vector<MemoryResult> runMemoryBenchmark(size_t startNeurons, size_t maxNeurons,
                                                size_t stepSize);

    // Benchmark configuration
    void setBenchmarkThreadCount(size_t threads);
    void enableBenchmarkValidation(bool enable);
    void setBenchmarkTimeout(double timeout);

    // Results management
    void saveBenchmarkResults(const std::string& filename) const;
    void loadBenchmarkResults(const std::string& filename);
    std::string getBenchmarkReport() const;

    // Statistical analysis
    double calculateBenchmarkScore(const PerformanceMetrics& metrics) const;
    std::string getPerformanceRating(double score) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    PerformanceMetrics runSingleBenchmark(const std::string& benchmarkName);
    double calculateEfficiency(const PerformanceMetrics& metrics) const;
    double calculateScalability(const std::vector<ScalingResult>& results) const;
};

// Performance analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();

    // Load and analyze performance data
    bool loadPerformanceData(const std::string& filename);
    bool loadMultipleDataFiles(const std::vector<std::string>& filenames);

    // Analysis methods
    std::string analyzePerformanceTrends() const;
    std::string analyzeBottlenecks() const;
    std::string analyzeMemoryUsage() const;
    std::string analyzeTimeComplexity() const;
    std::string analyzeNeuralDynamics() const;

    // Statistical analysis
    struct StatisticalSummary {
        double mean;
        double median;
        double standardDeviation;
        double variance;
        double min;
        double max;
        double range;
        size_t sampleSize;
    };

    StatisticalSummary calculateStatistics(const std::vector<double>& data) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    std::vector<double> calculateMovingAverage(const std::vector<double>& data, size_t windowSize) const;

    // Performance optimization recommendations
    std::string generateOptimizationRecommendations() const;
    std::string getPerformanceInsights() const;

    // Comparison analysis
    std::string compareWithBenchmark(const std::string& benchmarkName) const;
    std::string compareWithPreviousRuns() const;

    // Report generation
    std::string generateDetailedReport() const;
    std::string generateExecutiveSummary() const;
    std::string generateTechnicalReport() const;

    // Export
    bool exportAnalysis(const std::string& filename, const std::string& format = "text");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void validateDataConsistency() const;
    double calculatePerformanceScore(const PerformanceMetrics& metrics) const;
    std::string formatNumber(double value) const;
    std::string formatTime(double seconds) const;
};

// Convenience functions
namespace Performance {
    // Create performance profiler
    PerformanceProfiler createProfiler(const std::string& experimentName = "default");

    // Create real-time monitor
    RealTimePerformanceMonitor createMonitor(const std::string& identifier = "default");

    // Create benchmark suite
    PerformanceBenchmark createBenchmarkSuite(const std::string& name = "suite");

    // Create performance analyzer
    PerformanceAnalyzer createAnalyzer(const std::string& name = "analyzer");

    // Utility functions
    std::string getSystemInfo();
    double getCurrentMemoryUsageMB();
    size_t getCPUCoreCount();
    std::string getCurrentTimestamp();
}
