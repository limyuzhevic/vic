#pragma once

#include <string>
#include <map>
#include <variant>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Configuration types
using ConfigValue = std::variant<
    int64_t,
    float,
    double,
    std::string,
    bool,
    std::vector<int64_t>,
    std::vector<float>,
    std::vector<double>,
    std::vector<std::string>
>;

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic
};

class Config {
public:
    Config();
    ~Config();
    
    // Set configuration value
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Programmatic);
    
    // Get configuration value with type conversion
    template<typename T>
    bool get(const std::string& key, T& value) const;
    
    // GetOr: Get value with default fallback
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Load from file
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get configuration keys
    std::vector<std::string> getKeys() const;
    
    // Configuration schema validation
    bool validate() const;
    
    // Get configuration source
    ConfigSource getSource(const std::string& key) const;
    
    // Clear configuration
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type;           // "int64", "float", "double", "string", "bool", "vector"
    ConfigSource requiredSource;  // Minimum source allowed
    ConfigValue defaultValue;
    std::string description;
    
    bool (ConfigSchema::*validateFn)(const ConfigValue& value) const = nullptr;
};

// Global configuration instance
extern std::shared_ptr<Config> globalConfig;

// Helper functions for common configuration keys
namespace ConfigKeys {
    // Core simulation parameters
    static constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    static constexpr const char* NEURON_COUNT = "neuron_count";
    static constexpr const char* REGION_COUNT = "region_count";
    static constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    static constexpr const char* RANDOM_SEED = "random_seed";
    
    // Plasticity parameters
    static constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    static constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    static constexpr const char* STDP_TAU = "stdp_tau";
    static constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    static constexpr const char* PRUNING_RATE = "pruning_rate";
    
    // Memory parameters
    static constexpr const char* WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    static constexpr const char* EPISODIC_MEMORY_MAX_EPISODES = "episodic_memory_max_episodes";
    static constexpr const char* ASSOC_MEMORY_PATTERN_COUNT = "assoc_memory_pattern_count";
    
    // Neuromodulation parameters
    static constexpr const char* DOPAMINE_INITIAL_LEVEL = "dopamine_initial_level";
    static constexpr const char* CURIOUSY_INITIAL_LEVEL = "curiosity_initial_level";
    static constexpr const char* NOVELTY_INITIAL_LEVEL = "novelty_initial_level";
    
    // Development parameters
    static constexpr const char* DEVELOPMENT_ENABLED = "development_enabled";
    static constexpr const char* DEVELOPMENT_STAGES = "development_stages";
    
    // Checkpoint parameters
    static constexpr const char* CHECKPOINT_DIR = "checkpoint_dir";
    static constexpr const char* CHECKPOINT_FREQUENCY = "checkpoint_frequency";
    static constexpr const char* CHECKPOINT_COMPRESSION = "checkpoint_compression";
    
    // Experiment parameters
    static constexpr const char* EXPERIMENT_MAX_STEPS = "experiment_max_steps";
    static constexpr const char* EXPERIMENT_REWARD_FUNCTION = "experiment_reward_function";
};

// Configuration initialization
void initializeConfig();
Config& getConfig();

// Configuration validation
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add validation rule
    void addValidation(const std::string& key, bool (ConfigValidator::*validator)(const ConfigValue&)) const;
    
    // Validate configuration
    bool validate(const Config& config) const;
    
    // Get validation errors
    std::vector<std::string> getErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm