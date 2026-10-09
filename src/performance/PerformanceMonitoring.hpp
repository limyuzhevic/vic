// Performance Monitoring System for NLM
// Provides comprehensive performance tracking for neural computation optimizations

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Performance Metrics
 * Comprehensive statistics for neural computation tracking
 */
struct PerformanceMetrics {
    // Core performance counters
    std::atomic<size_t> total_neurons{0};
    std::atomic<size_t> total_synapses{0};
    std::atomic<size_t> active_neurons{0};
    std::atomic<size_t> firing_neurons{0};
    std::atomic<size_t> total_spikes{0};
    
    // Performance counters
    std::atomic<size_t> simulation_steps{0};
    std::atomic<size_t> pending_spike_events{0};
    std::atomic<size_t> memory_allocations{0};
    std::atomic<size_t> memory_deallocations{0};
    
    // Time tracking
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point last_step_time;
    std::chrono::steady_clock::time_point peak_step_time;
    
    // Optimization effectiveness tracking
    std::atomic<size_t> simd_operations{0};
    std::atomic<size_t> parallel_updates{0};
    std::atomic<size_t> memory_pool_allocs{0};
    std::atomic<size_t> event_queue_operations{0};
    
    // Memory tracking
    std::atomic<size_t> total_memory_usage{0};
    std::atomic<size_t> peak_memory_usage{0};
    
    // Statistical tracking
    std::vector<double> spike_rates_history;
    std::vector<double> firing_rates_history;
    std::vector<double> memory_usage_history;
    
    PerformanceMetrics() : start_time(std::chrono::steady_clock::now()) {}
};

/**
 * Performance Benchmark Suite
 * Tools for measuring and comparing optimization effectiveness
 */
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double execution_time;
        size_t neurons_processed;
        size_t spikes_processed;
        size_t memory_usage;
        double efficiency_score;
        std::map<std::string, double> metrics;
        
        BenchmarkResult(const std::string& n, double time, size_t neurons,
                       size_t spikes, size_t memory, double efficiency)
            : name(n), execution_time(time), neurons_processed(neurons),
              spikes_processed(spikes), memory_usage(memory),
              efficiency_score(efficiency) {}
    };
    
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    // Disable copying
    PerformanceBenchmark(const PerformanceBenchmark&) = delete;
    PerformanceBenchmark& operator=(const PerformanceBenchmark&) = delete;
    
    /**
     * Run a performance benchmark
     * @param benchmarkName Name of the benchmark
     * @param neurons_count Number of neurons to simulate
     * @param steps Number of simulation steps
     * @return Benchmark result
     */
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                 size_t neurons_count, size_t steps);
    
    /**
     * Get benchmark history
     */
    const std::vector<BenchmarkResult>& getBenchmarkHistory() const { return history_; }
    
    /**
     * Get average execution time for benchmarks
     */
    double getAverageExecutionTime() const;
    
    /**
     * Get performance improvement report
     */
    std::string generatePerformanceReport() const;
    
    /**
     * Export benchmarks to CSV
     */
    bool exportToCSV(const std::string& filepath) const;
    
    /**
     * Clear benchmark history
     */
    void clearHistory();
    
    /**
     * Get benchmark statistics
     */
    struct BenchmarkStats {
        size_t total_benchmarks{0};
        double average_time{0.0};
        double min_time{0.0};
        double max_time{0.0};
        double total_neurons_processed{0};
        double total_spikes_processed{0};
        double average_efficiency{0.0};
    };
    
    BenchmarkStats getStatistics() const;
    
private:
    std::vector<BenchmarkResult> history_;
    mutable std::mutex history_mutex_;
    std::chrono::steady_clock::time_point session_start_time_;
};

/**
 * Performance Monitor
 * Real-time performance monitoring for neural simulation
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Start performance monitoring
     */
    void startMonitoring();
    
    /**
     * Stop performance monitoring
     */
    void stopMonitoring();
    
    /**
     * Record performance metrics for a simulation step
     */
    void recordStep(const PerformanceMetrics& metrics, size_t step);
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getCurrentMetrics() const;
    
    /**
     * Get performance statistics over time
     */
    struct PerformanceStats {
        size_t total_steps{0};
        size_t max_active_neurons{0};
        size_t total_spikes{0};
        double average_firing_rate{0.0};
        double average_spike_rate{0.0};
        double memory_usage_mb{0.0};
        double simulation_speed{0.0}; // steps per second
        std::chrono::duration<double> uptime{0.0};
    };
    
    PerformanceStats getPerformanceStats() const;
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * Get monitoring session duration
     */
    double getSessionDuration() const;
    
    /**
     * Get performance trend analysis
     */
    struct TrendAnalysis {
        double spike_rate_trend{0.0};        // Positive = increasing, Negative = decreasing
        double firing_rate_trend{0.0};
        double memory_usage_trend{0.0};
        double performance_trend{0.0};       // Overall performance trend
    };
    
    TrendAnalysis getTrendAnalysis(size_t window_size = 100) const;
    
    /**
     * Reset performance statistics
     */
    void reset();
    
    /**
     * Get detailed metrics history
     */
    const std::vector<PerformanceMetrics>& getMetricsHistory() const { return metrics_history_; }
    
    /**
     * Compare performance against baseline
     */
    struct PerformanceComparison {
        std::string metric_name;
        double baseline_value{0.0};
        double current_value{0.0};
        double improvement_percentage{0.0};
        std::string trend_description;
    };
    
    std::vector<PerformanceComparison> compareWithBaseline(
        const std::map<std::string, double>& baseline) const;
    
private:
    bool monitoring_;
    PerformanceMetrics current_metrics_;
    std::vector<PerformanceMetrics> metrics_history_;
    mutable std::mutex metrics_mutex_;
    std::chrono::steady_clock::time_point monitoring_start_time_;
    std::chrono::steady_clock::time_point session_start_time_;
    
    // Internal helper methods
    void updateMetrics(const PerformanceMetrics& new_metrics);
    double calculateTrend(const std::vector<double>& data, size_t window) const;
};

/**
 * Optimization Effectiveness Tracker
 * Tracks how well performance optimizations are working
 */
class OptimizationEffectivenessTracker {
public:
    OptimizationEffectivenessTracker();
    ~OptimizationEffectivenessTracker();
    
    // Disable copying
    OptimizationEffectivenessTracker(const OptimizationEffectivenessTracker&) = delete;
    OptimizationEffectivenessTracker& operator=(const OptimizationEffectivenessTracker&) = delete;
    
    /**
     * Record optimization operation
     */
    void recordOptimization(const std::string& optimization_type, 
                           double execution_time, size_t objects_processed);
    
    /**
     * Get optimization effectiveness statistics
     */
    struct OptimizationStats {
        size_t total_operations{0};
        double total_execution_time{0.0};
        size_t total_objects_processed{0};
        double average_efficiency{0.0};
        std::map<std::string, double> effectiveness_by_type;
    };
    
    OptimizationStats getOptimizationStats() const;
    
    /**
     * Get effectiveness report for specific optimization
     */
    std::string getEffectivenessReport(const std::string& optimization_type) const;
    
    /**
     * Compare optimization effectiveness
     */
    struct OptimizationComparison {
        std::string optimization_type;
        double before_value{0.0};    // Value without optimization
        double after_value{0.0};     // Value with optimization
        double improvement_factor{0.0};  // after_value / before_value
        std::string impact_level;    // "minimal", "moderate", "significant", "major"
    };
    
    std::vector<OptimizationComparison> compareOptimizations(
        const std::map<std::string, double>& baseline) const;
    
    /**
     * Get overall optimization impact
     */
    struct OverallImpact {
        double total_performance_improvement{0.0};
        double total_memory_improvement{0.0};
        double total_efficiency_improvement{0.0};
        std::vector<std::string> most_effective_optimizations;
        std::vector<std::string> least_effective_optimizations;
    };
    
    OverallImpact getOverallImpact() const;
    
    /**
     * Generate effectiveness analysis
     */
    std::string generateEffectivenessAnalysis() const;
    
private:
    struct OptimizationRecord {
        std::string type;
        double execution_time;
        size_t objects_processed;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::vector<OptimizationRecord> records_;
    mutable std::mutex records_mutex_;
    
    // Impact level classification
    std::string classifyImpactLevel(double improvement_factor) const;
    
    // Effectiveness calculation
    double calculateEffectiveness(const std::string& optimization_type) const;
};

/**
 * Performance Reporter
 * Generates comprehensive performance reports
 */
class PerformanceReporter {
public:
    PerformanceReporter();
    ~PerformanceReporter();
    
    // Disable copying
    PerformanceReporter(const PerformanceReporter&) = delete;
    PerformanceReporter& operator=(const PerformanceReporter&) = delete;
    
    /**
     * Generate comprehensive performance report
     */
    std::string generateReport(const PerformanceMonitor& monitor,
                              const PerformanceBenchmark& benchmark,
                              const OptimizationEffectivenessTracker& tracker) const;
    
    /**
     * Generate HTML report
     */
    bool generateHTMLReport(const std::string& filepath,
                           const PerformanceMonitor& monitor,
                           const PerformanceBenchmark& benchmark,
                           const OptimizationEffectivenessTracker& tracker) const;
    
    /**
     * Generate JSON report
     */
    bool generateJSONReport(const std::string& filepath,
                           const PerformanceMonitor& monitor,
                           const PerformanceBenchmark& benchmark,
                           const OptimizationEffectivenessTracker& tracker) const;
    
    /**
     * Generate text report for console output
     */
    void generateTextReport(std::ostream& output,
                           const PerformanceMonitor& monitor,
                           const PerformanceBenchmark& benchmark,
                           const OptimizationEffectivenessTracker& tracker) const;
    
    /**
     * Generate performance summary
     */
    std::string generatePerformanceSummary(const PerformanceMonitor& monitor) const;
    
    /**
     * Generate benchmark summary
     */
    std::string generateBenchmarkSummary(const PerformanceBenchmark& benchmark) const;
    
    /**
     * Generate optimization effectiveness summary
     */
    std::string generateOptimizationSummary(const OptimizationEffectivenessTracker& tracker) const;
    
private:
    // Report generation helpers
    std::string formatTime(double seconds) const;
    std::string formatMemory(size_t bytes) const;
    std::string formatPercentage(double value) const;
    
    // Report content generation
    std::string generateExecutiveSummary(const PerformanceMonitor& monitor,
                                        const PerformanceBenchmark& benchmark,
                                        const OptimizationEffectivenessTracker& tracker) const;
    
    std::string generatePerformanceDetails(const PerformanceMonitor& monitor) const;
    std::string generateBenchmarkDetails(const PerformanceBenchmark& benchmark) const;
    std::string generateOptimizationDetails(const OptimizationEffectivenessTracker& tracker) const;
    std::string generateRecommendations(const PerformanceMonitor& monitor,
                                        const OptimizationEffectivenessTracker& tracker) const;
    
    // Trend analysis for reports
    std::string generateTrendAnalysis(const PerformanceMonitor& monitor) const;
    std::string generateComparisonAnalysis(const PerformanceMonitor& monitor,
                                           const OptimizationEffectivenessTracker& tracker) const;
};

/**
 * Global performance monitoring interface
 */
class GlobalPerformanceMonitor {
public:
    static GlobalPerformanceMonitor& getInstance();
    
    // Disable copying
    GlobalPerformanceMonitor(const GlobalPerformanceMonitor&) = delete;
    GlobalPerformanceMonitor& operator=(const GlobalPerformanceMonitor&) = delete;
    
    /**
     * Get performance monitor instance
     */
    PerformanceMonitor& getMonitor() { return monitor_; }
    
    /**
     * Get performance benchmark instance
     */
    PerformanceBenchmark& getBenchmark() { return benchmark_; }
    
    /**
     * Get optimization effectiveness tracker instance
     */
    OptimizationEffectivenessTracker& getOptimizationTracker() { return optimization_tracker_; }
    
    /**
     * Get performance reporter instance
     */
    PerformanceReporter& getReporter() { return reporter_; }
    
    /**
     * Start global performance monitoring
     */
    void startMonitoring() { monitor_.startMonitoring(); }
    
    /**
     * Stop global performance monitoring
     */
    void stopMonitoring() { monitor_.stopMonitoring(); }
    
    /**
     * Record simulation step in global performance monitoring
     */
    void recordStep(const PerformanceMetrics& metrics, size_t step) {
        monitor_.recordStep(metrics, step);
    }
    
    /**
     * Run performance benchmark
     */
    PerformanceBenchmark::BenchmarkResult runBenchmark(const std::string& name,
                                                        size_t neurons, size_t steps) {
        return benchmark_.runBenchmark(name, neurons, steps);
    }
    
    /**
     * Generate global performance report
     */
    std::string generateReport() {
        return reporter_.generateReport(monitor_, benchmark_, optimization_tracker_);
    }
    
private:
    GlobalPerformanceMonitor() : monitor_(), benchmark_(), optimization_tracker_(), reporter_() {}
    
    PerformanceMonitor monitor_;
    PerformanceBenchmark benchmark_;
    OptimizationEffectivenessTracker optimization_tracker_;
    PerformanceReporter reporter_;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP
