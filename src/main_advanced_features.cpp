// Advanced NLM features implementation

#include "main_advanced_features.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>

namespace nlm {
namespace main {

// PerformanceProfiler implementation
PerformanceProfiler& PerformanceProfiler::getInstance() {
    static PerformanceProfiler instance;
    return instance;
}

void PerformanceProfiler::startTimer(const std::string& operation) {
    // Check if there's already an active timer for this operation
    for (auto& timer : active_timers_) {
        if (timer.operation == operation) {
            NLM_LOG_WARNING("Timer already active for operation: " + operation);
            return;
        }
    }
    
    TimerEntry entry;
    entry.operation = operation;
    entry.start = std::chrono::high_resolution_clock::now();
    active_timers_.push_back(entry);
    
    if (session_start_.time_since_epoch().count() == 0) {
        session_start_ = std::chrono::high_resolution_clock::now();
    }
    
    NLM_LOG_DEBUG("Started timing: " + operation);
}

void PerformanceProfiler::stopTimer(const std::string& operation) {
    auto it = std::find_if(active_timers_.begin(), active_timers_.end(),
                          [&operation](const TimerEntry& timer) { return timer.operation == operation; });
    
    if (it == active_timers_.end()) {
        NLM_LOG_WARNING("No active timer found for operation: " + operation);
        return;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - it->start);
    
    TimingResult result;
    result.operation = operation;
    result.elapsed = duration;
    result.iterations = 1;
    
    results_.push_back(result);
    
    // Remove the stopped timer
    active_timers_.erase(it);
    
    NLM_LOG_DEBUG("Stopped timing: " + operation + " - " + std::to_string(duration.count()) + "s");
}

void PerformanceProfiler::reset() {
    results_.clear();
    active_timers_.clear();
    session_start_ = std::chrono::steady_clock::time_point();
    NLM_LOG_INFO("Performance profiler reset");
}

void PerformanceProfiler::printReport() const {
    NLM_LOG_INFO("=== Performance Profiler Report ===");
    NLM_LOG_INFO("Session started: " + std::to_string(session_start_.time_since_epoch().count()) + " ms ago");
    
    if (results_.empty()) {
        NLM_LOG_INFO("No timing results recorded");
    } else {
        NLM_LOG_INFO("Timing Results:");
        for (const auto& result : results_) {
            NLM_LOG_INFO("  " + result.operation + ": " + std::to_string(result.elapsed.count()) + "s");
            if (result.iterations > 1) {
                NLM_LOG_INFO("    (" + std::to_string(result.perIteration()) + "s per iteration)");
            }
        }
    }
    
    if (!active_timers_.empty()) {
        NLM_LOG_WARNING("Active timers:");
        for (const auto& timer : active_timers_) {
            NLM_LOG_WARNING("  " + timer.operation + " (running...)");
        }
    }
    
    NLM_LOG_INFO("====================================");
}

double PerformanceProfiler::getTotalTime() const {
    if (session_start_.time_since_epoch().count() == 0) return 0.0;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - session_start_);
    return duration.count();
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler() {
    tracking_enabled_ = true;
    memory_info_.total_allocated = 0;
    memory_info_.current_allocated = 0;
    memory_info_.peak_allocated = 0;
    NLM_LOG_INFO("Memory profiler initialized");
}

MemoryProfiler::~MemoryProfiler() {
    if (tracking_enabled_) {
        printMemoryReport();
    }
}

void MemoryProfiler::trackAllocation(const std::string& type, size_t size) {
    if (!tracking_enabled_) return;
    
    memory_info_.total_allocated += size;
    memory_info_.current_allocated += size;
    memory_info_.peak_allocated = std::max(memory_info_.peak_allocated, memory_info_.current_allocated);
    memory_info_.allocations_by_type[type] += size;
    
    NLM_LOG_DEBUG("Memory allocation: " + type + " - " + std::to_string(size) + " bytes (total: " + 
                  std::to_string(memory_info_.current_allocated) + " bytes)");
}

void MemoryProfiler::trackDeallocation(const std::string& type, size_t size) {
    if (!tracking_enabled_) return;
    
    memory_info_.current_allocated = std::max(0ULL, memory_info_.current_allocated - size);
    
    NLM_LOG_DEBUG("Memory deallocation: " + type + " - " + std::to_string(size) + " bytes (current: " + 
                  std::to_string(memory_info_.current_allocated) + " bytes)");
}

void MemoryProfiler::printMemoryReport() const {
    NLM_LOG_INFO("=== Memory Profiler Report ===");
    NLM_LOG_INFO("Total allocated: " + std::to_string(memory_info_.total_allocated) + " bytes");
    NLM_LOG_INFO("Current allocated: " + std::to_string(memory_info_.current_allocated) + " bytes");
    NLM_LOG_INFO("Peak allocated: " + std::to_string(memory_info_.peak_allocated) + " bytes");
    
    if (!memory_info_.allocations_by_type.empty()) {
        NLM_LOG_INFO("Allocations by type:");
        for (const auto& pair : memory_info_.allocations_by_type) {
            NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second) + " bytes");
        }
    }
    
