#pragma once

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace nlm {

/**
 * Performance profiling for NLM (Neural Learning Machine)
 * 
 * Comprehensive performance monitoring system that tracks:
 * - Execution time for different operations
 * - Memory usage patterns
 * - CPU utilization
 * - Network I/O (if applicable)
 * - Custom event timing
 * - Performance benchmarks
 * - Real-time monitoring
 * 
 * Features:
 * - Hierarchical timing measurements
 * - Thread-safe profiling
 * - Multiple output formats (JSON, text, CSV)
 * - Performance benchmarks
 * - Memory profiling
 * - System resource monitoring
 * - Custom event tracking
 * 
 * Usage:
 * @code
 * // Start profiling
 * auto profiler = PerformanceProfiler::getInstance();
 * profiler->start("simulation");
 * 
 * // Profile different operations
 * profiler->start("brain_step");
 * brain->step(step);
 * profiler->stop("brain_step");
 * 
 * // Profile nested operations
 * profiler->start("neuron_update");
 * for (auto& neuron : neurons) {
 *     neuron->update();
 * }
 * profiler->stop("neuron_update");
 * 
 * // Get results
 * auto results = profiler->getResults();
 * profiler->printReport();
 * @endcode
 */
class PerformanceProfiler {
public:
    struct ProfileResult {
        std::string name;
        std::chrono::nanoseconds duration;
        uint64_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
        double totalTime;
        std::map<std::string, ProfileResult> children;
        
        ProfileResult(const std::string& n) 
            : name(n), duration(0), callCount(0), averageTime(0.0),
              minTime(0.0), maxTime(0.0), totalTime(0.0) {}
        
        void addChild(const std::string& childName) {
            children[childName] = ProfileResult(childName);
        }
        
        ProfileResult& getChild(const std::string& childName) {
            if (children.find(childName) == children.end()) {
                addChild(childName);
            }
            return children[childName];
        }
    };
    
    struct SystemMetrics {
        double cpuUsage;           // Percentage (0-100)
        double memoryUsage;        // MB
        double memoryPercent;      // Percentage (0-100)
        uint64_t totalMemory;      // Total system memory (MB)
        uint64_t processMemory;    // Process memory (MB)
        std::chrono::steady_clock::time_point timestamp;
        
        SystemMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryPercent(0.0),
                         totalMemory(0), processMemory(0) {
            timestamp = std::chrono::steady_clock::now();
        }
    };
    
    struct BenchmarkResult {
        std::string name;
        std::string description;
        std::chrono::nanoseconds duration;
        uint64_t neurons;
        uint64_t synapses;
        uint64_t steps;
        double spikesPerSecond;
        double performanceScore;
        std::map<std::string, std::string> metrics;
        
        BenchmarkResult(const std::string& n, const std::string& desc)
            : name(n), description(desc), duration(0), neurons(0),
              synapses(0), steps(0), spikesPerSecond(0.0), performanceScore(0.0) {}
    };
    
    class ProfilerScope {
    public:
        ProfilerScope(PerformanceProfiler* profiler, const std::string& name)
            : profiler_(profiler), name_(name), startTime_(std::chrono::steady_clock::now()) {
            if (profiler_) {
                profiler_->start(name_);
            }
        }
        
        ~ProfilerScope() {
            if (profiler_) {
                profiler_->stop(name_);
            }
        }
        
        std::chrono::nanoseconds getDuration() const {
            return std::chrono::steady_clock::now() - startTime_;
        }
        
    private:
        PerformanceProfiler* profiler_;
        std::string name_;
        std::chrono::steady_clock::time_point startTime_;
    };
    
    static PerformanceProfiler* getInstance();
    static void resetInstance();
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Start profiling a named operation
     * 
     * @param name Name of the operation to profile
     */
    void start(const std::string& name);
    
    /**
     * Stop profiling the current operation
     * 
     * @param name Name of the operation to stop
     * @return Duration of the operation
     */
    std::chrono::nanoseconds stop(const std::string& name);
    
    /**
     * Start a nested profiling scope
     * 
     * @param name Name of the operation
     * @return Profiler scope object
     */
    ProfilerScope profile(const std::string& name);
    
    /**
     * Get results from profiling
     * 
     * @return Map of operation names to profile results
     */
    const std::map<std::string, ProfileResult>& getResults() const { return results_; }
    
    /**
     * Get results by name
     * 
     * @param name Operation name
     * @return Profile result
     */
    const ProfileResult* getResult(const std::string& name) const;
    
    /**
     * Collect system metrics
     * 
     * @return System metrics including CPU and memory usage
     */
    SystemMetrics collectSystemMetrics() const;
    
    /**
     * Run performance benchmarks
     * 
     * @param benchmarkName Name of benchmark
     * @param description Description of benchmark
     * @param neurons Number of neurons
     * @param steps Number of simulation steps
     * @return Benchmark result
     */
    BenchmarkResult runBenchmark(const std::string& benchmarkName, 
                                 const std::string& description,
                                 uint64_t neurons, uint64_t steps);
    
    /**
     * Get all benchmark results
     */
    const std::vector<BenchmarkResult>& getBenchmarks() const { return benchmarks_; }
    
    /**
     * Print profiling report to console
     * 
     * @param reportType Type of report (detailed, summary, csv)
     */
    void printReport(const std::string& reportType = "detailed") const;
    
    /**
     * Save profiling results to file
     * 
     * @param filename Output file name
     * @param format Output format (json, csv, txt)
     */
    bool saveResults(const std::string& filename, const std::string& format = "json");
    
    /**
     * Load profiling results from file
     * 
     * @param filename Input file name
     * @param format Input format (json, csv, txt)
     * @return true if successful
     */
    bool loadResults(const std::string& filename, const std::string& format = "json");
    
    /**
     * Reset all profiling data
     */
    void reset();
    
    /**
     * Check if profiler is active
     */
    bool isActive() const { return active_; }
    
    /**
     * Enable or disable profiler
     */
    void setActive(bool active) { active_ = active; }
    
    /**
     * Get total profiling time
     */
    std::chrono::nanoseconds getTotalTime() const { return totalTime_; }
    
    /**
     * Get number of active profiles
     */
    size_t getActiveProfileCount() const { return activeProfiles_.size(); }
    
    /**
     * Get performance score (higher is better)
     * 
     * @return Performance score
     */
    double getPerformanceScore() const;
    
private:
    bool active_;
    std::map<std::string, ProfileResult> results_;
    std::map<std::string, std::chrono::steady_clock::time_point> activeProfiles_;
    std::chrono::steady_clock::time_point sessionStart_;
    std::vector<BenchmarkResult> benchmarks_;
    mutable SystemMetrics lastSystemMetrics_;
    
    void updateProfile(const std::string& name, std::chrono::nanoseconds duration);
    void recursivePrint(const ProfileResult& result, int depth = 0) const;
    void writeJSON(std::ostream& out) const;
    void writeCSV(std::ostream& out) const;
    void writeTXT(std::ostream& out) const;
    double calculatePerformanceScore() const;
};

} // namespace nlm
