// Enhanced NLM Profiler System
// File: src/core/Profiling/Profiler.hpp

#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <mutex>
#include <functional>
#include <unordered_set>
#include <stack>
#include <set>

namespace nlm {

// Profiler types and configuration
enum class ProfilerType {
    CPU,
    MEMORY,
    THREAD,
    SYSTEM,
    FUNCTION,
    ALLOCATION,
    GPU,
    NETWORK,
    DISK
};

enum class SampleMode {
    PERIODIC,
    ON_DEMAND,
    TRIGGERED,
    CONTINUOUS
};

enum class AnalysisMode {
    SUMMARY,
    DETAILED,
    TIMELINE,
    COMPARATIVE,
    ROOT_CAUSE
};

// Profiler configuration
struct ProfilerConfig {
    ProfilerType type;
    SampleMode sample_mode;
    AnalysisMode analysis_mode;
    double sampling_interval = 0.1;  // seconds
    size_t max_samples = 100000;
    bool enable_detailed_timestamps = true;
    bool enable_thread_profiling = true;
    bool enable_system_profiling = true;
    bool enable_memory_profiling = true;
    bool enable_cpu_profiling = true;
    bool enable_callstack_profiling = true;
    bool enable_allocation_profiling = true;
    bool enable_cpu_frequency_profiling = false;
    bool enable_memory_bandwidth_profiling = false;
    bool enable_power_profiling = false;
    double cpu_threshold = 80.0;  // CPU usage threshold percentage
    double memory_threshold = 80.0;  // Memory usage threshold percentage
    size_t thread_count_threshold = 100;  // Thread count threshold
    std::string output_directory = ".";
    std::string output_filename = "profiler_output";
    std::string output_format = "json";
    bool compress_output = false;
    bool enable_real_time_analysis = false;
    size_t real_time_update_interval = 1000;  // milliseconds
    std::vector<std::string> include_patterns;  // Regex patterns to include
    std::vector<std::string> exclude_patterns;  // Regex patterns to exclude
    std::map<std::string, std::string> custom_settings;
};

// Sample data structures
struct CPUSample {
    std::chrono::steady_clock::time_point timestamp;
    double cpu_usage;                    // CPU usage percentage
    double user_time;                     // User CPU time
    double system_time;                   // System CPU time
    double frequency;                     // CPU frequency (Hz)
    uint64_t cache_misses;                // Cache misses
    uint64_t cache_hits;                  // Cache hits
    uint64_t context_switches;            // Context switches
    uint64_t total_instructions;          // Total instructions
    uint64_t total_cycles;                // Total cycles
    double instructions_per_cycle;        // IPC
    std::thread::id thread_id;            // Thread ID
    std::string process_name;             // Process name
    pid_t process_id;                     // Process ID
};

struct MemorySample {
    std::chrono::steady_clock::time_point timestamp;
    uint64_t total_memory;                // Total virtual memory (bytes)
    uint64_t used_memory;                 // Used memory (bytes)
    uint64_t free_memory;                 // Free memory (bytes)
    uint64_t allocated_objects;           // Number of allocated objects
    uint64_t deallocated_objects;         // Number of deallocated objects
    uint64_t peak_memory;                 // Peak memory usage (bytes)
    uint64_t current_allocation_rate;     // Current allocation rate (objects/sec)
    uint64_t current_deallocation_rate;   // Current deallocation rate (objects/sec)
    double memory_fragmentation;         // Memory fragmentation ratio
    std::thread::id thread_id;            // Thread ID
    std::string allocation_pattern;      // Allocation pattern description
};

struct ThreadSample {
    std::chrono::steady_clock::time_point timestamp;
    std::thread::id thread_id;            // Thread ID
    std::string thread_name;              // Thread name
    std::chrono::duration<double> cpu_time;  // CPU time used
    std::chrono::duration<double> wall_time; // Wall time
    double utilization;                   // Thread utilization percentage
    size_t stack_size;                    // Stack size
    std::thread::id parent_thread;        // Parent thread ID
    std::string thread_function;          // Function that created the thread
    std::chrono::steady_clock::time_point creation_time; // Thread creation time
};

struct FunctionSample {
    std::chrono::steady_clock::time_point timestamp;
    std::thread::id thread_id;            // Thread ID
    std::string function_name;            // Function name
    std::string file_path;                // Source file path
    size_t line_number;                   // Line number
    std::string module_name;              // Module name
    std::chrono::duration<double> duration;  // Function duration
    std::chrono::duration<double> cpu_duration;  // CPU duration
    std::chrono::duration<double> wall_duration; // Wall duration
    size_t call_count;                    // Number of calls
    size_t recursive_depth;               // Recursion depth
    std::vector<std::string> call_stack;  // Call stack
    std::map<std::string, std::string> parameters; // Function parameters
    std::map<std::string, std::string> return_values; // Return values
    bool is_recursive;                    // Whether function is recursive
    double cpu_usage;                     // CPU usage percentage
    size_t memory_usage;                  // Memory usage
};

struct AllocationSample {
    std::chrono::steady_clock::time_point timestamp;
    std::thread::id thread_id;            // Thread ID
    void* allocated_address;              // Allocated address
    size_t allocation_size;               // Allocation size (bytes)
    std::string allocator_type;          // Allocator type
    std::string allocation_site;          // Allocation site (file:line)
    std::string allocation_pattern;      // Allocation pattern
    size_t stack_depth;                   // Stack depth
    std::vector<std::string> call_stack;  // Call stack
    std::chrono::steady_clock::time_point allocation_time; // Time of allocation
    std::chrono::steady_clock::time_point deallocation_time; // Time of deallocation
    bool is_allocated;                    // Whether still allocated
    double time_to_live;                  // Time to live (if deallocated)
    std::string object_type;              // Type of allocated object
    size_t object_count;                  // Number of objects allocated
};

struct SystemSample {
    std::chrono::steady_clock::time_point timestamp;
    double system_load;                   // System load average
    double cpu_temperature;               // CPU temperature (Celsius)
    double disk_usage;                    // Disk usage percentage
    double network_usage;                 // Network usage percentage
    uint64_t network_bytes_sent;           // Network bytes sent
    uint64_t network_bytes_received;       // Network bytes received
    double battery_level;                 // Battery level (if laptop)
    std::string power_state;              // Power state
    std::thread::id thread_id;            // Thread ID
};

struct ProfiledFunction {
    std::string name;                     // Function name
    std::string file_path;                // Source file path
    size_t line_number;                   // Line number
    std::string module_name;              // Module name
    size_t call_count;                    // Number of calls
    std::chrono::duration<double> total_time;  // Total execution time
    std::chrono::duration<double> cpu_time;    // Total CPU time
    std::chrono::duration<double> wall_time;   // Total wall time
    std::chrono::duration<double> avg_time;    // Average time per call
    std::chrono::duration<double> min_time;    // Minimum time
    std::chrono::duration<double> max_time;    // Maximum time
    double cpu_usage;                     // Average CPU usage
    size_t memory_usage;                  // Average memory usage
    double recursion_count;               // Recursion count
    std::vector<std::chrono::duration<double>> time_samples;  // Individual time samples
    std::unordered_map<size_t, size_t> call_counts_by_thread;  // Calls by thread ID
    std::vector<std::string> top_callers;  // Top calling functions
    std::vector<std::string> top_callees; // Top callee functions
};

struct ProfileAnalysis {
    std::string profile_name;             // Profile name
    std::chrono::duration<double> total_duration;  // Total profile duration
    size_t total_samples;                 // Total number of samples
    double cpu_usage;                     // Average CPU usage
    double memory_usage;                  // Average memory usage
    size_t total_allocations;             // Total allocations
    size_t total_deallocations;           // Total deallocations
    size_t memory_leak_count;             // Number of memory leaks detected
    std::vector<std::string> hot_functions; // Hot functions (sorted by time)
    std::vector<std::string> cold_functions; // Cold functions (sorted by time)
    std::vector<std::string> bottleneck_functions; // Bottleneck functions
    std::map<std::string, double> function_time_distribution; // Time distribution by function
    std::map<std::string, double> function_call_distribution; // Call distribution by function
    double efficiency_score;              // Overall efficiency score
    double throughput;                    // Operations per second
    double cache_efficiency;              // Cache efficiency
    std::string recommendations;           // Performance recommendations
    std::vector<std::string> optimization_opportunities; // Optimization opportunities
};

class Profiler {
public:
    Profiler(const ProfilerConfig& config);
    ~Profiler();

