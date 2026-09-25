// Performance Monitoring and Benchmarking Module
// 
// This module provides comprehensive performance monitoring, benchmarking, and analysis
// capabilities for the NLM neural simulation system.
// 
// Features:
// 1. Real-time performance metrics collection
// 2. Benchmarking utilities for different experiment types
// 3. Resource usage monitoring (CPU, memory, I/O)
// 4. Timeline and checkpoint performance analysis
// 5. Comparative analysis tools
// 6. Performance profiling and bottleneck identification
// 
// Usage:
// - Integrated with CLI for command-line benchmarking
// - Provides detailed performance reports
// - Supports multiple benchmark scenarios
// - Includes performance optimization recommendations
// 
// Author: NLM Research Team
// Version: 0.1.0 (Phase 6)
// Build Date: __DATE__ at __TIME__

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>
#include <fstream>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <memory>

#include "performance/Performance.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Timeline metrics
    double simulationTime;           // Total simulation time (seconds)
    double wallClockTime;            // Wall clock time (seconds)
    double cpuTime;                  // CPU time (seconds)
    double memoryTime;               // Memory access time (seconds)
    
    // Neuron processing metrics
    size_t neuronsProcessed;          // Total neurons processed
    size_t spikesGenerated;          // Total spikes generated
    size_t spikesDelivered;          // Total spike events delivered
    size_t synapticConnections;      // Total synaptic connections processed
    
    // Memory management metrics
    size_t memoryAllocations;         // Memory allocations
    size_t memoryDeallocations;       // Memory deallocations
    size_t memoryPoolHits;            // Memory pool hits
    size_t memoryPoolMisses;          // Memory pool misses
    size_t activeMemoryUsage;        // Active memory usage (bytes)
    size_t peakMemoryUsage;           // Peak memory usage (bytes)
    
    // Performance efficiency metrics
    double spikeProcessingRate;      // Spikes per second
    double synapticActivityRate;     // Synapses processed per second
    double memoryBandwidth;          // Memory bandwidth (GB/s)
    double cpuUtilization;           // CPU utilization (%)
    double cacheHitRate;             // Cache hit rate (%)
    
    // System resource metrics
    double systemCPULoad;            // System CPU load (%)
    double systemMemoryUsage;        // System memory usage (%)
    double networkIO;                // Network I/O (bytes/second)
    
    // Temporal metrics
    size_t simulationSteps;          // Number of simulation steps
    double stepsPerSecond;           // Steps processed per second
    size_t eventsProcessed;           // Total events processed
    size_t eventsDelayed;            // Delayed events
    
    // Benchmark-specific metrics
    double benchmarkScore;           // Overall benchmark score
    double efficiencyScore;          // Performance efficiency score
    double scalabilityScore;         // Scalability assessment score
    
    // Phase-specific metrics
    double neuronDynamicsTime;       // Time spent on neuron dynamics
    double synapseDynamicsTime;       // Time spent on synapse dynamics
    double memorySystemTime;          // Time spent on memory systems
    double neuromodulationTime;       // Time spent on neuromodulation
    double predictionTime;           // Time spent on prediction
    double cognitionTime;             // Time spent on cognition
    double developmentTime;           // Time spent on development
    
    PerformanceMetrics()
        : simulationTime(0.0)
        , wallClockTime(0.0)
        , cpuTime(0.0)
        , memoryTime(0.0)
        , neuronsProcessed(0)
        , spikesGenerated(0)
        , spikesDelivered(0)
        , synapticConnections(0)
        , memoryAllocations(0)
        , memoryDeallocations(0)
        , memoryPoolHits(0)
        , memoryPoolMisses(0)
        , activeMemoryUsage(0)
        , peakMemoryUsage(0)
        , spikeProcessingRate(0.0)
        , synapticActivityRate(0.0)
        , memoryBandwidth(0.0)
        , cpuUtilization(0.0)
        , cacheHitRate(0.0)
        , systemCPULoad(0.0)
        , systemMemoryUsage(0.0)
        , networkIO(0.0)
        , simulationSteps(0)
        , stepsPerSecond(0.0)
        , eventsProcessed(0)
        , eventsDelayed(0)
        , benchmarkScore(0.0)
        , efficiencyScore(0.0)
        , scalabilityScore(0.0)
        , neuronDynamicsTime(0.0)
        , synapseDynamicsTime(0.0)
        , memorySystemTime(0.0)
        , neuromodulationTime(0.0)
        , predictionTime(0.0)
        , cognitionTime(0.0)
        , developmentTime(0.0) {}
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;                    // Benchmark name
    size_t neuronCount;                  // Number of neurons to simulate
    size_t regionCount;                  // Number of brain regions
    float connectionProbability;         // Connectivity probability
    std::string plasticityMode;          // Plasticity mode: "stdp", "hebbian", "both", "reward"
    std::string experimentType;          // Experiment type: "basic", "learning", "stdp", "development", "memory", "integrated", "curiosity", "social"
    std::vector<std::string> customOptions; // Additional configuration options
    size_t simulationSteps;              // Number of simulation steps
    bool enablePerformanceMonitoring;    // Enable detailed performance monitoring
    bool enableProfiling;                // Enable detailed profiling
    bool saveIntermediateResults;        // Save intermediate results
    std::string outputFormat;            // Output format: "text", "json", "csv", "xml"
    std::string outputFile;              // Output file path
    
    BenchmarkConfig()
        : neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , plasticityMode("both")
        , experimentType("basic")
        , simulationSteps(1000)
        , enablePerformanceMonitoring(true)
        , enableProfiling(false)
        , saveIntermediateResults(false)
        , outputFormat("text")
        , outputFile("") {}
};

