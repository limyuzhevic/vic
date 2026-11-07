#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message) 
        : std::runtime_error("Configuration validation error: " + message) {}
};

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

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string description;
    std::optional<std::string> defaultValue;
    bool required;
    std::vector<std::string> allowedValues;
    std::pair<double, double> numericRange;
    bool isDeprecated;
};

// Standard configuration keys
namespace ConfigKeys {
    // Core simulation parameters
    const std::string RANDOM_SEED = "random_seed";
    const std::string SIMULATION_TIMESTEP = "simulation_timestep";
    const std::string NEURON_COUNT = "neuron_count";
    const std::string REGION_COUNT = "region_count";
    const std::string CONNECTION_PROBABILITY = "connection_probability";
    
    // STDP parameters
    const std::string STDP_LTP_WEIGHT = "stdp_ltp_weight";
    const std::string STDP_LTD_WEIGHT = "stdp_ltd_weight";
    const std::string STDP_TAU = "stdp_tau";
    
    // Structural plasticity
    const std::string SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    const std::string PRUNING_RATE = "pruning_rate";
    
    // Neuromodulation
    const std::string DOPAMINE_BASELINE = "dopamine_baseline";
    const std::string REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    
    // Environment
    const std::string ENVIRONMENT_NAME = "environment_name";
    const std::string ENVIRONMENT_WIDTH = "environment_width";
    const std::string ENVIRONMENT_HEIGHT = "environment_height";
    
    // Logging
    const std::string LOG_LEVEL = "log_level";
    const std::string LOG_TO_FILE = "log_to_file";
    const std::string LOG_FILENAME = "log_filename";
    
    // Performance
    const std::string MAX_SIMULATION_STEPS = "max_simulation_steps";
    const std::string SIMULATION_TIME_LIMIT = "simulation_time_limit";
    const std::string VISUALIZATION_ENABLED = "visualization_enabled";
    const std::string VISUALIZATION_UPDATE_RATE = "visualization_update_rate";
    
    // Memory and performance
    const std::string REPLAY_INTERVAL = "replay_interval";
    const std::string CONSOLIDATION_INTERVAL = "consolidation_interval";
    const std::string CHECKPOINT_DIR = "checkpoint_dir";
}

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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
