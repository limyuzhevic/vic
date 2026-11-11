// Performance profiling and optimization utilities for NLM

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "core/Logger/Logger.hpp"

namespace nlm {
namespace main {

class PerformanceProfiler {
public:
    struct TimingResult {
        std::string operation;
        std::chrono::duration<double> elapsed;
        size_t iterations = 1;
        double perIteration() const { return elapsed.count() / iterations; }
    };

    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    void reset();
    void printReport() const;
    double getTotalTime() const;
    const std::vector<TimingResult>& getResults() const { return results_; }
    
    static PerformanceProfiler& getInstance();

private:
    struct TimerEntry {
        std::string operation;
        std::chrono::steady_clock::time_point start;
    };
    
    std::vector<TimingResult> results_;
    std::vector<TimerEntry> active_timers_;
    std::chrono::steady_clock::time_point session_start_;
};

class MemoryProfiler {
public:
    struct MemoryInfo {
        size_t total_allocated = 0;
        size_t current_allocated = 0;
        size_t peak_allocated = 0;
        std::unordered_map<std::string, size_t> allocations_by_type;
    };

    MemoryProfiler();
    ~MemoryProfiler();
    
    void trackAllocation(const std::string& type, size_t size);
    void trackDeallocation(const std::string& type, size_t size);
    void printMemoryReport() const;
    const MemoryInfo& getMemoryInfo() const { return memory_info_; }
    
private:
    MemoryInfo memory_info_;
    bool tracking_enabled_ = false;
};

class OptimizationHints {
public:
    struct NetworkOptimization {
        bool enable_spike_optimization = true;
        bool enable_synaptic_optimization = true;
        bool enable_plasticity_optimization = true;
        bool enable_memory_optimization = true;
        int optimization_level = 2; // 1=basic, 2=advanced, 3=aggressive
        float optimization_gain = 1.2f; // Expected speedup factor
    };

    struct SimulationOptimization {
        bool enable_event_driven = true;
        bool enable_batch_processing = true;
        bool enable_parallel_spikes = true;
        int spike_processing_batch_size = 100;
        bool enable_compatibility_mode = true; // Fallback to safe defaults
    };

    static NetworkOptimization getNetworkOptimization(const std::string& level = "auto");
    static SimulationOptimization getSimulationOptimization(const std::string& level = "auto");
    static void printOptimizationSummary(const NetworkOptimization& net_opt, const SimulationOptimization& sim_opt);
    static bool validateOptimization(const NetworkOptimization& net_opt, const SimulationOptimization& sim_opt);
};

// Advanced configuration options for power users
class AdvancedConfiguration {
public:
    struct AdvancedOptions {
        // Debug and testing options
        bool enable_debug_output = false;
        bool enable_silence_mode = false;
        bool enable_reduced_logging = false;
        bool enable_validation_checks = true;
        
        // Advanced plasticity options
        bool enable_custom_stdp = false;
        bool enable_reward_modulated_plasticity = false;
        float custom_stdp_rate = 0.001f;
        
        // Development and debugging options
        bool enable_developer_console = false;
        bool enable_memory_leak_detection = false;
        bool enable_performance_profiling = false;
        
        // Export and analysis options
        bool enable_checkpoint_export = false;
        bool enable_statistical_analysis = false;
        bool enable_detailed_metrics = false;
        
        // Network architecture options
        bool enable_dynamic_rewiring = false;
        bool enable_meta_plasticity = false;
        bool enable_structural_plasticity = false;
        
        // Environment interaction options
        bool enable_external_input = false;
        bool enable_reward_shaping = false;
        bool enable_curiosity_driven_learning = false;
    };

    static AdvancedOptions getDefaultAdvancedOptions();
    static AdvancedOptions getPowerUserOptions();
    static bool validateAdvancedOptions(const AdvancedOptions& options);
    static void printAdvancedOptionsSummary(const AdvancedOptions& options);
};

// Enhanced error recovery and resilience
class ErrorRecovery {
public:
    enum class RecoveryLevel {
        NONE,      // No recovery, immediate failure
        BASIC,     // Try to recover from non-critical errors
        ADVANCED,  // Attempt automatic recovery with fallbacks
        COMPREHENSIVE // Full recovery with state restoration
    };

