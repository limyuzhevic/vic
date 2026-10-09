#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <sstream>

namespace nlm {

// Performance metrics types
struct SystemResourceMetrics {
    double cpu_usage_percent = 0.0;
    double memory_usage_mb = 0.0;
    double memory_percent = 0.0;
    uint64_t disk_io_read = 0;
    uint64_t disk_io_write = 0;
    uint64_t network_io_sent = 0;
    uint64_t network_io_received = 0;
    int64_t process_id = 0;
    std::chrono::steady_clock::time_point timestamp;
};

struct NeuralComputationMetrics {
    double step_time_ms = 0.0;
    double neuron_update_time_ms = 0.0;
    double synaptic_update_time_ms = 0.0;
    double spike_processing_time_ms = 0.0;
    double plasticity_time_ms = 0.0;
    double memory_update_time_ms = 0.0;
    double prediction_time_ms = 0.0;
    double cognition_time_ms = 0.0;
    double neuromodulation_time_ms = 0.0;
    uint32_t neuron_count = 0;
    uint32_t synapse_count = 0;
    uint32_t firing_neuron_count = 0;
    uint32_t total_spikes = 0;
    float excitation_inhibition_ratio = 0.0f;
    float average_firing_rate = 0.0f;
    std::chrono::steady_clock::time_point timestamp;
};

struct MemoryUsageMetrics {
    double working_memory_mb = 0.0;
    double episodic_memory_mb = 0.0;
    double concept_memory_mb = 0.0;
    double prediction_memory_mb = 0.0;
    double total_memory_mb = 0.0;
    uint32_t working_memory_capacity = 0;
    uint32_t episodic_memory_capacity = 0;
    uint32_t concept_memory_capacity = 0;
    double memory_growth_rate_mb_per_step = 0.0;
    uint32_t spike_event_memory_usage = 0;
    std::chrono::steady_clock::time_point timestamp;
};

struct SpikeEventMetrics {
    uint32_t spike_events_this_step = 0;
    uint32_t delayed_spike_events = 0;
    double spike_generation_rate = 0.0;
    double spike_delivery_rate = 0.0;
    std::vector<double> spike_times_per_region;
    std::chrono::steady_clock::time_point timestamp;
};

struct DevelopmentMetrics {
    double stage_transition_time_ms = 0.0;
    double plasticity_update_time_ms = 0.0;
    double structural_plasticity_time_ms = 0.0;
    uint32_t synaptogenesis_events = 0;
    uint32_t pruning_events = 0;
    double developmental_stage = 0.0;
    double developmental_progress = 0.0;
    std::chrono::steady_clock::time_point timestamp;
};

struct NeuromodulationMetrics {
    double dopamine_level = 0.0;
    double serotonin_level = 0.0;
    double norepinephrine_level = 0.0;
    double acetylcholine_level = 0.0;
    double curiosity_level = 0.0;
    double novelty_level = 0.0;
    double prediction_error_level = 0.0;
    double reward_level = 0.0;
    double neuromodulation_processing_time_ms = 0.0;
    std::chrono::steady_clock::time_point timestamp;
};

struct PerformanceMetrics {
    uint64_t simulation_step = 0;
    std::chrono::steady_clock::time_point simulation_start_time;
    SystemResourceMetrics system_resources;
    NeuralComputationMetrics neural_computation;
    MemoryUsageMetrics memory_usage;
    SpikeEventMetrics spike_events;
    DevelopmentMetrics development;
    NeuromodulationMetrics neuromodulation;
    std::map<std::string, double> custom_metrics;
    double wall_clock_time_ms = 0.0;
    double cpu_time_ms = 0.0;
    double memory_time_ms = 0.0;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying and moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) = delete;
    PerformanceMonitor& operator=(PerformanceMonitor&&) = delete;
    
    // Start/stop monitoring
    void start();
    void stop();
    void pause();
    void resume();
    bool isMonitoring() const { return monitoring_.load(); }
    
    // Record performance data
    void recordStepMetrics(const PerformanceMetrics& metrics);
    void recordSystemResource(const SystemResourceMetrics& sys_metrics);
    void recordNeuralComputation(const NeuralComputationMetrics& neural_metrics);
    void recordMemoryUsage(const MemoryUsageMetrics& memory_metrics);
    void recordSpikeEvents(const SpikeEventMetrics& spike_metrics);
    void recordDevelopment(const DevelopmentMetrics& dev_metrics);
    void recordNeuromodulation(const NeuromodulationMetrics& neuromod_metrics);
    
    // Get latest metrics
    PerformanceMetrics getLatestMetrics() const;
    PerformanceMetrics getAverageMetrics(uint64_t steps) const;
    PerformanceMetrics getMaxMetrics(uint64_t steps) const;
    std::vector<PerformanceMetrics> getMetricsHistory(uint64_t steps = 1000) const;
    
