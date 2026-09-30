// Advanced configuration parameters for NLM
// Extends the base configuration with advanced neural and system parameters

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

// Advanced neural network configuration parameters
struct AdvancedNeuralParameters {
    // Neuron parameters
    double neuron_threshold;
    double neuron_rest_potential;
    double neuron_membrane_time_constant;
    double neuron_spike_amplitude;
    
    // Synapse parameters
    double synapse_weight_initial;
    double synapse_weight_max;
    double synapse_weight_min;
    double synapse_delay_min;
    double synapse_delay_max;
    double synapse_decay_rate;
    
    // Plasticity parameters
    double stdp_learning_rate LTP;
    double stdp_learning_rate LTD;
    double stdp_time_constant;
    double hebbian_learning_rate;
    
    // Network architecture
    double connectivity_probability;
    size_t max_synapses_per_neuron;
    double excitation_inhibition_ratio;
    
    AdvancedNeuralParameters() : neuron_threshold(20.0), neuron_rest_potential(-70.0),
                               neuron_membrane_time_constant(10.0), neuron_spike_amplitude(40.0),
                               synapse_weight_initial(0.5), synapse_weight_max(1.0), synapse_weight_min(0.0),
                               synapse_delay_min(0.0), synapse_delay_max(5.0), synapse_decay_rate(0.95),
                               stdp_learning_rate LTP(0.01), stdp_learning_rate LTD(0.012),
                               stdp_time_constant(20.0), hebbian_learning_rate(0.001),
                               connectivity_probability(0.15), max_synapses_per_neuron(100),
                               excitation_inhibition_ratio(2.0) {}
};

// Performance optimization parameters
struct PerformanceParameters {
    size_t num_threads;
    bool enable_simd;
    std::string performance_mode;  // {"balanced", "speed", "power", "memory"}
    size_t checkpoint_interval;
    size_t memory_pool_size;
    bool enable_profiling;
    std::string benchmark_mode;
    double target_fps;
    size_t max_memory_usage_mb;
    
    PerformanceParameters() : num_threads(std::max(1u, std::thread::hardware_concurrency())),
                              enable_simd(true), performance_mode("balanced"),
                              checkpoint_interval(100), memory_pool_size(1024 * 1024),  // 1MB
                              enable_profiling(false), benchmark_mode("off"),
                              target_fps(30.0), max_memory_usage_mb(1024) {}  // 1GB
};

// Debugging and logging parameters
struct DebugParameters {
    int debug_level;  // 0 = none, 1 = errors, 2 = warnings, 3 = info, 4 = verbose, 5 = debug
    std::vector<NeuronId> debug_neuron_ids;
    std::vector<SynapseId> debug_synapse_ids;
    bool enable_plasticity_debug;
    bool enable_validation_debug;
    bool enable_trace_events;
    bool enable_execution_profiling;
    std::string trace_output_file;
    std::string profile_output_file;
    std::string debug_log_file;
    
    DebugParameters() : debug_level(0), enable_plasticity_debug(false),
                       enable_validation_debug(false), enable_trace_events(false),
                       enable_execution_profiling(false) {}
};

// Experiment configuration parameters
struct ExperimentParameters {
    std::string experiment_mode;  // {"standard", "stress_test", "learning_curve", "ablation"}
    std::string experiment_name;
    double experiment_duration;  // in simulation time
    std::string experiment_objective;  // {"maximize", "minimize", "reach_target", "maintain"}
    std::map<std::string, std::string> experiment_parameters;
    std::string experiment_config_file;
    size_t observation_interval;
    std::string reward_mode;  // {"sparse", "dense", "shaped"}
    std::string environment_config_file;
    
    ExperimentParameters() : experiment_mode("standard"), experiment_duration(1000.0),
                            observation_interval(10), reward_mode("sparse") {}
};

// Configuration validation parameters
struct ValidationParameters {
    bool strict_mode;
    std::map<std::string, double> validation_thresholds;  // {"numeric", "memory", "performance"}
    bool skip_invalid_configurations;
    bool attempt_fix_invalid;
    bool validate_before_execution;
    size_t max_consecutive_errors;
    double tolerance_numeric;
    double tolerance_memory;
    double tolerance_performance;
    
    ValidationParameters() : strict_mode(false), skip_invalid_configurations(false),
                            attempt_fix_invalid(false), validate_before_execution(true),
                            max_consecutive_errors(3), tolerance_numeric(0.01),
                            tolerance_memory(0.1), tolerance_performance(0.05) {}
};

// Output formatting and logging parameters
struct OutputParameters {
    std::string output_format;  // {"text", "json", "csv", "binary"}
    std::string output_prefix;
    size_t output_interval;
    bool verbose_output;
    bool quiet_output;
    bool progress_reporting;
    std::string output_directory;
    bool enable_logging;
    std::string log_level;  // {"trace", "debug", "info", "warn", "error", "critical"}
    size_t max_log_file_size_mb;
    size_t log_file_rotation_count;
    bool log_to_console;
    bool log_to_file;
    
    OutputParameters() : output_format("text"), output_interval(100), verbose_output(false),
                        quiet_output(false), progress_reporting(true), enable_logging(true),
                        log_level("info"), max_log_file_size_mb(100), log_file_rotation_count(5),
                        log_to_console(true), log_to_file(true) {}
};

