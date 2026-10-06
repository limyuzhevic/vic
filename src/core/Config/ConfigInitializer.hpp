#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Configuration key constants - centralize all config keys to avoid typos
namespace ConfigKeys {
    // Basic simulation parameters
    constexpr const char* RANDOM_SEED = "random_seed";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    
    // Plasticity parameters
    constexpr const char* PLASTICITY_LEARNING_RATE = "plasticity_learning_rate";
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TAU = "stdp_tau"; // Note: config uses "stdp_time_constant"
    
    // Structural plasticity parameters
    constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    constexpr const char* DEVELOPMENT_SYNAPTOGENESIS_RATE = "development_synaptogenesis_rate";
    constexpr const char* DEVELOPMENT_PRUNING_RATE = "development_pruning_rate";
    
    // Neuromodulation parameters
    constexpr const char* DOPAMINE_BASELINE = "dopamine_baseline";
    constexpr const char* DOPAMINE_GAIN = "dopamine_gain";
    
    // Reward parameters
    constexpr const char* REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    constexpr const char* REWARD_BASELINE = "reward_baseline";
    
    // Environment parameters
    constexpr const char* ENVIRONMENT_NAME = "environment_name";
    constexpr const char* ENVIRONMENT_WIDTH = "environment_width";
    constexpr const char* ENVIRONMENT_HEIGHT = "environment_height";
    
    // Logging parameters
    constexpr const char* LOG_LEVEL = "log_level";
    constexpr const char* LOG_TO_FILE = "log_to_file";
    constexpr const char* LOG_FILENAME = "log_filename";
    
    // Simulation parameters
    constexpr const char* MAX_SIMULATION_STEPS = "max_simulation_steps";
    constexpr const char* SIMULATION_TIME_LIMIT = "simulation_time_limit";
    
    // Visualization parameters
    constexpr const char* VISUALIZATION_ENABLED = "visualization_enabled";
    constexpr const char* VISUALIZATION_UPDATE_RATE = "visualization_update_rate";
    
    // Memory parameters
    constexpr const char* WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    constexpr const char* EPISODIC_MEMORY_CAPACITY = "episodic_memory_capacity";
    
    // Development parameters
    constexpr const char* DEVELOPMENT_ENABLE = "development_enable";
    constexpr const char* CRITICAL_PERIOD_DURATION = "critical_period_duration";
}

// Default configuration values - used as initialization
struct DefaultConfig {
    // Basic simulation parameters
    static constexpr int64_t RANDOM_SEED = 42;
    static constexpr double SIMULATION_TIMESTEP = 0.001;
    static constexpr int64_t NEURON_COUNT = 1000;
    static constexpr int64_t REGION_COUNT = 2;
    static constexpr float CONNECTION_PROBABILITY = 0.1f;
    
    // Plasticity parameters
    static constexpr float PLASTICITY_LEARNING_RATE = 0.01f;
    static constexpr float STDP_LTP_WEIGHT = 0.01f;
    static constexpr float STDP_LTD_WEIGHT = 0.012f;
    static constexpr float STDP_TAU = 20.0f;
    
    // Structural plasticity parameters
    static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float PRUNING_RATE = 0.00001f;
    static constexpr float DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f;
    static constexpr float DEVELOPMENT_PRUNING_RATE = 0.0001f;
    
    // Neuromodulation parameters
    static constexpr float DOPAMINE_BASELINE = 0.1f;
    static constexpr float DOPAMINE_GAIN = 1.0f;
    
    // Reward parameters
    static constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;
    static constexpr float REWARD_BASELINE = 0.0f;
    
    // Environment parameters
    static constexpr const char* ENVIRONMENT_NAME = "GridWorld";
    static constexpr int64_t ENVIRONMENT_WIDTH = 10;
    static constexpr int64_t ENVIRONMENT_HEIGHT = 10;
    
    // Logging parameters
    static constexpr const char* LOG_LEVEL = "INFO";
    static constexpr bool LOG_TO_FILE = false;
    static constexpr const char* LOG_FILENAME = "nlm.log";
    
    // Simulation parameters
    static constexpr int64_t MAX_SIMULATION_STEPS = 10000;
    static constexpr double SIMULATION_TIME_LIMIT = 0.0;
    
    // Visualization parameters
    static constexpr bool VISUALIZATION_ENABLED = false;
    static constexpr float VISUALIZATION_UPDATE_RATE = 30.0f;
    
