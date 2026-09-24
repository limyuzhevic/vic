// Performance Analysis Framework
// Provides comprehensive profiling, benchmarking, and optimization tools
// for NLM neural network simulation performance analysis

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <memory>
#include <atomic>
#include <filesystem>

namespace nlm {
namespace performance {

// Define types for performance metrics
using Timestamp = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;
using MetricValue = double;
using MetricMap = std::unordered_map<std::string, MetricValue>;
using ProfileData = std::unordered_map<std::string, MetricMap>;

class PerformanceProfiler {
public:
    struct ProfilerConfig {
        bool enable_cpu_profiling = true;
        bool enable_memory_profiling = true;
        bool enable_timeline = true;
        bool enable_call_stacks = true;
        int sampling_interval_ms = 10;
        std::string output_dir = "./profiles";
        bool generate_html = true;
        bool generate_plots = true;
    };

    PerformanceProfiler(const ProfilerConfig& config = {});
    ~PerformanceProfiler();

    // Start/stop profiling
    void start(const std::string& phase_name);
    void stop(const std::string& phase_name);
    void checkpoint(const std::string& name);

    // Measurement utilities
    void measure_cpu(const std::string& operation);
    void measure_memory(const std::string& operation);
    void measure_io(const std::string& operation);

    // Thread-specific profiling
    void set_thread_id(size_t id);
    void record_thread_activity(const std::string& operation, Duration duration);

    // Data collection
    ProfileData get_profile_data() const;
    void export_profile(const std::string& filename);
    void generate_report(const std::string& filename);

    // Configuration
    void set_config(const ProfilerConfig& config);

private:
    struct PhaseData {
        Timestamp start_time;
        Timestamp end_time;
        Duration total_duration;
        MetricMap metrics;
        std::vector<std::pair<Timestamp, std::string>> timeline;
        std::vector<ProfileData> nested_profiles;
        size_t thread_id;
    };

    std::unordered_map<std::string, PhaseData> active_phases_;
    std::unordered_map<size_t, std::string> thread_names_;
    ProfilerConfig config_;
    std::atomic<size_t> next_thread_id_{0};
};

class BenchmarkSuite {
public:
    struct BenchmarkConfig {
        std::string name;
        int iterations = 5;
        int warmup_iterations = 2;
        bool measure_memory = true;
        bool measure_cpu = true;
        bool measure_io = true;
        std::string output_format = "json";
        std::string report_template = "standard";
    };

    BenchmarkSuite(const BenchmarkConfig& config = {});
    ~BenchmarkSuite();

    // Benchmark registration
    template<typename F>
    void register_benchmark(const std::string& name, F&& benchmark_func);

    // Benchmark execution
    void run_all();
    void run_benchmark(const std::string& name);
    void run_benchmark(const std::string& name, std::function<void()> benchmark_func);

    // Configuration
    void set_config(const BenchmarkConfig& config);
    void add_metric_processor(std::function<void(const std::string&, const MetricMap&)> processor);

    // Results
    const ProfileData& get_results(const std::string& name) const;
    void export_results(const std::string& filename);
    void generate_report(const std::string& filename);

private:
    struct BenchmarkResult {
        std::string name;
        Duration total_time;
        MetricMap metrics;
        std::vector<Duration> iteration_times;
        std::vector<MetricMap> iteration_metrics;
    };

    std::unordered_map<std::string, std::function<void()>> benchmarks_;
    std::unordered_map<std::string, BenchmarkResult> results_;
    BenchmarkConfig config_;
    std::vector<std::function<void(const std::string&, const MetricMap&)>> metric_processors_;
};

class MemoryOptimizer {
public:
    struct MemoryProfile {
        size_t total_memory;
        size_t allocated_memory;
        size_t peak_memory;
        size_t allocation_count;
        std::unordered_map<std::string, size_t> allocation_breakdown;
        std::vector<std::pair<Timestamp, size_t>> memory_timeline;
    };

    MemoryOptimizer();
    ~MemoryOptimizer();

    // Memory tracking
    void track_allocation(const std::string& source, size_t bytes);
    void track_deallocation(const std::string& source, size_t bytes);
    void begin_allocation_block(const std::string& context);
    void end_allocation_block(const std::string& context);

    // Optimization suggestions
    std::vector<std::string> get_optimization_suggestions() const;
    std::string generate_memory_report() const;

    // Pool management
    template<typename T>
    void optimize_pool_access(const std::vector<T*>& objects);

