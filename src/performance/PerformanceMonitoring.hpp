#pragma once

/**
 * Performance Monitoring Infrastructure
 * 
 * Comprehensive performance monitoring and profiling system for NLM
 * Lightweight, minimal overhead monitoring for neural simulation performance
 */

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

// Performance monitoring configuration
struct PerformanceConfig {
    bool enable_monitoring = true;
    bool enable_profiling = false;
    bool enable_metrics = true;
    bool enable_alerting = false;
    uint32_t sampling_interval_ms = 100;
    uint32_t profile_sample_rate = 1000; // Sample every N steps
    uint32_t max_history_samples = 10000;
    std::string metrics_output_file = "";
    bool enable_profiler_output = false;
    std::string profiler_output_file = "";
};

// Performance metrics
struct PerformanceMetrics {
    uint64_t timestamp_ns = 0;
    uint32_t simulation_step = 0;
    double cpu_usage_percent = 0.0;
    double memory_usage_mb = 0.0;
    uint64_t active_neurons = 0;
    uint64_t total_spikes = 0;
    double average_firing_rate = 0.0;
    double simulation_time_ms = 0.0;
    uint32_t thread_count = 0;
    uint64_t memory_allocated = 0;
    uint64_t memory_pool_allocated = 0;
    uint64_t memory_pool_used = 0;
};

// Profiling data structure
struct ProfileEvent {
    uint64_t timestamp_ns = 0;
    std::string function_name;
    uint64_t duration_ns = 0;
    uint32_t thread_id = 0;
    std::string file_name;
    uint32_t line_number = 0;
};

// Performance alerting condition
struct PerformanceAlert {
    enum class AlertType {
        CPU_HIGH,
        MEMORY_HIGH,
        SPIKE_RATE_HIGH,
        SIMULATION_SLOW,
        THREAD_COUNT_HIGH
    };
    
    AlertType type;
    std::string condition;
    double threshold_value;
    double current_value;
    std::string severity; // "WARNING", "ERROR", "CRITICAL"
    std::string message;
    uint64_t timestamp_ns = 0;
};

/**
 * Main performance monitoring class
 */
class PerformanceMonitor {
private:
    PerformanceConfig config;
    std::vector<PerformanceMetrics> metrics_history;
    std::vector<ProfileEvent> profile_events;
    std::vector<PerformanceAlert> alerts;
    mutable std::mutex metrics_mutex;
    mutable std::mutex profile_mutex;
    mutable std::mutex alert_mutex;
    
    std::chrono::time_point<std::chrono::steady_clock> last_sampling_time;
    uint64_t total_simulation_steps = 0;
    uint64_t total_spikes = 0;
    
    // Thread-local storage for per-thread metrics
    struct ThreadMetrics {
        uint64_t local_spikes = 0;
        double local_cpu_time = 0.0;
        uint64_t thread_id = 0;
    };
    std::vector<ThreadMetrics> thread_metrics;
    mutable std::mutex thread_mutex;

public:
    PerformanceMonitor(const PerformanceConfig& config = PerformanceConfig());
    ~PerformanceMonitor();
    
    // Configuration
    void configure(const PerformanceConfig& config);
    
    // Metrics collection
    void recordMetrics(const PerformanceMetrics& metrics);
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getMetricsHistory() const;
    
    // Profiling
    void recordProfileEvent(const ProfileEvent& event);
    void startProfile(const std::string& function_name, const std::string& file_name = "", uint32_t line_number = 0);
    void stopProfile(const std::string& function_name);
    std::vector<ProfileEvent> getProfileEvents() const;
    
    // Alerting
    void addAlertCondition(const PerformanceAlert& alert);
    std::vector<PerformanceAlert> getActiveAlerts() const;
    void clearAlerts();
    
    // Thread management
    void registerThread(uint32_t thread_id);
    void unregisterThread(uint32_t thread_id);
    
    // Utilities
    void saveMetricsToFile(const std::string& filename) const;
    void saveProfileToFile(const std::string& filename) const;
    void saveAlertsToFile(const std::string& filename) const;
    
    // Performance analysis
    double getAverageFiringRate() const;
    double getCurrentFiringRate() const;
    uint64_t getTotalSimulationSteps() const { return total_simulation_steps; }
    uint64_t getTotalSpikes() const { return total_spikes; }
    
    // Reset
    void reset();
    void resetProfileData();
};

/**
 * Performance profiler for detailed function timing
 */
class PerformanceProfiler {
private:
    struct ProfileScope {
        uint64_t start_time_ns = 0;
        std::string function_name;
        std::string file_name;
        uint32_t line_number = 0;
        uint32_t thread_id = 0;
    };
    
    std::vector<ProfileScope> active_scopes;
    mutable std::mutex scopes_mutex;
    PerformanceMonitor* monitor;

public:
    PerformanceProfiler(PerformanceMonitor* monitor = nullptr);
    ~PerformanceProfiler();
    
    // Manual profiling
    void start(const std::string& function_name, const std::string& file_name = "", uint32_t line_number = 0);
    void stop(const std::string& function_name);
    
    // RAII-style profiling
    class Scope {
    private:
        PerformanceProfiler* profiler;
        std::string function_name;
        std::string file_name;
        uint32_t line_number;
    public:
        Scope(PerformanceProfiler* profiler, const std::string& function_name, 
              const std::string& file_name = "", uint32_t line_number = 0);
        ~Scope();
    };
    
    Scope makeScope(const std::string& function_name, const std::string& file_name = "", uint32_t line_number = 0);
};

/**
 * Performance metrics calculator and analyzer
 */
class PerformanceAnalyzer {
public:
    static double calculateAverageFiringRate(const std::vector<PerformanceMetrics>& metrics);
    static double calculateCpuUsage(const std::vector<PerformanceMetrics>& metrics);
    static double calculateMemoryUsage(const std::vector<PerformanceMetrics>& metrics);
    static std::vector<double> calculateMovingAverage(const std::vector<double>& data, uint32_t window_size);
    static uint64_t findMaxValue(const std::vector<PerformanceMetrics>& metrics, 
                                double (PerformanceMetrics::* getter)() const);
    static uint64_t findMinValue(const std::vector<PerformanceMetrics>& metrics,
                                double (PerformanceMetrics::* getter)() const);
    static void detectAnomalies(const std::vector<PerformanceMetrics>& metrics,
                               std::vector<PerformanceAlert>& alerts);
};

} // namespace nlm