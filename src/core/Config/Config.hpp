#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>
#include <nlohmann/json.hpp>

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

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigError("Config parse error: " + message) {}
};

class ConfigTypeError : public ConfigError {
public:
    explicit ConfigTypeError(const std::string& message) : ConfigError("Config type error: " + message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Config validation error: " + message) {}
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    ConfigValue defaultValue;
    std::string type;  // "int", "int64", "double", "bool", "string", "int_array", "double_array", "string_array"
    std::string description;
    std::optional<std::pair<double, double>> range;  // For numeric types
    std::optional<size_t> minLength;  // For string arrays
    bool required;  // Whether this is a required parameter
    
    ConfigSchemaEntry() : key(), defaultValue(), type(), description(), range(), minLength(), required(false) {}
    
    ConfigSchemaEntry(const std::string& k, const ConfigValue& dv, const std::string& t, 
                     const std::string& desc, bool req = false,
                     const std::optional<std::pair<double, double>>& r = std::nullopt,
                     const std::optional<size_t>& ml = std::nullopt)
        : key(k), defaultValue(dv), type(t), description(desc), range(r), minLength(ml), required(req) {}
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
    
    // Initialize with schema
    static void initializeSchema();
    
    // Get schema for a specific key
    static std::optional<ConfigSchemaEntry> getSchema(const std::string& key);
    
    // Get all schema keys
    static std::vector<std::string> getSchemaKeys();
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON/YAML string
    bool loadFromJson(const std::string& jsonStr);
    bool loadFromYaml(const std::string& yamlStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with description (for schema)
    void setWithDescription(const std::string& key, const ConfigValue& value, 
                            const std::string& description, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries with source info
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration schema summary
    static std::string getSchemaSummary();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema storage
    static std::map<std::string, ConfigSchemaEntry> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& json);
    static std::string detectFileFormat(const std::string& filepath);
    
    // Validation
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value, const ConfigSchemaEntry& schemaEntry);
    static bool validateRange(const ConfigValue& value, const std::optional<std::pair<double, double>>& range);
    static bool validateStringArrayLength(const ConfigValue& value, const std::optional<size_t>& minLength);
    
    // Internal validation setter
    void setValidated(const std::string& key, const ConfigValue& value, ConfigSource source);
};

// Configuration schema initialization function
void initializeDefaultConfigSchema();

// Configuration option declarations
struct CoreSimulationParameters {
    static const std::string timestep;
    static const std::string neuron_count;
    static const std::string region_count;
};

struct ConnectivityParameters {
    static const std::string connection_probability;
    static const std::string connection_strength;
};

struct PlasticityParameters {
    static const std::string stdp_ltp_weight;
    static const std::string stdp_ltd_weight;
    static const std::string stdp_tau;
    static const std::string hebbian_learning_rate;
};

struct StructuralPlasticityParameters {
    static const std::string synaptogenesis_rate;
    static const std::string pruning_rate;
    static const std::string max_synapses;
};

struct MemoryParameters {
    static const std::string working_memory_capacity;
    static const std::string episodic_memory_max_episodes;
    static const std::string associative_memory_capacity;
};

struct NeuromodulationParameters {
    static const std::string dopamine_baseline;
    static const std::string curiosity_strength;
    static const std::string novelty_threshold;
    static const std::string prediction_error_decay;
};

struct DevelopmentParameters {
    static const std::string developmental_stage_duration;
    static const std::string plasticity_decay;
};

struct PerformanceTuning {
    static const std::string max_steps;
    static const std::string checkpoint_interval;
    static const std::string memory_limit;
    static const std::string max_memory_mb;
    static const std::string thread_count;
};

struct AdvancedDebugging {
    static const std::string enable_debug_logging;
    static const std::string enable_spike_tracing;
    static const std::string enable_visualization;
    static const std::string trace_spikes;
    static const std::string show_synaptic_graphics;
};

struct RandomSeedOptions {
    static const std::string random_seed;
    static const std::string deterministic;
};

struct FileOptions {
    static const std::string config_file;
    static const std::string output_file;
    static const std::string checkpoint_dir;
};

struct LoggingOptions {
    static const std::string log_level;
    static const std::string log_file;
    static const std::string verbose;
};

struct ExperimentOptions {
    static const std::string experiment_mode;
    static const std::string test_number;
    static const std::string profile;
    static const std::string profile_interval;
    static const std::string fast_mode;
};

} // namespace nlm