    // Configuration
    void configure(const ProfilerConfig& config);
    ProfilerConfig get_config() const { return config_; }

    // Control
    void start();
    void stop();
    bool is_running() const { return running_.load(); }

    // Sampling
    void take_sample();
    void sample_function(const std::string& function_name, 
                        const std::string& file_path,
                        size_t line_number,
                        std::chrono::duration<double> duration,
                        std::chrono::duration<double> cpu_duration = std::chrono::duration<double>(0),
                        const std::map<std::string, std::string>& parameters = {});
    void sample_allocation(void* address, size_t size,
                          const std::string& allocator_type,
                          const std::string& allocation_site,
                          const std::vector<std::string>& call_stack);
    void sample_deallocation(void* address, size_t size);
    void sample_thread(std::thread::id thread_id,
                      const std::string& thread_name,
                      std::chrono::duration<double> cpu_time,
                      std::chrono::duration<double> wall_time,
                      double utilization);
    void sample_system();

    // Data access
    std::vector<CPUSample> get_cpu_samples() const { return cpu_samples_; }
    std::vector<MemorySample> get_memory_samples() const { return memory_samples_; }
    std::vector<ThreadSample> get_thread_samples() const { return thread_samples_; }
    std::vector<FunctionSample> get_function_samples() const { return function_samples_; }
    std::vector<AllocationSample> get_allocation_samples() const { return allocation_samples_; }
    std::vector<SystemSample> get_system_samples() const { return system_samples_; }

