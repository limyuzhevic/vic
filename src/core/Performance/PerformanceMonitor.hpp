// Performance monitoring and profiling system for NLM
// Core performance infrastructure for real-time monitoring, profiling, and optimization

#pragma once

#include <chrono>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <optional>
#include <variant>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Performance metrics data structures
struct PerformanceMetrics {
    double timestamp;  // Wall clock time in seconds
    double cpu_usage;  // CPU utilization percentage
    double memory_usage;  // Memory utilization percentage
    uint64_t memory_bytes;  // Memory usage in bytes
    uint64_t active_neurons;  // Number of active neurons
    uint64_t total_spikes;  // Total spike count
    double spike_rate;  // Spikes per second
    uint64_t synaptic_operations;  // Number of synaptic operations
    uint64_t event_queue_size;  // Current event queue size
    double simulation_time_step;  // Current simulation timestep
    uint64_t simulation_steps;  // Number of simulation steps
    uint32_t active_threads;  // Number of active worker threads
    std::unordered_map<std::string, double> region_metrics;  // Per-region metrics
    std::unordered_map<std::string, double> system_metrics;  // Per-system metrics
};

// Memory profiling data structures
struct MemoryProfile {
    std::string timestamp;
    std::string allocation_type;
    size_t size_bytes;
    size_t count;
    double cumulative_time;
    size_t memory_pool_id;
    std::string stack_trace;
    bool is_leak;
    std::string allocation_site;
};

// CPU profiling data structures
struct CpuProfile {
    std::string timestamp;
    std::string function_name;
    std::string file_path;
    int line_number;
    uint64_t instruction_count;
    uint64_t cycles;
    uint64_t cache_misses;
    double self_time;  // Time spent in this function excluding children
    double total_time;  // Total time including children
    double cpu_usage;
    int thread_id;
    std::string stack_trace;
};

// I/O profiling data structures
struct IoProfile {
    std::string timestamp;
    std::string operation_type;  // "read", "write", "seek"
    std::string file_path;
    size_t bytes_transferred;
    double duration;
    int thread_id;
    bool is_blocking;
    std::string stack_trace;
};

// Network profiling data structures
struct NetworkProfile {
    std::string timestamp;
    std::string endpoint;
    std::string protocol;  // "TCP", "UDP", "HTTP"
    size_t bytes_sent;
    size_t bytes_received;
    double duration;
    bool is_encrypted;
    int thread_id;
    std::string stack_trace;
};

// Performance alert types
enum class AlertType {
    CPU_USAGE_HIGH,
    MEMORY_USAGE_HIGH,
    SPIKE_RATE_ANOMALY,
    LATENCY_THRESHOLD_EXCEEDED,
    SYNAPTIC_PERFORMANCE_DEGRADED,
    EVENT_QUEUE_BACKLOG,
    MEMORY_LEAK_DETECTED,
    THREAD_POOL_EXHAUSTION,
    NETWORK_LATENCY_HIGH
};

struct PerformanceAlert {
    AlertType type;
    std::string message;
    double severity;  // 0.0 to 1.0
    std::string timestamp;
    std::string context;
    std::function<void()> action;  // Optional alert action
};

// Performance benchmark specifications
struct BenchmarkConfig {
    std::string name;
    uint64_t neuron_count;
    uint64_t synapse_count;
    double duration_seconds;
    std::unordered_map<std::string, std::string> parameters;
    bool enabled;
    std::string description;
};

// Performance regression data
struct RegressionData {
    std::string benchmark_name;
    double baseline_performance;
    double current_performance;
    double regression_percentage;
    std::string regression_detected_at;
    std::string regression_threshold;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();

    // Start monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }

    // Record performance metrics
    void recordMetrics(const PerformanceMetrics& metrics);
    void updateNeuronCount(uint64_t count);
    void updateSpikeCount(uint64_t count);
    void updateMemoryUsage(size_t bytes);
    void updateCpuUsage(double usage);
    void updateEventQueueSize(uint64_t size);

    // Profile specific operations
    void profileMemoryAllocation(const MemoryProfile& profile);
    void profileCpu(const CpuProfile& profile);
    void profileIo(const IoProfile& profile);
    void profileNetwork(const NetworkProfile& profile);

    // Alert management
    void addAlert(const PerformanceAlert& alert);
    std::vector<PerformanceAlert> getAlerts() const;
    void clearAlerts();
    void setAlertThreshold(AlertType type, double threshold);

    // Benchmarking
    void addBenchmark(const BenchmarkConfig& config);
    void runBenchmark(const std::string& benchmark_name);
    std::vector<RegressionData> getRegressionData() const;

    // Data access
    PerformanceMetrics getLatestMetrics() const;
    std::vector<PerformanceMetrics> getMetricsHistory(size_t count) const;
    std::vector<MemoryProfile> getMemoryProfiles() const;
    std::vector<CpuProfile> getCpuProfiles() const;

    // Configuration
    void setMonitoringInterval(double interval_seconds);
    void enableRealTimeMonitoring(bool enable);
    void enableProfiling(bool enable);

    // Statistics and analysis
    double getAverageCpuUsage() const;
    double getPeakMemoryUsage() const;
    double getAverageSpikeRate() const;
    std::string generatePerformanceReport() const;

private:
    bool monitoring_;
    double monitoring_interval_;
    bool real_time_monitoring_;
    bool profiling_enabled_;
    
    mutable std::mutex metrics_mutex_;
    mutable std::mutex alerts_mutex_;
    
    std::vector<PerformanceMetrics> metrics_history_;
    std::vector<MemoryProfile> memory_profiles_;
    std::vector<CpuProfile> cpu_profiles_;
    std::vector<IoProfile> io_profiles_;
    std::vector<NetworkProfile> network_profiles_;
    std::vector<PerformanceAlert> alerts_;
    
    std::unordered_map<std::string, double> alert_thresholds_;
    std::unordered_map<std::string, double> benchmark_baselines_;
    std::unordered_map<std::string, BenchmarkConfig> benchmarks_;
    
    std::chrono::steady_clock::time_point start_time_;
    uint64_t total_steps_;
    uint64_t total_spikes_;
    
    std::thread monitoring_thread_;
    std::condition_variable monitoring_cv_;
    std::atomic<bool> monitoring_running_;

    void monitoringLoop();
    void checkAlerts();
    void cleanupOldData();
    std::string generateStackTrace() const;
    std::string formatTimestamp(double timestamp) const;
};

// Global performance monitor instance
class PerformanceManager {
public:
    static PerformanceManager& getInstance() {
        static PerformanceManager instance;
        return instance;
    }

    void initialize();
    void shutdown();
    PerformanceMonitor& getMonitor() { return monitor_; }

private:
    PerformanceManager() : monitor_() {}
    PerformanceMonitor monitor_;
};

// Performance profiling utilities
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();

    void startProfiling(const std::string& name);
    void stopProfiling();
    void profileFunction(const std::string& name, std::function<void()> func);

    std::vector<CpuProfile> getCpuProfiles() const { return cpu_profiles_; }
    void saveProfiles(const std::string& directory) const;

private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> profiling_sessions_;
    std::vector<CpuProfile> cpu_profiles_;
    mutable std::mutex profiler_mutex_;
    int current_thread_id_;
};

// Performance visualization utilities
class PerformanceVisualizer {
public:
    PerformanceVisualizer();
    ~PerformanceVisualizer();

    void generatePerformanceDashboard(const std::string& output_file);
    void generateHistoricalGraphs(const std::string& output_directory);
    void generateAlertReport(const std::string& output_file);
    void generateBenchmarkReport(const std::string& output_file);

private:
    void createCpuUsageChart(const std::vector<PerformanceMetrics>& metrics, const std::string& output_file);
    void createMemoryUsageChart(const std::vector<PerformanceMetrics>& metrics, const std::string& output_file);
    void createSpikeRateChart(const std::vector<PerformanceMetrics>& metrics, const std::string& output_file);
    void createAlertTimeline(const std::vector<PerformanceAlert>& alerts, const std::string& output_file);
    void createBenchmarkComparison(const std::vector<RegressionData>& regressions, const std::string& output_file);
};

// Performance optimization assistant
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();

    std::vector<std::string> analyzePerformanceBottlenecks(const PerformanceMetrics& metrics);
    std::vector<std::string> getOptimizationRecommendations(const PerformanceMetrics& metrics);
    double estimatePerformanceGain(const std::string& optimization_type) const;
    void applyAutomaticOptimization(const std::string& optimization_type);

private:
    std::unordered_map<std::string, double> optimization_effects_;
};
}