// Enhanced NLM Performance Monitoring System
// File: src/core/Performance/PerformanceMonitor.hpp

#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <algorithm>
#include <limits>

namespace nlm {

// Performance monitoring configuration
struct PerformanceMonitorConfig {
    bool enable_cpu_monitoring = true;
    bool enable_memory_monitoring = true;
    bool enable_thread_monitoring = true;
    bool enable_network_monitoring = false;
    bool enable_disk_monitoring = false;
    bool real_time_updates = true;
    double sampling_interval = 0.1;  // seconds
    size_t max_history_size = 10000;
    std::string output_format = "json";
    std::string output_file = "";
    bool compress_output = false;
    bool track_system_resources = true;
    bool track_application_metrics = true;
    size_t cpu_sampling_interval = 100;  // milliseconds
    size_t memory_sampling_interval = 1000;  // milliseconds
};

// Performance metrics
struct CPUMetrics {
    double user_time = 0.0;      // User CPU time (seconds)
    double system_time = 0.0;    // System CPU time (seconds)
    double cpu_percent = 0.0;    // CPU utilization percentage
    double frequency = 0.0;      // CPU frequency (Hz)
    uint64_t cache_misses = 0;   // Cache misses
    uint64_t cache_hits = 0;     // Cache hits
    uint64_t context_switches = 0;  // Context switches
    double instructions_per_cycle = 0.0;  // IPC
    uint64_t total_instructions = 0;     // Total instructions
    uint64_t total_cycles = 0;           // Total cycles
    std::chrono::steady_clock::time_point last_update;
};

struct MemoryMetrics {
    uint64_t total_memory = 0;           // Total virtual memory (bytes)
    uint64_t used_memory = 0;            // Used memory (bytes)
    uint64_t free_memory = 0;            // Free memory (bytes)
    uint64_t allocated_objects = 0;      // Number of allocated objects
    uint64_t deallocated_objects = 0;    // Number of deallocated objects
    uint64_t peak_memory = 0;            // Peak memory usage (bytes)
    uint64_t current_allocation_rate = 0;  // Current allocation rate (objects/sec)
    uint64_t current_deallocation_rate = 0;  // Current deallocation rate (objects/sec)
    double memory_fragmentation = 0.0;   // Memory fragmentation ratio
    std::vector<uint64_t> allocation_sizes;  // Recent allocation sizes for analysis
    std::chrono::steady_clock::time_point last_update;
};

struct ThreadMetrics {
    size_t active_threads = 0;           // Number of active threads
    size_t total_threads = 0;            // Total threads created
    size_t peak_threads = 0;             // Peak threads active simultaneously
    double average_thread_utilization = 0.0;  // Average thread utilization
    std::map<std::thread::id, double> thread_utilization;  // Per-thread utilization
    std::chrono::steady_clock::time_point last_update;
};

struct NetworkMetrics {
    uint64_t bytes_sent = 0;             // Bytes sent
    uint64_t bytes_received = 0;         // Bytes received
    uint64_t packets_sent = 0;           // Packets sent
    uint64_t packets_received = 0;       // Packets received
    double send_rate = 0.0;              // Send rate (bytes/sec)
    double receive_rate = 0.0;           // Receive rate (bytes/sec)
    std::chrono::steady_clock::time_point last_update;
};

struct DiskMetrics {
    uint64_t reads = 0;                  // Number of reads
    uint64_t writes = 0;                 // Number of writes
    uint64_t read_bytes = 0;             // Bytes read
    uint64_t write_bytes = 0;            // Bytes written
    double read_rate = 0.0;              // Read rate (bytes/sec)
    double write_rate = 0.0;             // Write rate (bytes/sec)
    double read_latency = 0.0;           // Average read latency (seconds)
    double write_latency = 0.0;          // Average write latency (seconds)
    std::chrono::steady_clock::time_point last_update;
};

struct ApplicationMetrics {
    double cpu_time = 0.0;               // Total CPU time (seconds)
    double wall_time = 0.0;              // Total wall clock time (seconds)
    size_t memory_usage = 0;             // Current memory usage (bytes)
    size_t peak_memory = 0;              // Peak memory usage (bytes)
    uint64_t operations_completed = 0;   // Total operations completed
    uint64_t operations_failed = 0;      // Total operations failed
    double operations_per_second = 0.0;   // Operations per second
    double memory_efficiency = 0.0;       // Memory efficiency ratio
    double cache_efficiency = 0.0;       // Cache efficiency ratio
    std::chrono::steady_clock::time_point last_update;
};

struct PerformanceSnapshot {
    std::chrono::steady_clock::time_point timestamp;
    CPUMetrics cpu;
    MemoryMetrics memory;
    ThreadMetrics threads;
    NetworkMetrics network;
    DiskMetrics disk;
    ApplicationMetrics application;
    double system_load = 0.0;            // System load average
    double temperature = 0.0;            // CPU temperature (if available)
    std::string process_name;            // Process name
    pid_t process_id = 0;                // Process ID
};

class PerformanceMonitor {
public:
    PerformanceMonitor(const PerformanceMonitorConfig& config = PerformanceMonitorConfig());
    ~PerformanceMonitor();

