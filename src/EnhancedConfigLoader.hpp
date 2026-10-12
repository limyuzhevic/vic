#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <setenv.h>
#include <cstdlib>

namespace nlm {

class EnhancedConfigLoader {
public:
    // Load configuration from file and additional files
    bool loadConfig(Config& config, const std::string& configFile = "configs/default.cfg",
                   const std::vector<std::string>& extraConfigFiles = {});
    
    // Load configuration from environment variables
    bool loadFromEnvironment(Config& config);
    
    // Validate configuration
    bool validateConfig(const Config& config) const;
    
    // Print configuration summary
    void printConfigSummary(const Config& config) const;
    
    // Add configuration file
    bool addConfigFile(const std::string& filename);
    
    // Get all loaded parameter names
    std::vector<std::string> getAllParameterNames() const;
    
    // Get number of loaded files
    size_t getFileCount() const;
    
    // Get total number of parameters
    size_t getTotalParameters() const;
    
    // Get parameter by name with default
    template<typename T>
    T getOr(const std::string& name, T defaultValue) const;
    
    // Type conversion helpers
    template<typename T>
    static T convertStringToValue(const std::string& str);
    
    // Parameter management
    void addParameter(const std::string& name, const std::string& value);
    bool hasParameter(const std::string& name) const;
    std::string getParameter(const std::string& name) const;

private:
    // File loading methods
    bool loadFromFile(Config& config, const std::string& filename);
    bool parseConfigFile(Config& config, const std::string& filename);
    
    // Environment variable parsing
    bool parseEnvironmentVariables(Config& config);
    
    // Parameter merging
    void mergeConfig(Config& target, const Config& source);
    
    // Validation
    bool validateParameterTypes(const Config& config) const;
    bool validateInterDependencies(const Config& config) const;
    
    // File processing
    std::map<std::string, std::string> loadKeyValuePairs(const std::string& content);
    std::string trimString(const std::string& str);
    
    // Environment variable mapping
    std::map<std::string, std::string> environmentPrefixes = {
        {"NLM_RANDOM_SEED", "random_seed"},
        {"NLM_NEURON_COUNT", "neuron_count"},
        {"NLM_REGION_COUNT", "region_count"},
        {"NLM_CONNECTION_PROBABILITY", "connection_probability"},
        {"NLM_SIMULATION_TIMESTEP", "simulation_timestep"},
        {"NLM_PLATIVITY_LEARNING_RATE", "plasticity_learning_rate"},
        {"NLM_STDP_LTP_WEIGHT", "stdp_ltp_weight"},
        {"NLM_STDP_LTD_WEIGHT", "stdp_ltd_weight"},
        {"NLM_SYNAPTOGENESIS_RATE", "synaptogenesis_rate"},
        {"NLM_PRUNING_RATE", "pruning_rate"},
        {"NLM_DOPAMINE_BASELINE", "dopamine_baseline"},
        {"NLM_REWARD_DISCOUNT_FACTOR", "reward_discount_factor"},
        {"NLM_ENVIRONMENT_NAME", "environment_name"},
        {"NLM_LOG_LEVEL", "log_level"},
        {"NLM_MAX_SIMULATION_STEPS", "max_simulation_steps"},
        {"NLM_VISUALIZATION_ENABLED", "visualization_enabled"},
        // Performance optimization options
        {"NLM_ENABLE_EVENT_DRIVEN", "enable_event_driven"},
        {"NLM_ENABLE_MULTITHREADING", "enable_multithreading"},
        {"NLM_ENABLE_SIMD", "enable_simd"},
        {"NLM_ENABLE_SPARSE_CONNECTIVITY", "enable_sparse_connectivity"},
        {"NLM_THREAD_COUNT", "thread_count"},
        {"NLM_ENABLE_PERFORMANCE_MONITORING", "enable_performance_monitoring"},
        {"NLM_NEURON_POOL_SIZE", "neuron_pool_size"},
        {"NLM_SYNAPSE_POOL_SIZE", "synapse_pool_size"},
        {"NLM_ENABLE_MEMORY_STATS", "enable_memory_stats"},
        {"NLM_EVENT_QUEUE_CAPACITY", "event_queue_capacity"},
        {"NLM_ENABLE_ZERO_COPY", "enable_zero_copy"},
        {"NLM_ENABLE_WORK_STEALING", "enable_work_steal"},
        {"NLM_ENABLE_CACHE_ALIGNMENT", "enable_cache_alignment"},
        {"NLM_ENABLE_EVENT_RECYCLING", "enable_event_recycling"},
        {"NLM_COMPRESSION_ENABLED", "compression_enabled"},
        {"NLM_COMPRESSION_LEVEL", "compression_level"},
        {"NLM_OPTIMIZE_FOR_SPEED", "optimize_for_speed"},
        {"NLM_MAX_EVENTS_PER_STEP", "max_events_per_step"}
    };
    
    // Loaded configuration files
    std::vector<std::string> loadedFiles;
    
    // Parsed parameters
    std::map<std::string, std::string> parameters;
};

} // namespace nlm