// Memory management parameters
struct MemoryParameters {
    std::string optimization_mode;  // {"standard", "aggressive", "conservative"}
    bool enable_memory_pool;
    size_t memory_pool_size;
    bool enable_virtual_memory;
    size_t mmap_threshold;
    size_t memory_limit_mb;
    bool enable_memory_logging;
    std::string memory_allocator;  // {"system", "pool", "tcmalloc", "jemalloc"}
    bool enable_numa_awareness;
    size_t numa_policy;
    
    MemoryParameters() : optimization_mode("standard"), enable_memory_pool(false),
                        memory_pool_size(64 * 1024 * 1024),  // 64MB
                        enable_virtual_memory(false), mmap_threshold(1024 * 1024),  // 1MB
                        memory_limit_mb(2048), enable_memory_logging(false),  // 2GB
                        memory_allocator("system"), enable_numa_awareness(false),
                        numa_policy(0) {}
};

// Checkpoint management parameters
struct CheckpointParameters {
    std::string checkpoint_file;
    bool save_checkpoint;
    size_t checkpoint_interval;
    std::string checkpoint_format;  // {"binary", "json", "custom"}
    std::string checkpoint_compression;  // {"none", "gzip", "lz4"}
    int checkpoint_compression_level;
    bool enable_checkpoint_validation;
    std::string checkpoint_encryption_key;
    bool enable_checkpoint_diff;
    size_t checkpoint_history_limit;
    
    CheckpointParameters() : save_checkpoint(false), checkpoint_interval(100),
                            checkpoint_format("binary"), checkpoint_compression("none"),
                            checkpoint_compression_level(6), enable_checkpoint_validation(true),
                            enable_checkpoint_diff(false), checkpoint_history_limit(10) {}
};

// Advanced configuration extension
// This class extends the base Config class with advanced configuration parameters
class ConfigAdvanced {
public:
    ConfigAdvanced();
    ~ConfigAdvanced();
    
    // Initialize from base configuration
    void initializeFromConfig(const Config& baseConfig);
    
    // Get advanced neural parameters
    const AdvancedNeuralParameters& getNeuralParameters() const { return neuralParams; }
    AdvancedNeuralParameters& getNeuralParameters() { return neuralParams; }
    
    // Get performance parameters
    const PerformanceParameters& getPerformanceParameters() const { return perfParams; }
    PerformanceParameters& getPerformanceParameters() { return perfParams; }
    
    // Get debug parameters
    const DebugParameters& getDebugParameters() const { return debugParams; }
    DebugParameters& getDebugParameters() { return debugParams; }
    
    // Get experiment parameters
    const ExperimentParameters& getExperimentParameters() const { return expParams; }
    ExperimentParameters& getExperimentParameters() { return expParams; }
    
    // Get validation parameters
    const ValidationParameters& getValidationParameters() const { return valParams; }
    ValidationParameters& getValidationParameters() { return valParams; }
    
    // Get output parameters
    const OutputParameters& getOutputParameters() const { return outParams; }
    OutputParameters& getOutputParameters() { return outParams; }
    
    // Get memory parameters
    const MemoryParameters& getMemoryParameters() const { return memParams; }
    MemoryParameters& getMemoryParameters() { return memParams; }
    
    // Get checkpoint parameters
    const CheckpointParameters& getCheckpointParameters() const { return cpParams; }
    CheckpointParameters& getCheckpointParameters() { return cpParams; }
    
    // Apply command line overrides
    void applyCommandLineOverrides(const std::map<std::string, std::string>& overrides);
    
    // Validate all parameters
    bool validateAllParameters() const;
    
    // Get all validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Update parameters from configuration file
    bool updateFromConfigFile(const std::string& filepath);
    
    // Save advanced parameters to configuration
    void saveToConfig(Config& config) const;
    
    // Load advanced parameters from configuration
    void loadFromConfig(const Config& config);
    
    // Get parameter value with type conversion
    template<typename T>
    T getParameter(const std::string& category, const std::string& name, const T& defaultValue) const;
    
    // Set parameter value
    template<typename T>
    void setParameter(const std::string& category, const std::string& name, const T& value);
    
    // Check if parameter exists
    bool hasParameter(const std::string& category, const std::string& name) const;
    
    // Get all parameter keys
    std::vector<std::string> getAllParameterKeys() const;
    
    // Clear all parameters
    void clear();
    
    // Get parameter summary
    std::string getSummary() const;
    
private:
    AdvancedNeuralParameters neuralParams;
    PerformanceParameters perfParams;
    DebugParameters debugParams;
    ExperimentParameters expParams;
    ValidationParameters valParams;
    OutputParameters outParams;
    MemoryParameters memParams;
    CheckpointParameters cpParams;
    
    mutable std::vector<std::string> validationErrors;
    
    // Validate neural parameters
    void validateNeuralParameters() const;
    
    // Validate performance parameters
    void validatePerformanceParameters() const;
    
    // Validate debug parameters
    void validateDebugParameters() const;
    
    // Validate experiment parameters
    void validateExperimentParameters() const;
    
    // Validate validation parameters
    void validateValidationParameters() const;
    
    // Validate output parameters
    void validateOutputParameters() const;
    
    // Validate memory parameters
    void validateMemoryParameters() const;
    
    // Validate checkpoint parameters
    void validateCheckpointParameters() const;
    
    // Helper function to set parameter from string
    template<typename T>
    void setParameterFromString(const std::string& category, const std::string& name, const std::string& value);
    
    // Helper function to get parameter type
    std::string getParameterType(const std::string& category, const std::string& name) const;
};

} // namespace nlm