    // Results
    MemoryProfile get_profile() const;
    void export_profile(const std::string& filename);

private:
    struct AllocationInfo {
        Timestamp timestamp;
        std::string source_file;
        int source_line;
        size_t bytes;
        std::string context;
        size_t call_stack_hash;
    };

    std::vector<AllocationInfo> allocations_;
    size_t total_allocated_;
    size_t peak_allocated_;
    std::atomic<size_t> current_allocated_;
    mutable std::mutex mutex_;
};

class ThreadPerformanceAnalyzer {
public:
    struct ThreadStats {
        size_t thread_id;
        std::string thread_name;
        Duration total_time;
        size_t work_items_completed;
        size_t work_items_processed;
        Duration average_work_time;
        Duration min_work_time;
        Duration max_work_time;
        double cpu_usage;
        size_t memory_usage;
        double contention_level;
        std::vector<Duration> work_times;
    };

    ThreadPerformanceAnalyzer(size_t max_threads = 32);
    ~ThreadPerformanceAnalyzer();

    // Thread tracking
    void register_thread(const std::string& name);
    void start_work(size_t thread_id);
    void end_work(size_t thread_id, const std::string& operation);
    void record_contention(size_t thread_id, double level);

    // Analysis
    std::vector<ThreadStats> analyze_threads() const;
    std::string generate_thread_report() const;
    std::string get_bottleneck_analysis() const;

    // Configuration
    void set_analysis_window(Duration window);
    void set_min_samples(size_t min_samples);

private:
    struct ThreadWorkRecord {
        Timestamp start_time;
        Timestamp end_time;
        std::string operation;
        Duration duration;
        size_t thread_id;
    };

    std::vector<ThreadWorkRecord> work_records_;
    std::unordered_map<size_t, std::string> thread_names_;
    mutable std::mutex mutex_;
    Duration analysis_window_;
    size_t min_samples_;
};

class PerformanceVisualization {
public:
    PerformanceVisualization();
    ~PerformanceVisualization();

    // Plot generation
    void generate_performance_heatmap(const ProfileData& data, const std::string& filename);
    void generate_resource_usage_graph(const ProfileData& data, const std::string& filename);
    void generate_simulation_timeline(const ProfileData& data, const std::string& filename);
    void generate_bottleneck_chart(const ProfileData& data, const std::string& filename);

    // Dashboard generation
    void generate_dashboard(const ProfileData& data, const std::string& output_dir);
    void generate_comparison_dashboard(const std::map<std::string, ProfileData>& datasets, const std::string& output_dir);

    // Configuration
    void set_output_format(const std::string& format);
    void set_color_scheme(const std::string& scheme);
    void set_chart_style(const std::string& style);

private:
    std::string output_format_;
    std::string color_scheme_;
    std::string chart_style_;
};

class PerformanceAutomation {
public:
    struct AutomationConfig {
        std::string results_directory = "./performance_results";
        std::string baseline_file = "baseline.json";
        double regression_threshold = 0.05; // 5% regression
        bool auto_optimize = true;
        bool generate_suggestions = true;
        bool notify_on_regressions = false;
        std::string notification_email;
    };

    PerformanceAutomation(const AutomationConfig& config = {});
    ~PerformanceAutomation();

    // Regression testing
    bool detect_regressions(const ProfileData& current, const ProfileData& baseline);
    std::vector<std::string> analyze_regression(const ProfileData& current, const ProfileData& baseline) const;

    // Optimization recommendations
    std::vector<std::string> generate_optimization_recommendations(const ProfileData& data) const;
    bool apply_optimization(const std::string& recommendation);

    // Continuous integration
    void run_ci_performance_check(const std::string& baseline_file);
    bool pass_ci_check(const ProfileData& current, const ProfileData& baseline);

    // Reporting
    void generate_performance_report(const ProfileData& data, const std::string& filename);
    void send_notification(const std::string& message);

    // Configuration
    void set_config(const AutomationConfig& config);

private:
    struct RegressionResult {
        std::string metric_name;
        double baseline_value;
        double current_value;
        double change_percent;
        bool is_regression;
        std::string recommendation;
    };

    AutomationConfig config_;
};

// Convenience functions for easy integration
namespace utils {
    std::string format_duration(Duration duration);
    std::string format_memory_size(size_t bytes);
    std::string get_current_timestamp();
    void create_output_directory(const std::string& path);
    void write_json_file(const std::string& filename, const std::string& json_data);
}

} // namespace performance
} // namespace nlm