    // Analysis methods
    double getAverageStepTime(uint64_t steps) const;
    double getAverageCPUUsage(uint64_t steps) const;
    double getAverageMemoryUsage(uint64_t steps) const;
    double getAverageFiringRate(uint64_t steps) const;
    std::vector<double> getStepTimeTrend(uint64_t steps) const;
    std::vector<double> getMemoryUsageTrend(uint64_t steps) const;
    std::vector<double> getCPUUsageTrend(uint64_t steps) const;
    
    // Bottleneck detection
    bool detectBottlenecks() const;
    std::string getBottleneckAnalysis() const;
    
    // Performance profiling
    void startProfiling(const std::string& profile_name);
    void stopProfiling(const std::string& profile_name);
    void profileFunction(const std::string& function_name, std::function<void()> func);
    std::map<std::string, double> getProfileResults() const;
    
    // Configuration
    void setSamplingInterval(uint64_t interval) { sampling_interval_ = interval; }
    void setMemoryLimitMB(double limit) { memory_limit_mb_ = limit; }
    void setCPUThreshold(double threshold) { cpu_threshold_percent_ = threshold; }
    void enableRealTimeReporting(bool enable) { real_time_reporting_ = enable; }
    
    // Data export
    bool exportMetrics(const std::string& filepath, uint64_t start_step, uint64_t end_step) const;
    bool exportJSON(const std::string& filepath, uint64_t start_step, uint64_t end_step) const;
    bool exportCSV(const std::string& filepath, uint64_t start_step, uint64_t end_step) const;
    std::string getMetricsSummary(uint64_t steps) const;
    
    // Spike event analysis
    struct SpikeAnalysis {
        uint64_t total_spikes = 0;
        double average_spike_frequency = 0.0;
        std::vector<double> spike_intervals;
        std::vector<std::pair<uint64_t, double>> region_spike_counts;
        double burst_detection_threshold = 0.0;
        bool has_bursts = false;
    };
    
    SpikeAnalysis analyzeSpikeEvents(uint64_t steps) const;
    
    // Memory analysis
    struct MemoryAnalysis {
        double peak_memory_mb = 0.0;
        double average_memory_mb = 0.0;
        double memory_growth_rate_mb_per_step = 0.0;
        std::vector<double> memory_usage_over_time;
        std::vector<uint32_t> memory_allocation_pattern;
        bool memory_leak_detected = false;
        double memory_leak_rate = 0.0;
    };
    
    MemoryAnalysis analyzeMemoryUsage(uint64_t steps) const;
    
    // System analysis
    struct SystemAnalysis {
        double average_cpu_usage = 0.0;
        double peak_cpu_usage = 0.0;
        double average_memory_percent = 0.0;
        double peak_memory_percent = 0.0;
        std::vector<double> cpu_usage_over_time;
        std::vector<double> memory_usage_over_time;
        bool cpu_saturation_events = 0;
        bool memory_pressure_events = 0;
    };
    
    SystemAnalysis analyzeSystemResources(uint64_t steps) const;
    
    // Performance comparison
    struct PerformanceComparison {
        double performance_score = 0.0;
        std::map<std::string, double> metric_improvements;
        std::map<std::string, double> metric_degradations;
        std::string best_configuration;
    };
    
    PerformanceComparison compareConfigurations(
        const std::vector<std::vector<PerformanceMetrics>>& datasets,
        const std::vector<std::string>& config_names) const;
    
private:
    mutable std::mutex metrics_mutex_;
    std::atomic<bool> monitoring_ = false;
    uint64_t sampling_interval_ = 1;
    double memory_limit_mb_ = 1024.0;
    double cpu_threshold_percent_ = 90.0;
    bool real_time_reporting_ = false;
    
    struct ProfilingData {
        std::chrono::steady_clock::time_point start_time;
        std::map<std::string, double> function_times;
        std::map<std::string, uint64_t> call_counts;
    };
    
    std::map<std::string, ProfilingData> profiling_sessions_;
    
    std::vector<PerformanceMetrics> metrics_history_;
    uint64_t total_steps_monitored_ = 0;
    std::chrono::steady_clock::time_point simulation_start_time_;
    
    // Internal helper methods
    void updateSystemMetrics(SystemResourceMetrics& metrics);
    void updateNeuralMetrics(NeuralComputationMetrics& metrics, const PerformanceMetrics& prev_metrics);
    void updateMemoryMetrics(MemoryUsageMetrics& metrics, const PerformanceMetrics& prev_metrics);
    void updateSpikeMetrics(SpikeEventMetrics& metrics, const PerformanceMetrics& prev_metrics);
    double calculateCPUUsage() const;
    double calculateMemoryUsageMB() const;
    uint64_t getProcessID() const;
    
    // File I/O helpers
    static std::string metricsToJSON(const PerformanceMetrics& metrics);
    static PerformanceMetrics metricsFromJSON(const std::string& json_str);
};

} // namespace nlm