    // Memory parameters
    static constexpr int64_t WORKING_MEMORY_CAPACITY = 1000;
    static constexpr int64_t EPISODIC_MEMORY_CAPACITY = 100;
    
    // Development parameters
    static constexpr bool DEVELOPMENT_ENABLE = false;
    static constexpr double CRITICAL_PERIOD_DURATION = 100.0;
};

// Configuration schema - defines all valid configuration keys and their types
struct ConfigSchema {
    struct Entry {
        std::string key;
        std::string type; // "int", "int64", "double", "float", "bool", "string"
        std::string description;
        std::string defaultValue;
        bool required;
    };
    
    static std::vector<Entry> getSchema() {
        return {
            // Basic simulation parameters
            {ConfigKeys::RANDOM_SEED, "int64", "Random seed for reproducible experiments", "42", false},
            {ConfigKeys::SIMULATION_TIMESTEP, "double", "Simulation timestep in seconds", "0.001", false},
            {ConfigKeys::NEURON_COUNT, "int64", "Number of neurons in the brain", "1000", false},
            {ConfigKeys::REGION_COUNT, "int64", "Number of brain regions", "2", false},
            {ConfigKeys::CONNECTION_PROBABILITY, "float", "Connection probability for random connectivity", "0.1", false},
            
            // Plasticity parameters
            {ConfigKeys::PLASTICITY_LEARNING_RATE, "float", "Plasticity learning rate", "0.01", false},
            {ConfigKeys::STDP_LTP_WEIGHT, "float", "STDP LTP weight", "0.01", false},
            {ConfigKeys::STDP_LTD_WEIGHT, "float", "STDP LTD weight", "0.012", false},
            {ConfigKeys::STDP_TAU, "float", "STDP time constant", "20.0", false},
            
            // Structural plasticity parameters
            {ConfigKeys::SYNAPTOGENESIS_RATE, "float", "Synaptogenesis rate", "0.0001", false},
            {ConfigKeys::PRUNING_RATE, "float", "Pruning rate", "0.00001", false},
            {ConfigKeys::DEVELOPMENT_SYNAPTOGENESIS_RATE, "float", "Development synaptogenesis rate", "0.001", false},
            {ConfigKeys::DEVELOPMENT_PRUNING_RATE, "float", "Development pruning rate", "0.0001", false},
            
            // Neuromodulation parameters
            {ConfigKeys::DOPAMINE_BASELINE, "float", "Dopamine baseline level", "0.1", false},
            {ConfigKeys::DOPAMINE_GAIN, "float", "Dopamine gain factor", "1.0", false},
            
            // Reward parameters
            {ConfigKeys::REWARD_DISCOUNT_FACTOR, "float", "Reward discount factor", "0.99", false},
            {ConfigKeys::REWARD_BASELINE, "float", "Reward baseline", "0.0", false},
            
            // Environment parameters
            {ConfigKeys::ENVIRONMENT_NAME, "string", "Environment name", "GridWorld", false},
            {ConfigKeys::ENVIRONMENT_WIDTH, "int64", "Environment width", "10", false},
            {ConfigKeys::ENVIRONMENT_HEIGHT, "int64", "Environment height", "10", false},
            
            // Logging parameters
            {ConfigKeys::LOG_LEVEL, "string", "Logging level (DEBUG, INFO, WARN, ERROR)", "INFO", false},
            {ConfigKeys::LOG_TO_FILE, "bool", "Log to file instead of console", "false", false},
            {ConfigKeys::LOG_FILENAME, "string", "Log filename", "nlm.log", false},
            
            // Simulation parameters
            {ConfigKeys::MAX_SIMULATION_STEPS, "int64", "Maximum simulation steps", "10000", false},
            {ConfigKeys::SIMULATION_TIME_LIMIT, "double", "Simulation time limit in seconds", "0.0", false},
            
            // Visualization parameters
            {ConfigKeys::VISUALIZATION_ENABLED, "bool", "Enable visualization", "false", false},
            {ConfigKeys::VISUALIZATION_UPDATE_RATE, "float", "Visualization update rate (Hz)", "30.0", false},
            
            // Memory parameters
            {ConfigKeys::WORKING_MEMORY_CAPACITY, "int64", "Working memory capacity", "1000", false},
            {ConfigKeys::EPISODIC_MEMORY_CAPACITY, "int64", "Episodic memory capacity", "100", false},
            
            // Development parameters
            {ConfigKeys::DEVELOPMENT_ENABLE, "bool", "Enable developmental stages", "false", false},
            {ConfigKeys::CRITICAL_PERIOD_DURATION, "double", "Critical period duration in steps", "100.0", false},
        };
    }
};

// Helper function to initialize config with defaults
inline void initializeConfigWithDefaults(std::shared_ptr<nlm::Config> config) {
    if (!config) return;
    
    // Set all defaults
    config->set(ConfigKeys::RANDOM_SEED, DefaultConfig::RANDOM_SEED);
    config->set(ConfigKeys::SIMULATION_TIMESTEP, DefaultConfig::SIMULATION_TIMESTEP);
    config->set(ConfigKeys::NEURON_COUNT, DefaultConfig::NEURON_COUNT);
    config->set(ConfigKeys::REGION_COUNT, DefaultConfig::REGION_COUNT);
    config->set(ConfigKeys::CONNECTION_PROBABILITY, DefaultConfig::CONNECTION_PROBABILITY);
    
    config->set(ConfigKeys::PLASTICITY_LEARNING_RATE, DefaultConfig::PLASTICITY_LEARNING_RATE);
    config->set(ConfigKeys::STDP_LTP_WEIGHT, DefaultConfig::STDP_LTP_WEIGHT);
    config->set(ConfigKeys::STDP_LTD_WEIGHT, DefaultConfig::STDP_LTD_WEIGHT);
    config->set(ConfigKeys::STDP_TAU, DefaultConfig::STDP_TAU);
    
    config->set(ConfigKeys::SYNAPTOGENESIS_RATE, DefaultConfig::SYNAPTOGENESIS_RATE);
    config->set(ConfigKeys::PRUNING_RATE, DefaultConfig::PRUNING_RATE);
    config->set(ConfigKeys::DEVELOPMENT_SYNAPTOGENESIS_RATE, DefaultConfig::DEVELOPMENT_SYNAPTOGENESIS_RATE);
    config->set(ConfigKeys::DEVELOPMENT_PRUNING_RATE, DefaultConfig::DEVELOPMENT_PRUNING_RATE);
    
    config->set(ConfigKeys::DOPAMINE_BASELINE, DefaultConfig::DOPAMINE_BASELINE);
    config->set(ConfigKeys::DOPAMINE_GAIN, DefaultConfig::DOPAMINE_GAIN);
    
    config->set(ConfigKeys::REWARD_DISCOUNT_FACTOR, DefaultConfig::REWARD_DISCOUNT_FACTOR);
    config->set(ConfigKeys::REWARD_BASELINE, DefaultConfig::REWARD_BASELINE);
    
    config->set(ConfigKeys::ENVIRONMENT_NAME, DefaultConfig::ENVIRONMENT_NAME);
    config->set(ConfigKeys::ENVIRONMENT_WIDTH, DefaultConfig::ENVIRONMENT_WIDTH);
    config->set(ConfigKeys::ENVIRONMENT_HEIGHT, DefaultConfig::ENVIRONMENT_HEIGHT);
    
    config->set(ConfigKeys::LOG_LEVEL, std::string(DefaultConfig::LOG_LEVEL));
    config->set(ConfigKeys::LOG_TO_FILE, DefaultConfig::LOG_TO_FILE);
    config->set(ConfigKeys::LOG_FILENAME, std::string(DefaultConfig::LOG_FILENAME));
    
    config->set(ConfigKeys::MAX_SIMULATION_STEPS, DefaultConfig::MAX_SIMULATION_STEPS);
    config->set(ConfigKeys::SIMULATION_TIME_LIMIT, DefaultConfig::SIMULATION_TIME_LIMIT);
    
    config->set(ConfigKeys::VISUALIZATION_ENABLED, DefaultConfig::VISUALIZATION_ENABLED);
    config->set(ConfigKeys::VISUALIZATION_UPDATE_RATE, DefaultConfig::VISUALIZATION_UPDATE_RATE);
    
    config->set(ConfigKeys::WORKING_MEMORY_CAPACITY, DefaultConfig::WORKING_MEMORY_CAPACITY);
    config->set(ConfigKeys::EPISODIC_MEMORY_CAPACITY, DefaultConfig::EPISODIC_MEMORY_CAPACITY);
    
    config->set(ConfigKeys::DEVELOPMENT_ENABLE, DefaultConfig::DEVELOPMENT_ENABLE);
    config->set(ConfigKeys::CRITICAL_PERIOD_DURATION, DefaultConfig::CRITICAL_PERIOD_DURATION);
}

} // namespace nlm