// Performance benchmark results
struct BenchmarkResult {
    BenchmarkConfig config;              // Benchmark configuration
    PerformanceMetrics metrics;          // Performance metrics collected
    double totalBenchmarkScore;          // Overall benchmark score
    double efficiencyScore;              // Performance efficiency score
    double scalabilityScore;             // Scalability assessment score
    std::vector<double> phaseScores;     // Scores for each phase
    std::vector<std::string> bottlenecks; // Identified performance bottlenecks
    std::vector<std::string> recommendations; // Performance optimization recommendations
    std::map<std::string, double> componentTimes; // Time spent in each component
    std::map<std::string, double> resourceUsage;    // Resource usage statistics
    std::chrono::system_clock::time_point startTime; // Benchmark start time
    std::chrono::system_clock::time_point endTime;   // Benchmark end time
    
    BenchmarkResult() : totalBenchmarkScore(0.0), efficiencyScore(0.0), scalabilityScore(0.0) {
        startTime = std::chrono::system_clock::now();
        endTime = startTime;
    }
    
    double getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        return duration.count();
    }
    
    std::string getDurationString() const {
        double duration = getDuration();
        if (duration < 60.0) {
            return std::to_string(duration) + " seconds";
        } else if (duration < 3600.0) {
            return std::to_string(duration / 60.0) + " minutes";
        } else {
            return std::to_string(duration / 3600.0) + " hours";
        }
    }
};

// Performance monitoring interface
class PerformanceMonitor {
public:
    PerformanceMonitor();
    virtual ~PerformanceMonitor();
    
    // Start performance monitoring
    void startMonitoring();
    
    // Stop performance monitoring
    void stopMonitoring();
    
    // Record performance metrics
    void recordMetrics(const PerformanceMetrics& metrics);
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get performance history
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    
    // Configure monitoring settings
    void setMonitoringEnabled(bool enabled);
    bool isMonitoringEnabled() const;
    
    // Set sampling rate
    void setSamplingInterval(double interval);
    double getSamplingInterval() const;
    
    // Set metrics filters
    void setMetricFilter(const std::function<bool(const PerformanceMetrics&)>& filter);
    
    // Start performance benchmark
    void startBenchmark(const BenchmarkConfig& config);
    
    // Record benchmark progress
    void recordBenchmarkProgress(const PerformanceMetrics& metrics);
    
    // Complete performance benchmark
    BenchmarkResult completeBenchmark();
    
    // Get current benchmark result
    BenchmarkResult getCurrentBenchmark() const;
    
    // Generate performance report
    std::string generateReport(const BenchmarkResult& result, const std::string& format = "text");
    
    // Save performance report to file
    bool saveReport(const std::string& filepath, const BenchmarkResult& result, const std::string& format = "text");
    
    // Load performance report from file
    bool loadReport(const std::string& filepath, BenchmarkResult& result);
    
    // Clear performance data
    void clear();
    
    // Check if monitoring is active
    bool isActive() const;
    
    // Get performance statistics
    std::string getStatistics() const;
    
    // Get performance summary
    std::string getSummary() const;
    
    // Export performance data
    bool exportData(const std::string& filepath);
    
    // Import performance data
    bool importData(const std::string& filepath);
    
protected:
    // Internal monitoring methods
    void updateMetrics();
    void calculateDerivedMetrics();
    void applyFilters();
    void cleanupOldData();
    
    // Performance calculation methods
    double calculateSpikeProcessingRate() const;
    double calculateSynapticActivityRate() const;
    double calculateMemoryBandwidth() const;
    double calculateCPUUtilization() const;
    double calculateCacheHitRate() const;
    double calculateEfficiencyScore() const;
    double calculateScalabilityScore() const;
    
    // Performance bottleneck detection
    std::vector<std::string> detectBottlenecks() const;
    std::vector<std::string> detectResourceIssues() const;
    std::vector<std::string> generateRecommendations() const;
    
    // Data persistence methods
    void saveToFile(const std::string& filepath) const;
    void loadFromFile(const std::string& filepath);
    
    // Report generation methods
    std::string generateTextReport(const BenchmarkResult& result) const;
    std::string generateJSONReport(const BenchmarkResult& result) const;
    std::string generateCSVReport(const BenchmarkResult& result) const;
    std::string generateXMLReport(const BenchmarkResult& result) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance benchmark utilities
class BenchmarkUtils {
public:
    BenchmarkUtils();
    virtual ~BenchmarkUtils();
    
    // Generate standard benchmark configurations
    static std::vector<BenchmarkConfig> getStandardBenchmarks();
    
    // Generate scalability benchmark configurations
    static std::vector<BenchmarkConfig> getScalabilityBenchmarks();
    
    // Generate performance benchmark configurations
    static std::vector<BenchmarkConfig> getPerformanceBenchmarks();
    
    // Generate experiment-specific benchmark configurations
    static std::vector<BenchmarkConfig> getExperimentBenchmarks(const std::string& experimentType);
    
    // Calculate benchmark score
    static double calculateBenchmarkScore(const PerformanceMetrics& metrics);
    
    // Calculate efficiency score
    static double calculateEfficiencyScore(const PerformanceMetrics& metrics, double baselineTime);
    
    // Calculate scalability score
    static double calculateScalabilityScore(const std::vector<BenchmarkResult>& results);
    
    // Normalize metrics for comparison
    static PerformanceMetrics normalizeMetrics(const PerformanceMetrics& metrics, const PerformanceMetrics& baseline);
    
    // Compare benchmarks
    static std::vector<std::pair<std::string, double>> compareBenchmarks(const std::vector<BenchmarkResult>& results);
    
    // Generate benchmark recommendation
    static std::string generateBenchmarkRecommendation(const BenchmarkResult& result);
    
    // Validate benchmark configuration
    static bool validateBenchmarkConfig(const BenchmarkConfig& config);
    
    // Get benchmark preset names
    static std::vector<std::string> getBenchmarkPresetNames();
    
    // Get experiment types
    static std::vector<std::string> getExperimentTypes();
    
    // Get plasticity modes
    static std::vector<std::string> getPlasticityModes();
    
