#ifndef NLM_PERFORMANCE_PROFILER_HPP
#define NLM_PERFORMANCE_PROFILER_HPP

/**
 * Performance Profiler for NLM Neural Network Simulation
 * 
 * Comprehensive profiling and benchmarking infrastructure for the NLM system.
 * Provides real-time monitoring, detailed component profiling, and benchmarking
 * capabilities for neurons, synapses, plasticity rules, and memory systems.
 */

#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <functional>
#include <sstream>

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"

namespace nlm {

/**
 * Performance metrics data structure
 */
struct ProfileMetrics {
    // Timing metrics
    double timestamp;
    double cpu_time;           // CPU time in seconds
    double wall_time;          // Wall clock time in seconds
    double real_time;          // Real time (including idle)
    
    // Memory metrics
    size_t memory_used;        // Memory used in bytes
    size_t memory_peak;        // Peak memory usage in bytes
    size_t memory_allocated;   // Total memory allocated
    
    // Component-specific metrics
    double neuron_activity;    // Average neuron activity level
    double spike_rate;         // Average spikes per second
    double synapse_weight_change; // Average synaptic weight changes
    
    // System metrics
    double cpu_utilization;    // CPU utilization percentage
    double memory_utilization; // Memory utilization percentage
    
    // Event counts
    size_t neuron_creation_events;
    size_t synapse_creation_events;
    size_t plasticity_events;
    size_t memory_access_events;
    
    // Error tracking
    size_t error_count;
    std::string last_error;
};

/**
 * Detailed component profiling data
 */
struct ComponentProfile {
    std::string name;
    std::string type; // "neuron", "synapse", "plasticity", "memory", "simulation"
    
    // Timing data
    std::vector<double> operation_times;
    std::vector<double> processing_times;
    double total_time;
    double average_time;
    double min_time;
    double max_time;
    double std_deviation;
    
    // Performance metrics
    std::atomic<size_t> operations_count{0};
    std::atomic<size_t> successful_operations{0};
    std::atomic<size_t> failed_operations{0};
    
    // Resource usage
    double average_cpu_usage;
    double average_memory_usage;
    
    // Component-specific data
    std::unordered_map<std::string, double> custom_metrics;
    
    ComponentProfile(const std::string& name, const std::string& type)
        : name(name), type(type), total_time(0.0), average_time(0.0),
          min_time(0.0), max_time(0.0), std_deviation(0.0),
          average_cpu_usage(0.0), average_memory_usage(0.0) {}
};

/**
 * Benchmark results structure
 */
struct BenchmarkResult {
    std::string benchmark_name;
    std::string test_type;
    std::string configuration;
    
    // Performance metrics
    double total_time;          // Total benchmark duration
    double throughput;          // Operations per second
    size_t total_operations;    // Total number of operations
    
    // Component performance
    std::unordered_map<std::string, double> component_performance;
    
    // Resource usage
    size_t memory_peak;         // Peak memory usage
    double cpu_time;            // CPU time used
    
    // Statistical data
    std::vector<double> measurements;
    double mean;                // Mean performance
    double median;              // Median performance
    double std_deviation;       // Standard deviation
    double min_value;           // Minimum value
    double max_value;           // Maximum value
    
    // Comparison data
    std::string baseline_name;  // Baseline benchmark for comparison
    double improvement_factor;  // Improvement factor over baseline
    
    BenchmarkResult(const std::string& name, const std::string& test_type)
        : benchmark_name(name), test_type(test_type), total_time(0.0),
          throughput(0.0), total_operations(0), memory_peak(0), cpu_time(0.0),
          mean(0.0), median(0.0), std_deviation(0.0), min_value(0.0),
          max_value(0.0), improvement_factor(0.0) {}
};

/**
 * Real-time performance monitoring system
 */
class RealTimeMonitor {
private:
    std::mutex monitor_mutex;
    std::atomic<bool> running{false};
    std::thread monitor_thread;
    
    // Monitoring data
    std::deque<ProfileMetrics> metrics_history;
    std::atomic<size_t> max_history_size{1000};
    
    // System metrics
    std::atomic<double> current_cpu_usage{0.0};
    std::atomic<double> current_memory_usage{0.0};
    std::atomic<size_t> current_memory_used{0};
    