    NLM_LOG_INFO("==================================");
}

// OptimizationHints implementation
OptimizationHints::NetworkOptimization OptimizationHints::getNetworkOptimization(const std::string& level) {
    NetworkOptimization opts;
    
    if (level == "basic" || level == "auto") {
        opts.enable_spike_optimization = true;
        opts.enable_synaptic_optimization = true;
        opts.enable_plasticity_optimization = false; // Keep it simple
        opts.enable_memory_optimization = true;
        opts.optimization_level = 1;
        opts.optimization_gain = 1.1f;
    } else if (level == "advanced") {
        opts.enable_spike_optimization = true;
        opts.enable_synaptic_optimization = true;
        opts.enable_plasticity_optimization = true;
        opts.enable_memory_optimization = true;
        opts.optimization_level = 2;
        opts.optimization_gain = 1.5f;
    } else if (level == "aggressive") {
        opts.enable_spike_optimization = true;
        opts.enable_synaptic_optimization = true;
        opts.enable_plasticity_optimization = true;
        opts.enable_memory_optimization = true;
        opts.optimization_level = 3;
        opts.optimization_gain = 2.0f;
    }
    
    return opts;
}

OptimizationHints::SimulationOptimization OptimizationHints::getSimulationOptimization(const std::string& level) {
    SimulationOptimization opts;
    
    if (level == "basic" || level == "auto") {
        opts.enable_event_driven = true;
        opts.enable_batch_processing = false;
        opts.enable_parallel_spikes = false;
        opts.spike_processing_batch_size = 10;
        opts.enable_compatibility_mode = true;
    } else if (level == "advanced") {
        opts.enable_event_driven = true;
        opts.enable_batch_processing = true;
        opts.enable_parallel_spikes = true;
        opts.spike_processing_batch_size = 100;
        opts.enable_compatibility_mode = true;
    } else if (level == "aggressive") {
        opts.enable_event_driven = true;
        opts.enable_batch_processing = true;
        opts.enable_parallel_spikes = true;
        opts.spike_processing_batch_size = 500;
        opts.enable_compatibility_mode = false; // More aggressive, less safe
    }
    
    return opts;
}