    // Configuration
    void configure(const PerformanceMonitorConfig& config);
    PerformanceMonitorConfig get_config() const { return config_; }

    // Monitoring control
    void start();
    void stop();
    bool is_running() const { return running_.load(); }

    // Data collection
    void update();
    PerformanceSnapshot get_current_snapshot() const;
    std::vector<PerformanceSnapshot> get_history() const { return history_; }
    PerformanceSnapshot get_latest_snapshot() const { return current_snapshot_; }

    // Metrics retrieval
    CPUMetrics get_cpu_metrics() const { return current_snapshot_.cpu; }
    MemoryMetrics get_memory_metrics() const { return current_snapshot_.memory; }
    ThreadMetrics get_thread_metrics() const { return current_snapshot_.threads; }
    NetworkMetrics get_network_metrics() const { return current_snapshot_.network; }
    DiskMetrics get_disk_metrics() const { return current_snapshot_.disk; }
    ApplicationMetrics get_application_metrics() const { return current_snapshot_.application; }

    // Historical data
    std::vector<PerformanceSnapshot> get_history(double time_window) const;
    std::vector<PerformanceSnapshot> get_history(size_t max_count) const;

    // Analysis
    double calculate_cpu_utilization(double time_window = 60.0) const;
    double calculate_memory_utilization(double time_window = 60.0) const;
    double calculate_thread_utilization(double time_window = 60.0) const;
    std::vector<double> get_cpu_history(double time_window = 60.0) const;
    std::vector<double> get_memory_history(double time_window = 60.0) const;
    std::vector<double> get_thread_history(double time_window = 60.0) const;

    // Alerts and thresholds
    void set_cpu_threshold(double threshold) { cpu_threshold_ = threshold; }
    void set_memory_threshold(double threshold) { memory_threshold_ = threshold; }
    void set_thread_threshold(size_t threshold) { thread_threshold_ = threshold; }
    void set_performance_alert_callback(std::function<void(const PerformanceSnapshot&)> callback)
    {
        alert_callback_ = std::move(callback);
    }

    // Export
    void export_data(const std::string& filename, const std::string& format = "json") const;
    void export_metrics(const std::string& filename) const;

    // Statistics
    size_t get_sample_count() const { return history_.size(); }
    double get_uptime() const;
    std::chrono::steady_clock::time_point get_start_time() const { return start_time_; }

    // Resource management
    void cleanup_old_data();
    void clear_history() { history_.clear(); }

private:
    void monitor_cpu();
    void monitor_memory();
    void monitor_threads();
    void monitor_network();
    void monitor_disk();
    void monitor_application();
    void collect_system_data();
    PerformanceSnapshot create_snapshot() const;
    void add_snapshot_to_history(const PerformanceSnapshot& snapshot);
    void check_alerts(const PerformanceSnapshot& snapshot);
    void update_performance_counters();

    PerformanceMonitorConfig config_;
    std::atomic<bool> running_{false};
    std::chrono::steady_clock::time_point start_time_;
    mutable std::mutex data_mutex_;
    std::vector<PerformanceSnapshot> history_;
    PerformanceSnapshot current_snapshot_;
    double cpu_threshold_ = 80.0;       // CPU threshold percentage
    double memory_threshold_ = 80.0;    // Memory threshold percentage
    size_t thread_threshold_ = 100;     // Thread threshold
    std::function<void(const PerformanceSnapshot&)> alert_callback_;
    std::chrono::steady_clock::time_point last_cleanup_;
    size_t max_history_size_ = 10000;
    double cleanup_interval_ = 300.0;   // 5 minutes
    std::atomic<uint64_t> total_samples_{0};
    std::chrono::steady_clock::time_point last_update_;
    std::thread monitoring_thread_;
};

} // namespace nlm