    struct RecoveryConfig {
        RecoveryLevel level = RecoveryLevel::BASIC;
        bool auto_reset_on_failure = true;
        bool save_state_on_error = false;
        bool continue_on_minor_errors = true;
        bool enable_graceful_degradation = true;
    };

    static RecoveryConfig getRecoveryConfig(const std::string& level = "auto");
    static bool shouldRecover(const std::string& errorType);
    static void handleRecovery(const std::string& errorType, const std::exception& e);
    static void printRecoverySummary(const RecoveryConfig& config);
};

// Enhanced visualization and monitoring
class AdvancedVisualization {
public:
    struct VisualizationOptions {
        bool enable_real_time_monitoring = false;
        bool enable_network_metrics = false;
        bool enable_plasticity_metrics = false;
        bool enable_memory_usage = false;
        bool enable_spike_pattern_analysis = false;
        int update_interval_ms = 1000;
        bool enable_performance_dashboard = false;
        bool enable_detailed_analytics = false;
    };

    static VisualizationOptions getVisualizationOptions(const std::string& mode = "auto");
    static void printVisualizationOptionsSummary(const VisualizationOptions& options);
    static void printAdvancedMetrics(const std::shared_ptr<Brain>& brain, const AdvancedConfiguration::AdvancedOptions& adv_opts);
};

// Command-line argument parser for advanced options
class AdvancedArgumentParser {
public:
    struct AdvancedArgs {
        std::string custom_config_file;
        std::string output_file;
        std::string log_file;
        std::string checkpoint_file;
        std::string profile_output;
        std::string metrics_output;
        
        // Optimization settings
        std::string network_optimization_level;
        std::string simulation_optimization_level;
        std::string recovery_level;
        
        // Debugging settings
        std::string debug_mode;
        std::string log_level;
        bool enable_profiling = false;
        bool enable_memory_tracking = false;
        bool enable_validation = false;
        
        // Visualization settings
        std::string visualization_mode;
        bool enable_dashboard = false;
        
        // Performance settings
        int max_memory_mb = 1024;
        double max_time_seconds = 300.0;
        int thread_count = std::thread::hardware_concurrency();
        
        // Advanced plasticity settings
        float custom_stdp_rate = -1.0f; // -1.0 means use default
        bool enable_reward_modulated = false;
        
        // Development settings
        bool enable_developer_mode = false;
        bool enable_auto_save = false;
        int auto_save_interval = 60;
    };

    static AdvancedArgs parseAdvancedArgs(int argc, char** argv);
    static void printAdvancedArgsSummary(const AdvancedArgs& args);
    static bool validateAdvancedArgs(const AdvancedArgs& args);
};

// Main integration class for all advanced features
class AdvancedNLMFeatures {
public:
    struct Configuration {
        PerformanceProfiler profiler;
        MemoryProfiler memory_profiler;
        OptimizationHints::NetworkOptimization network_opt;
        OptimizationHints::SimulationOptimization sim_opt;
        AdvancedConfiguration::AdvancedOptions advanced_opts;
        ErrorRecovery::RecoveryConfig recovery_config;
        AdvancedVisualization::VisualizationOptions viz_options;
        AdvancedArgumentParser::AdvancedArgs parsed_args;
    };

    static bool initializeAdvancedFeatures(Configuration& config);
    static void printAdvancedFeaturesSummary(const Configuration& config);
    static void runAdvancedPerformanceTests(std::shared_ptr<Brain> brain, Configuration& config);
    static void cleanupAdvancedFeatures(Configuration& config);
    
private:
    static void setupProfilingInfrastructure(Configuration& config);
    static void setupMemoryTracking(Configuration& config);
    static void setupAdvancedLogging(Configuration& config);
    static void setupVisualization(Configuration& config);
};

}} // namespace nlm::main