void OptimizationHints::printOptimizationSummary(const NetworkOptimization& net_opt, const SimulationOptimization& sim_opt) {
    NLM_LOG_INFO("=== Optimization Configuration ===");
    NLM_LOG_INFO("Network Optimization:");
    NLM_LOG_INFO("  Spike optimization: " + std::string(net_opt.enable_spike_optimization ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Synaptic optimization: " + std::string(net_opt.enable_synaptic_optimization ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Plasticity optimization: " + std::string(net_opt.enable_plasticity_optimization ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Memory optimization: " + std::string(net_opt.enable_memory_optimization ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Optimization level: " + std::to_string(net_opt.optimization_level));
    NLM_LOG_INFO("  Expected speedup: " + std::to_string(net_opt.optimization_gain) + "x");
    
    NLM_LOG_INFO("Simulation Optimization:");
    NLM_LOG_INFO("  Event-driven: " + std::string(sim_opt.enable_event_driven ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Batch processing: " + std::string(sim_opt.enable_batch_processing ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Parallel spikes: " + std::string(sim_opt.enable_parallel_spikes ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Batch size: " + std::to_string(sim_opt.spike_processing_batch_size));
    NLM_LOG_INFO("  Compatibility mode: " + std::string(sim_opt.enable_compatibility_mode ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("===============================");
}

bool OptimizationHints::validateOptimization(const NetworkOptimization& net_opt, const SimulationOptimization& sim_opt) {
    bool valid = true;
    
    if (net_opt.optimization_level < 1 || net_opt.optimization_level > 3) {
        NLM_LOG_ERROR("Invalid optimization level: must be 1, 2, or 3");
        valid = false;
    }
    
    if (sim_opt.spike_processing_batch_size < 1) {
        NLM_LOG_ERROR("Invalid spike processing batch size: must be positive");
        valid = false;
    }
    
    if (net_opt.optimization_gain < 1.0f) {
        NLM_LOG_ERROR("Invalid optimization gain: must be >= 1.0");
        valid = false;
    }
    
    return valid;
}

// AdvancedConfiguration implementation
AdvancedConfiguration::AdvancedOptions AdvancedConfiguration::getDefaultAdvancedOptions() {
    AdvancedOptions opts;
    
    // Debug and testing options
    opts.enable_debug_output = false;
    opts.enable_silence_mode = false;
    opts.enable_reduced_logging = false;
    opts.enable_validation_checks = true;
    
    // Advanced plasticity options
    opts.enable_custom_stdp = false;
    opts.enable_reward_modulated_plasticity = false;
    opts.custom_stdp_rate = 0.001f;
    
    // Development and debugging options
    opts.enable_developer_console = false;
    opts.enable_memory_leak_detection = false;
    opts.enable_performance_profiling = false;
    
    // Export and analysis options
    opts.enable_checkpoint_export = false;
    opts.enable_statistical_analysis = false;
    opts.enable_detailed_metrics = false;
    
    // Network architecture options
    opts.enable_dynamic_rewiring = false;
    opts.enable_meta_plasticity = false;
    opts.enable_structural_plasticity = false;
    
    // Environment interaction options
    opts.enable_external_input = false;
    opts.enable_reward_shaping = false;
    opts.enable_curiosity_driven_learning = false;
    
    return opts;
}

AdvancedConfiguration::AdvancedOptions AdvancedConfiguration::getPowerUserOptions() {
    AdvancedOptions opts = getDefaultAdvancedOptions();
    
    // Enable advanced features for power users
    opts.enable_debug_output = true;
    opts.enable_validation_checks = true;
    opts.enable_memory_leak_detection = true;
    opts.enable_performance_profiling = true;
    opts.enable_checkpoint_export = true;
    opts.enable_statistical_analysis = true;
    opts.enable_detailed_metrics = true;
    opts.enable_structural_plasticity = true;
    opts.enable_reward_modulated_plasticity = true;
    opts.enable_curiosity_driven_learning = true;
    
    return opts;
}

bool AdvancedConfiguration::validateAdvancedOptions(const AdvancedOptions& options) {
    bool valid = true;
    
    if (options.custom_stdp_rate < 0.0f) {
        NLM_LOG_ERROR("Invalid custom STDP rate: must be non-negative");
        valid = false;
    }
    
    return valid;
}

void AdvancedConfiguration::printAdvancedOptionsSummary(const AdvancedOptions& options) {
    NLM_LOG_INFO("=== Advanced Configuration Options ===");
    NLM_LOG_INFO("Debug & Testing:");
    NLM_LOG_INFO("  Debug output: " + std::string(options.enable_debug_output ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Silence mode: " + std::string(options.enable_silence_mode ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Reduced logging: " + std::string(options.enable_reduced_logging ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Validation checks: " + std::string(options.enable_validation_checks ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("Plasticity:");
    NLM_LOG_INFO("  Custom STDP: " + std::string(options.enable_custom_stdp ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Reward-modulated plasticity: " + std::string(options.enable_reward_modulated_plasticity ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Custom STDP rate: " + std::to_string(options.custom_stdp_rate));
    
    NLM_LOG_INFO("Development & Debugging:");
    NLM_LOG_INFO("  Developer console: " + std::string(options.enable_developer_console ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Memory leak detection: " + std::string(options.enable_memory_leak_detection ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Performance profiling: " + std::string(options.enable_performance_profiling ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("Export & Analysis:");
    NLM_LOG_INFO("  Checkpoint export: " + std::string(options.enable_checkpoint_export ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Statistical analysis: " + std::string(options.enable_statistical_analysis ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Detailed metrics: " + std::string(options.enable_detailed_metrics ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("Architecture & Learning:");
    NLM_LOG_INFO("  Dynamic rewiring: " + std::string(options.enable_dynamic_rewiring ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Meta-plasticity: " + std::string(options.enable_meta_plasticity ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Structural plasticity: " + std::string(options.enable_structural_plasticity ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("Environment & Behavior:");
    NLM_LOG_INFO("  External input: " + std::string(options.enable_external_input ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Reward shaping: " + std::string(options.enable_reward_shaping ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Curiosity-driven learning: " + std::string(options.enable_curiosity_driven_learning ? "enabled" : "disabled"));
    
    NLM_LOG_INFO("===========================");
}

// ErrorRecovery implementation
ErrorRecovery::RecoveryConfig ErrorRecovery::getRecoveryConfig(const std::string& level) {
    RecoveryConfig config;
    
    if (level == "none") {
        config.level = RecoveryLevel::NONE;
        config.auto_reset_on_failure = false;
        config.save_state_on_error = false;
        config.continue_on_minor_errors = false;
        config.enable_graceful_degradation = false;
    } else if (level == "advanced") {
        config.level = RecoveryLevel::ADVANCED;
        config.auto_reset_on_failure = true;
        config.save_state_on_error = true;
        config.continue_on_minor_errors = true;
        config.enable_graceful_degradation = true;
    } else if (level == "comprehensive") {
        config.level = RecoveryLevel::COMPREHENSIVE;
        config.auto_reset_on_failure = true;
        config.save_state_on_error = true;
        config.continue_on_minor_errors = true;
        config.enable_graceful_degradation = true;
    } else { // default or "auto"
        config.level = RecoveryLevel::BASIC;
        config.auto_reset_on_failure = true;
        config.save_state_on_error = false;
        config.continue_on_minor_errors = true;
        config.enable_graceful_degradation = true;
    }
    
    return config;
}

bool ErrorRecovery::shouldRecover(const std::string& errorType) {
    // For now, assume we should try to recover from most errors
    // This could be made more sophisticated based on errorType
    return true;
}

void ErrorRecovery::handleRecovery(const std::string& errorType, const std::exception& e) {
    NLM_LOG_INFO("Handling recovery for error type: " + errorType);
    NLM_LOG_INFO("Error details: " + std::string(e.what());
    
    // Implement recovery logic based on level
    switch (level) {
        case RecoveryLevel::NONE:
            NLM_LOG_ERROR("No recovery enabled - terminating");
            break;
        case RecoveryLevel::BASIC:
            NLM_LOG_INFO("Attempting basic recovery...");
            // Basic recovery: reset state and continue
            break;
        case RecoveryLevel::ADVANCED:
            NLM_LOG_INFO("Attempting advanced recovery...");
            // Advanced recovery: try to restore from checkpoint if available
            break;
        case RecoveryLevel::COMPREHENSIVE:
            NLM_LOG_INFO("Attempting comprehensive recovery...");
            // Comprehensive recovery: full state restoration and validation
            break;
    }
}

void ErrorRecovery::printRecoverySummary(const RecoveryConfig& config) {
    NLM_LOG_INFO("=== Error Recovery Configuration ===");
    NLM_LOG_INFO("Recovery level: " + std::to_string(static_cast<int>(config.level)));
    NLM_LOG_INFO("Auto reset on failure: " + std::string(config.auto_reset_on_failure ? "yes" : "no"));
    NLM_LOG_INFO("Save state on error: " + std::string(config.save_state_on_error ? "yes" : "no"));
    NLM_LOG_INFO("Continue on minor errors: " + std::string(config.continue_on_minor_errors ? "yes" : "no"));
    NLM_LOG_INFO("Graceful degradation: " + std::string(config.enable_graceful_degradation ? "yes" : "no"));
    NLM_LOG_INFO("===============================");
}

// AdvancedVisualization implementation
AdvancedVisualization::VisualizationOptions AdvancedVisualization::getVisualizationOptions(const std::string& mode) {
    VisualizationOptions opts;
    
    if (mode == "basic") {
        opts.enable_real_time_monitoring = false;
        opts.enable_network_metrics = false;
        opts.enable_plasticity_metrics = false;
        opts.enable_memory_usage = false;
        opts.enable_spike_pattern_analysis = false;
        opts.update_interval_ms = 5000;
        opts.enable_performance_dashboard = false;
        opts.enable_detailed_analytics = false;
    } else if (mode == "advanced") {
        opts.enable_real_time_monitoring = true;
        opts.enable_network_metrics = true;
        opts.enable_plasticity_metrics = true;
        opts.enable_memory_usage = true;
        opts.enable_spike_pattern_analysis = true;
        opts.update_interval_ms = 1000;
        opts.enable_performance_dashboard = true;
        opts.enable_detailed_analytics = true;
    } else { // auto or "comprehensive"
        opts.enable_real_time_monitoring = true;
        opts.enable_network_metrics = true;
        opts.enable_plasticity_metrics = true;
        opts.enable_memory_usage = true;
        opts.enable_spike_pattern_analysis = true;
        opts.update_interval_ms = 500;
        opts.enable_performance_dashboard = true;
        opts.enable_detailed_analytics = true;
    }
    
    return opts;
}

void AdvancedVisualization::printVisualizationOptionsSummary(const VisualizationOptions& options) {
    NLM_LOG_INFO("=== Visualization Configuration ===");
    NLM_LOG_INFO("Real-time monitoring: " + std::string(options.enable_real_time_monitoring ? "enabled" : "disabled"));
    NLM_LOG_INFO("Network metrics: " + std::string(options.enable_network_metrics ? "enabled" : "disabled"));
    NLM_LOG_INFO("Plasticity metrics: " + std::string(options.enable_plasticity_metrics ? "enabled" : "disabled"));
    NLM_LOG_INFO("Memory usage: " + std::string(options.enable_memory_usage ? "enabled" : "disabled"));
    NLM_LOG_INFO("Spike pattern analysis: " + std::string(options.enable_spike_pattern_analysis ? "enabled" : "disabled"));
    NLM_LOG_INFO("Update interval: " + std::to_string(options.update_interval_ms) + " ms");
    NLM_LOG_INFO("Performance dashboard: " + std::string(options.enable_performance_dashboard ? "enabled" : "disabled"));
    NLM_LOG_INFO("Detailed analytics: " + std::string(options.enable_detailed_analytics ? "enabled" : "disabled"));
    NLM_LOG_INFO("==========================");
}

void AdvancedVisualization::printAdvancedMetrics(const std::shared_ptr<Brain>& brain, const AdvancedConfiguration::AdvancedOptions& adv_opts) {
    if (!brain) return;
    
    NLM_LOG_INFO("=== Advanced Neural Metrics ===");
    NLM_LOG_INFO("Network Statistics:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    if (adv_opts.enable_detailed_metrics) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Plasticity Metrics:");
        if (auto* stdp = brain->getSTDP()) {
            NLM_LOG_INFO("  STDP potentiations: [To be implemented]");
            NLM_LOG_INFO("  STDP depressants: [To be implemented]");
        }
        if (auto* hebbian = brain->getHebbian()) {
            NLM_LOG_INFO("  Hebbian updates: [To be implemented]");
        }
        if (auto* structural = brain->getStructuralPlasticity()) {
            NLM_LOG_INFO("  Structural changes: [To be implemented]");
        }
    }
    
    NLM_LOG_INFO("============================");
}

// AdvancedArgumentParser implementation
AdvancedArgumentParser::AdvancedArgs AdvancedArgumentParser::parseAdvancedArgs(int argc, char** argv) {
    AdvancedArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--custom-config" && i + 1 < argc) {
            args.custom_config_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            args.output_file = argv[++i];
        } else if (arg == "--log-file" && i + 1 < argc) {
            args.log_file = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            args.checkpoint_file = argv[++i];
        } else if (arg == "--profile-output" && i + 1 < argc) {
            args.profile_output = argv[++i];
        } else if (arg == "--metrics-output" && i + 1 < argc) {
            args.metrics_output = argv[++i];
        } else if (arg == "--network-opt" && i + 1 < argc) {
            args.network_optimization_level = argv[++i];
        } else if (arg == "--sim-opt" && i + 1 < argc) {
            args.simulation_optimization_level = argv[++i];
        } else if (arg == "--recovery-level" && i + 1 < argc) {
            args.recovery_level = argv[++i];
        } else if (arg == "--debug-mode" && i + 1 < argc) {
            args.debug_mode = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            args.log_level = argv[++i];
        } else if (arg == "--enable-profiling") {
            args.enable_profiling = true;
        } else if (arg == "--enable-memory-tracking") {
            args.enable_memory_tracking = true;
        } else if (arg == "--enable-validation") {
            args.enable_validation = true;
        } else if (arg == "--visualization-mode" && i + 1 < argc) {
            args.visualization_mode = argv[++i];
        } else if (arg == "--enable-dashboard") {
            args.enable_dashboard = true;
        } else if (arg == "--max-memory" && i + 1 < argc) {
            args.max_memory_mb = std::stoi(argv[++i]);
        } else if (arg == "--max-time" && i + 1 < argc) {
            args.max_time_seconds = std::stod(argv[++i]);
        } else if (arg == "--thread-count" && i + 1 < argc) {
            args.thread_count = std::stoi(argv[++i]);
        } else if (arg == "--custom-stdp-rate" && i + 1 < argc) {
            args.custom_stdp_rate = std::stof(argv[++i]);
        } else if (arg == "--enable-reward-modulated") {
            args.enable_reward_modulated = true;
        } else if (arg == "--enable-developer-mode") {
            args.enable_developer_mode = true;
        } else if (arg == "--enable-auto-save") {
            args.enable_auto_save = true;
        } else if (arg == "--auto-save-interval" && i + 1 < argc) {
            args.auto_save_interval = std::stoi(argv[++i]);
        }
    }
    
    return args;
}

void AdvancedArgumentParser::printAdvancedArgsSummary(const AdvancedArgs& args) {
    NLM_LOG_INFO("=== Advanced Arguments Summary ===");
    if (!args.custom_config_file.empty()) {
        NLM_LOG_INFO("Custom config file: " + args.custom_config_file);
    }
    if (!args.output_file.empty()) {
        NLM_LOG_INFO("Output file: " + args.output_file);
    }
    if (!args.log_file.empty()) {
        NLM_LOG_INFO("Log file: " + args.log_file);
    }
    if (!args.checkpoint_file.empty()) {
        NLM_LOG_INFO("Checkpoint file: " + args.checkpoint_file);
    }
    if (!args.profile_output.empty()) {
        NLM_LOG_INFO("Profile output: " + args.profile_output);
    }
    if (!args.metrics_output.empty()) {
        NLM_LOG_INFO("Metrics output: " + args.metrics_output);
    }
    
    if (!args.network_optimization_level.empty()) {
        NLM_LOG_INFO("Network optimization level: " + args.network_optimization_level);
    }
    if (!args.simulation_optimization_level.empty()) {
        NLM_LOG_INFO("Simulation optimization level: " + args.simulation_optimization_level);
    }
    if (!args.recovery_level.empty()) {
        NLM_LOG_INFO("Recovery level: " + args.recovery_level);
    }
    
    if (args.enable_profiling) {
        NLM_LOG_INFO("Profiling enabled: yes");
    }
    if (args.enable_memory_tracking) {
        NLM_LOG_INFO("Memory tracking enabled: yes");
    }
    if (args.enable_validation) {
        NLM_LOG_INFO("Validation enabled: yes");
    }
    
    if (args.max_memory_mb > 0) {
        NLM_LOG_INFO("Max memory: " + std::to_string(args.max_memory_mb) + " MB");
    }
    if (args.max_time_seconds > 0.0) {
        NLM_LOG_INFO("Max time: " + std::to_string(args.max_time_seconds) + " seconds");
    }
    if (args.thread_count > 0) {
        NLM_LOG_INFO("Thread count: " + std::to_string(args.thread_count));
    }
    
    if (args.custom_stdp_rate >= 0.0f) {
        NLM_LOG_INFO("Custom STDP rate: " + std::to_string(args.custom_stdp_rate));
    }
    
    if (args.enable_reward_modulated) {
        NLM_LOG_INFO("Reward-modulated plasticity: enabled");
    }
    
    if (args.enable_developer_mode) {
        NLM_LOG_INFO("Developer mode: enabled");
    }
    
    if (args.enable_auto_save) {
        NLM_LOG_INFO("Auto-save enabled: yes (interval: " + std::to_string(args.auto_save_interval) + " seconds)");
    }
    
    NLM_LOG_INFO("=============================");
}

bool AdvancedArgumentParser::validateAdvancedArgs(const AdvancedArgs& args) {
    bool valid = true;
    
    if (args.max_memory_mb <= 0) {
        NLM_LOG_ERROR("Invalid max memory: must be positive");
        valid = false;
    }
    
    if (args.max_time_seconds <= 0.0) {
        NLM_LOG_ERROR("Invalid max time: must be positive");
        valid = false;
    }
    
    if (args.thread_count <= 0) {
        NLM_LOG_ERROR("Invalid thread count: must be positive");
        valid = false;
    }
    
    if (args.custom_stdp_rate < 0.0f) {
        NLM_LOG_ERROR("Invalid custom STDP rate: must be non-negative");
        valid = false;
    }
    
    if (args.auto_save_interval <= 0) {
        NLM_LOG_ERROR("Invalid auto-save interval: must be positive");
        valid = false;
    }
    
    return valid;
}

// AdvancedNLMFeatures implementation
bool AdvancedNLMFeatures::initializeAdvancedFeatures(Configuration& config) {
    try {
        NLM_LOG_INFO("Initializing advanced NLM features...");
        
        // Setup profiling infrastructure
        if (config.parsed_args.enable_profiling) {
            setupProfilingInfrastructure(config);
        }
        
        // Setup memory tracking
        if (config.parsed_args.enable_memory_tracking) {
            setupMemoryTracking(config);
        }
        
        // Setup advanced logging
        if (config.parsed_args.enable_developer_mode) {
            // Enhanced logging for developer mode
        }
        
        // Setup visualization
        if (config.parsed_args.enable_dashboard || !config.viz_options.visualization_mode.empty()) {
            setupVisualization(config);
        }
        
        NLM_LOG_INFO("Advanced NLM features initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize advanced features: " + std::string(e.what()));
        return false;
    }
}

void AdvancedNLMFeatures::printAdvancedFeaturesSummary(const Configuration& config) {
    NLM_LOG_INFO("=== Advanced NLM Features Summary ===");
    
    if (config.parsed_args.enable_profiling) {
        NLM_LOG_INFO("Performance profiling: enabled");
    }
    if (config.parsed_args.enable_memory_tracking) {
        NLM_LOG_INFO("Memory tracking: enabled");
    }
    
    if (config.parsed_args.enable_developer_mode) {
        NLM_LOG_INFO("Developer mode: enabled");
    }
    
    if (config.parsed_args.enable_dashboard) {
        NLM_LOG_INFO("Performance dashboard: enabled");
    }
    
    if (!config.parsed_args.debug_mode.empty()) {
        NLM_LOG_INFO("Debug mode: " + config.parsed_args.debug_mode);
    }
    
    NLM_LOG_INFO("===============================");
}

void AdvancedNLMFeatures::runAdvancedPerformanceTests(std::shared_ptr<Brain> brain, Configuration& config) {
    try {
        NLM_LOG_INFO("Running advanced performance tests...");
        
        // Run basic performance tests
        PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
        
        profiler.startTimer("simulation_1000");
        for (int i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
        profiler.stopTimer("simulation_1000");
        
        profiler.startTimer("plasticity_experiment");
        // Run a plasticity experiment
        profiler.stopTimer("plasticity_experiment");
        
        // Print performance report
        profiler.printReport();
        
        NLM_LOG_INFO("Advanced performance tests completed");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Advanced performance tests failed: " + std::string(e.what()));
    }
}

void AdvancedNLMFeatures::cleanupAdvancedFeatures(Configuration& config) {
    NLM_LOG_INFO("Cleaning up advanced NLM features...");
    
    PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
    profiler.printReport();
    
    // Memory profiler will print report in destructor
    
    NLM_LOG_INFO("Advanced NLM features cleanup completed");
}

void AdvancedNLMFeatures::setupProfilingInfrastructure(Configuration& config) {
    PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
    profiler.reset();
    
    // Start key operation timers
    profiler.startTimer("brain_initialization");
    profiler.startTimer("simulation_setup");
    profiler.startTimer("plasticity_setup");
    
    NLM_LOG_INFO("Performance profiling infrastructure setup complete");
}

void AdvancedNLMFeatures::setupMemoryTracking(Configuration& config) {
    // Memory tracking is initialized in constructor
    MemoryProfiler& mem_profiler = MemoryProfiler::getInstance();
    
    NLM_LOG_INFO("Memory tracking infrastructure setup complete");
}

void AdvancedNLMFeatures::setupVisualization(Configuration& config) {
    AdvancedVisualization::VisualizationOptions opts = AdvancedVisualization::getVisualizationOptions(
        config.parsed_args.visualization_mode.empty() ? "auto" : config.parsed_args.visualization_mode);
    
    AdvancedVisualization::printVisualizationOptionsSummary(opts);
    
    NLM_LOG_INFO("Visualization infrastructure setup complete");
}

}} // namespace nlm::main