    // Get output formats
    static std::vector<std::string> getOutputFormats();
    
    // Convert time string to seconds
    static double timeStringToSeconds(const std::string& timeStr);
    
    // Convert seconds to time string
    static std::string secondsToTimeString(double seconds);
    
    // Format number for display
    static std::string formatNumber(double number, int precision = 2);
    
    // Format time for display
    static std::string formatTime(double seconds);
    
    // Format memory for display
    static std::string formatMemory(size_t bytes);
    
    // Format rate for display
    static std::string formatRate(double value, const std::string& unit);
    
    // Calculate throughput
    static double calculateThroughput(size_t items, double time);
    
    // Calculate efficiency
    static double calculateEfficiency(double usefulWork, double totalResources);
    
    // Calculate scalability
    static double calculateScalability(const std::vector<BenchmarkResult>& results);
    
    // Predict performance
    static PerformanceMetrics predictPerformance(const PerformanceMetrics& currentMetrics, double scalingFactor);
    
    // Validate performance metrics
    static bool validatePerformanceMetrics(const PerformanceMetrics& metrics);
    
    // Generate performance statistics
    static std::string generatePerformanceStatistics(const std::vector<PerformanceMetrics>& metrics);
    
    // Generate performance summary
    static std::string generatePerformanceSummary(const BenchmarkResult& result);
    
    // Generate performance comparison
    static std::string generatePerformanceComparison(const std::vector<BenchmarkResult>& results);
    
    // Generate performance recommendation
    static std::string generatePerformanceRecommendation(const BenchmarkResult& result);
    
    // Export benchmark results
    static bool exportBenchmarkResults(const std::string& filepath, const std::vector<BenchmarkResult>& results);
    
    // Import benchmark results
    static bool importBenchmarkResults(const std::string& filepath, std::vector<BenchmarkResult>& results);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global performance monitor instance
extern PerformanceMonitor& getPerformanceMonitor();

// Global benchmark utilities instance
extern BenchmarkUtils& getBenchmarkUtils();

// Convenience functions for performance monitoring
inline void startPerformanceMonitoring() {
    getPerformanceMonitor().startMonitoring();
}

inline void stopPerformanceMonitoring() {
    getPerformanceMonitor().stopMonitoring();
}

inline void recordPerformanceMetrics(const PerformanceMetrics& metrics) {
    getPerformanceMonitor().recordMetrics(metrics);
}

inline PerformanceMetrics getCurrentPerformanceMetrics() {
    return getPerformanceMonitor().getCurrentMetrics();
}

inline void startBenchmark(const BenchmarkConfig& config) {
    getPerformanceMonitor().startBenchmark(config);
}

inline void recordBenchmarkProgress(const PerformanceMetrics& metrics) {
    getPerformanceMonitor().recordBenchmarkProgress(metrics);
}

inline BenchmarkResult completeBenchmark() {
    return getPerformanceMonitor().completeBenchmark();
}

inline std::string generatePerformanceReport(const BenchmarkResult& result, const std::string& format = "text") {
    return getPerformanceMonitor().generateReport(result, format);
}

inline bool savePerformanceReport(const std::string& filepath, const BenchmarkResult& result, const std::string& format = "text") {
    return getPerformanceMonitor().saveReport(filepath, result, format);
}

inline std::vector<BenchmarkConfig> getBenchmarkPresets() {
    return getBenchmarkUtils().getStandardBenchmarks();
}

inline std::string generateBenchmarkRecommendation(const BenchmarkResult& result) {
    return getBenchmarkUtils().generateBenchmarkRecommendation(result);
}

inline double calculateBenchmarkScore(const PerformanceMetrics& metrics) {
    return getBenchmarkUtils().calculateBenchmarkScore(metrics);
}

inline double calculateEfficiencyScore(const PerformanceMetrics& metrics, double baselineTime) {
    return getBenchmarkUtils().calculateEfficiencyScore(metrics, baselineTime);
}

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP
