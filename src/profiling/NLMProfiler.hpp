// NLM Performance Profiling and Benchmarking
// Comprehensive profiling tools for NLM simulations

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>
#include <sstream>

namespace nlm {

/**
 * Profiler configuration
 */
struct ProfilerConfig {
    bool enableEventRecording;
    bool enableSampling;
    bool enableMetrics;
    double eventRecordingInterval;
    size_t maxEvents;
    std::string outputPath;
    std::string logLevel;
    
    ProfilerConfig();
    bool validate() const;
    std::string getOutputPath(const std::string& suffix = "") const;
};

/**
 * Event data structure for profiling
 */
struct EventData {
    std::string name;
    double duration;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::thread::id threadId;
};

/**
 * Sample data structure for profiling
 */
struct SampleData {
    std::string name;
    double value;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::thread::id threadId;
};

/**
 * Metric data structure for profiling
 */
struct MetricData {
    std::string name;
    double value;
    std::string category;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::thread::id threadId;
};

/**
 * Statistics for events
 */
struct EventStats {
    size_t count;
    double totalDuration;
    double minDuration;
    double maxDuration;
};

/**
 * Main performance profiler for NLM
 */
class NLMProfiler {
public:
    NLMProfiler();
    ~NLMProfiler();
    
    // Control
    void start();
    void stop();
    
    // Event recording
    void recordEvent(const std::string& eventName, double duration);
    void startTimer(const std::string& timerName);
    void stopTimer(const std::string& timerName);
    
    // Sampling
    void recordSample(const std::string& sampleName, double value);
    
    // Metrics
    void addMetric(const std::string& name, double value, const std::string& category = "");
    
    // Getters
    const std::vector<EventData>& getEvents() const;
    const std::vector<SampleData>& getSamples() const;
    const std::vector<MetricData>& getMetrics() const;
    const std::map<std::string, EventStats>& getEventStats() const;
    
    double getTotalDuration() const;
    
    // Reporting
    void generateReport(const std::string& outputPath);
    
    // Utility
    static std::string getCurrentTimestamp();

private:
    bool _isRunning;
    std::chrono::high_resolution_clock::time_point _startTime;
    double _totalDuration;
    
    mutable std::mutex _mutex;
    
    std::vector<EventData> _events;
    std::vector<SampleData> _samples;
    std::vector<MetricData> _metrics;
    std::map<std::string, EventStats> _eventStats;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> _timers;
};

/**
 * Performance analyzer for NLM profiles
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    void analyzeProfiles(const std::vector<std::string>& profileFiles);
    void analyzeProfile(const std::string& profileFile);
    void generateComparativeReport();
    void identifyBottlenecks();
    void generateOptimizationRecommendations();
    
private:
    NLMProfiler _profiler; // For convenience
};

/**
 * Benchmark suite for NLM performance testing
 */
class BenchmarkSuite {
public:
    struct BenchmarkData {
        std::string name;
        std::function<void()> function;
        int iterations;
        double totalTime;
    };
    
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    void addBenchmark(const std::string& name, std::function<void()> benchmarkFunction);
    void runBenchmarks();
    
    void setIterations(size_t iterations) {
        _defaultIterations = iterations;
    }
    
private:
    void runBenchmark(BenchmarkData& benchmark);
    void generateReport();
    
    std::vector<BenchmarkData> _benchmarks;
    size_t _defaultIterations;
};

} // namespace nlm