    // Component metrics
    std::unordered_map<std::string, ComponentProfile> component_profiles;
    
    // Callbacks
    std::function<void(const ProfileMetrics&)> metrics_callback;
    std::function<void(const std::string&, double)> progress_callback;
    
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Control
    void start();
    void stop();
    bool isRunning() const { return running; }
    
    // Monitoring setup
    void addComponentProfile(const std::string& name, const std::string& type);
    void updateComponentMetrics(const std::string& name, const ProfileMetrics& metrics);
    
    // Callbacks
    void setMetricsCallback(std::function<void(const ProfileMetrics&)> callback);
    void setProgressCallback(std::function<void(const std::string&, double)> callback);
    
    // Data access
    ProfileMetrics getCurrentMetrics() const;
    std::vector<ProfileMetrics> getMetricsHistory(size_t count = 100) const;
    ComponentProfile getComponentProfile(const std::string& name) const;
    
    // Reporting
    void generatePerformanceReport(const std::string& output_file) const;
};

/**
 * Advanced performance profiler for NLM
 */
class PerformanceProfiler {
private:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<SimulationClock> clock;
    std::shared_ptr<Brain> brain;
    
    // Monitoring instances
    std::unique_ptr<RealTimeMonitor> monitor;
    
    // Profiling state
    std::atomic<bool> profiling_enabled{false};
    std::atomic<bool> benchmarking_active{false};
    
    // Profiling data
    std::unordered_map<std::string, ComponentProfile> active_profiles;
    std::deque<ProfileMetrics> profiling_history;
    std::vector<BenchmarkResult> benchmark_results;
    
    // Configuration
    std::string output_directory;
    double sampling_interval; // seconds
    bool save_detailed_data;
    bool enable_real_time_monitoring;
    
    // Threading
    std::mutex profiler_mutex;
    std::thread monitoring_thread;
    
    // File writing
    std::ofstream metrics_file;
    std::ofstream benchmarks_file;
    
public:
    PerformanceProfiler(
        std::shared_ptr<Logger> logger,
        std::shared_ptr<SimulationClock> clock,
        std::shared_ptr<Brain> brain,
        const std::string& output_dir = "performance_data"
    );
    
    ~PerformanceProfiler();
    
    // Configuration
    void configure(
        double sampling_interval = 0.1,
        bool save_detailed = true,
        bool enable_monitoring = true
    );
    
    // Control
    void startProfiling();
    void stopProfiling();
    void reset();
    
    // Component profiling
    void startComponentProfile(const std::string& name, const std::string& type);
    void endComponentProfile(const std::string& name, const ProfileMetrics& metrics);
    
    // System profiling
    void profileSystemState();
    void profileNeuralActivity();
    void profileMemoryUsage();
    void profileCPUUsage();
    
    // Brain component profiling
    void profileNeuronOperations();
    void profileSynapseOperations();
    void profilePlasticityOperations();
    void profileMemorySystems();
    
    // Benchmarking
    void runNeuralNetworkSizeBenchmark(const std::vector<size_t>& neuron_counts);
    void runSimulationSpeedBenchmark(const std::vector<double>& durations);
    void runPlasticityRuleBenchmark(const std::vector<std::string>& rule_names);
    void runMemoryEfficiencyBenchmark(const std::vector<size_t>& memory_sizes);
    
    // Statistical analysis
    BenchmarkResult calculateStatistics(const std::vector<BenchmarkResult>& results);
    void compareBenchmarks(const std::string& baseline_name, const std::string& test_name);
    
    // Data access
    std::vector<BenchmarkResult> getBenchmarkResults() const { return benchmark_results; }
    std::vector<ProfileMetrics> getProfilingHistory() const { return profiling_history; }
    ProfileMetrics getCurrentMetrics() const;
    
    // Reporting
    void generatePerformanceReport(const std::string& output_file = "performance_report.html");
    void generateBenchmarkReport(const std::string& output_file = "benchmark_report.html");
    void generateDetailedReport(const std::string& output_file = "detailed_report.html");
    
    // File operations
    void saveDataToFiles();
    void loadDataFromFiles();
};

} // namespace nlm

#endif // NLM_PERFORMANCE_PROFILER_HPP