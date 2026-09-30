#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Known configuration keys for validation
namespace ConfigKeys {
    // Core configuration
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    constexpr const char* RANDOM_SEED = "random_seed";
    constexpr const char* LOG_LEVEL = "log_level";
    
    // Plasticity configuration
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TAU = "stdp_tau";
    constexpr const char* HEBBIAN_LEARNING_RATE = "hebbian_learning_rate";
    constexpr const char* PLASTICITY_LEARNING_RATE = "plasticity_learning_rate";
    
    // Structural plasticity
    constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    
    // Neuromodulation
    constexpr const char* DOPAMINE_BASELINE = "dopamine_baseline";
    constexpr const char* CURIOUSITY_THRESHOLD = "curiosity_threshold";
    constexpr const char* NOVELTY_THRESHOLD = "novelty_threshold";
    
    // Reward and learning
    constexpr const char* REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    constexpr const char* REWARD_PREDICTION_ERROR_SCALE = "reward_prediction_error_scale";
    
    // Environment
    constexpr const char* ENVIRONMENT_NAME = "environment_name";
    constexpr const char* ENVIRONMENT_WIDTH = "environment_width";
    constexpr const char* ENVIRONMENT_HEIGHT = "environment_height";
    
    // Development
    constexpr const char* DEVELOPMENT_STAGE = "development_stage";
    constexpr const char* CRITICAL_PERIOD_END = "critical_period_end";
    constexpr const char* MATURATION_RATE = "maturation_rate";
    
    // Memory
    constexpr const char* WORKING_MEMORY_SIZE = "working_memory_size";
    constexpr const char* EPISODIC_MEMORY_CAPACITY = "episodic_memory_capacity";
    constexpr const char* EPISODIC_MEMORY_REPLAY_RATE = "episodic_memory_replay_rate";
    
    // Prediction
    constexpr const char* PREDICTION_HORIZON = "prediction_horizon";
    constexpr const char* PREDICTION_ERROR_WEIGHT = "prediction_error_weight";
    
    // Logging and debugging
    constexpr const char* LOG_TO_FILE = "log_to_file";
    constexpr const char* LOG_FILENAME = "log_filename";
    constexpr const char* MAX_SIMULATION_STEPS = "max_simulation_steps";
    constexpr const char* SIMULATION_TIME_LIMIT = "simulation_time_limit";
    
    // Visualization
    constexpr const char* VISUALIZATION_ENABLED = "visualization_enabled";
    constexpr const char* VISUALIZATION_UPDATE_RATE = "visualization_update_rate";
    
    // Agent behavior
    constexpr const char* AGENT_EXPLORATION_RATE = "agent_exploration_rate";
    constexpr const char* AGENT_EXPERIENCE_REPLAY = "agent_experience_replay";
}

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool setValidated(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool setValidated(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool setValidated(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool setValidated(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    static bool validateKey(const std::string& key);
    static bool validateValue(const std::string& key, const ConfigValue& value);
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
    // Configuration defaults
    static std::optional<ConfigValue> getDefaultValue(const std::string& key);
    static std::string getDescription(const std::string& key);
    
    // Configuration types and ranges
    struct ValidationRules {
        std::string type;      // "int", "double", "bool", "string", "vector"
        double minValue;       // Minimum value (for numeric types)
        double maxValue;       // Maximum value (for numeric types)
        size_t minSize;        // Minimum vector size (for vector types)
        size_t maxSize;        // Maximum vector size (for vector types)
        std::string pattern;   // Regex pattern (for string types)
        std::string units;     // Physical units (e.g., "Hz", "ms", "%");
    };
    
    static ValidationRules getValidationRules(const std::string& key);
    
    // Configuration sources
    static bool validateSource(const std::string& key, ConfigSource source);
    static bool canOverride(const std::string& key, ConfigSource newSource);
    
    // Configuration history and tracking
    std::vector<std::pair<std::string, ConfigSource>> getChangeHistory() const;
    size_t getChangeCount(const std::string& key) const;
    ConfigSource getLastSource(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