    // Analysis
    ProfiledFunction get_function_analysis(const std::string& function_name) const;
    std::vector<ProfiledFunction> get_function_analysis(const std::string& pattern,
                                                         AnalysisMode mode = AnalysisMode::SUMMARY) const;
    ProfileAnalysis analyze_profile() const;
    std::vector<std::string> get_hot_functions(size_t count = 10) const;
    std::vector<std::string> get_cold_functions(size_t count = 10) const;
    std::vector<std::string> get_bottleneck_functions(size_t count = 10) const;
    std::vector<std::string> get_recommendation() const;

    // Filtering and querying
    std::vector<CPUSample> query_cpu_samples(double time_window) const;
    std::vector<MemorySample> query_memory_samples(double time_window) const;
    std::vector<ThreadSample> query_thread_samples(double time_window) const;
    std::vector<FunctionSample> query_function_samples(double time_window) const;
    std::vector<AllocationSample> query_allocation_samples(double time_window) const;

    // Statistics
    size_t get_sample_count() const;
    double get_total_duration() const;
    std::chrono::steady_clock::time_point get_start_time() const { return start_time_; }
    std::chrono::steady_clock::time_point get_end_time() const { return end_time_; }

    // Export
    void export_data(const std::string& filename, const std::string& format = "json") const;
    void export_samples(const std::string& filename) const;
    void export_analysis(const std::string& filename) const;

    // Cleanup
    void cleanup_old_samples();
    void clear_samples();
    void reset();

    // Real-time monitoring
    void enable_real_time_monitoring(bool enable);
    bool is_real_time_monitoring_enabled() const { return real_time_monitoring_; }

    // Callbacks
    void set_sample_callback(std::function<void(const CPUSample&)> callback);
    void set_alert_callback(std::function<void(const ProfilerConfig&)> callback);

    // Thread safety
    void set_thread_safe(bool enabled) { thread_safe_ = enabled; }

protected:
    void add_cpu_sample(const CPUSample& sample);
    void add_memory_sample(const MemorySample& sample);
    void add_thread_sample(const ThreadSample& sample);
    void add_function_sample(const FunctionSample& sample);
    void add_allocation_sample(const AllocationSample& sample);
    void add_system_sample(const SystemSample& sample);
    void filter_samples();
    bool should_sample_function(const std::string& function_name) const;
    bool should_sample_thread(std::thread::id thread_id) const;
    void update_real_time_data();
    void process_sample_callbacks(const CPUSample& sample);

    ProfilerConfig config_;
    std::atomic<bool> running_{false};
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point end_time_;
    mutable std::mutex samples_mutex_;
    std::vector<CPUSample> cpu_samples_;
    std::vector<MemorySample> memory_samples_;
    std::vector<ThreadSample> thread_samples_;
    std::vector<FunctionSample> function_samples_;
    std::vector<AllocationSample> allocation_samples_;
    std::vector<SystemSample> system_samples_;
    std::atomic<bool> real_time_monitoring_{false};
    std::thread real_time_thread_;
    std::function<void(const CPUSample&)> sample_callback_;
    std::function<void(const ProfilerConfig&)> alert_callback_;
    bool thread_safe_;
    std::atomic<size_t> current_sample_index_{0};
    std::chrono::steady_clock::time_point last_cleanup_;
    double cleanup_interval_ = 300.0;  // 5 minutes
    std::atomic<size_t> total_samples_taken_{0};
    std::chrono::steady_clock::time_point last_update_;
    std::unordered_set<std::string> sampled_functions_;  // Track sampled functions
    std::unordered_set<std::thread::id> sampled_threads_;  // Track sampled threads
};

} // namespace nlm
