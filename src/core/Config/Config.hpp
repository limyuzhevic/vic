#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace nlm {

// Configuration exception hierarchy
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public ConfigException {
public:
    explicit ConfigFileError(const std::string& message) : ConfigException("Config file error: " + message) {}
};

class ConfigValidationError : public ConfigException {
public:
    explicit ConfigValidationError(const std::string& key, const std::string& message) 
        : ConfigException("Config validation error for key \"" + key + "\": " + message) {}
};

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

// Configuration validation rule
using ValidationRule = std::function<bool(const ConfigValue&)>;

// Configuration metadata for documentation and validation
struct ConfigMetadata {
    std::string description;
    std::string defaultValue;
    std::string type;  // "int", "float", "string", "bool", "list"
    bool required;
    ValidationRule validator;
    std::unordered_map<std::string, std::string> constraints;
    
    ConfigMetadata() : description(""), defaultValue(""), type("string"), 
                      required(false), validator(nullptr) {}
};

// Predefined configuration templates
struct ConfigTemplates {
    static const std::unordered_map<std::string, std::string> DEFAULT;
    static const std::unordered_map<std::string, std::string> DEVELOPMENT;
    static const std::unordered_map<std::string, std::string> PRODUCTION;
    static const std::unordered_map<std::string, std::string> MINIMAL;
};

struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigMetadata metadata;
    
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
    
    // Configuration templates
    static bool loadFromTemplate(Config& config, const std::string& templateName);
    static bool applyDefaults(Config& config);
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool strict = false);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key, bool validate = true) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, bool validate = true) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validate(const std::string& key) const;
    bool validateAll() const;
    
    // Configuration information
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysWithType(const std::string& type) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string getDocumentation() const;
    
    // Configuration metadata management
    void registerConfig(const std::string& key, const ConfigMetadata& metadata);
    ConfigMetadata getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidType(const ConfigValue& value, const std::string& type);
    static std::optional<ConfigValue> parseValue(const std::string& str, const std::string& type);
};

}; // namespace nlm

// Implementation of configuration templates
namespace nlm {

// Default configuration template - basic working settings
const std::unordered_map<std::string, std::string> ConfigTemplates::DEFAULT = {
    {"random_seed", "42"},
    {"simulation_timestep", "0.001"},
    {"neuron_count", "500"},
    {"region_count", "1"},
    {"connection_probability", "0.1"},
    {"stdp_ltp_weight", "0.01"},
    {"stdp_ltd_weight", "0.012"},
    {"stdp_tau", "20.0"},
    {"synaptogenesis_rate", "0.0001"},
    {"pruning_rate", "0.00001"},
    {"replay_interval", "100"},
    {"consolidation_interval", "1000"}
};

// Development configuration template - more verbose, easier debugging
const std::unordered_map<std::string, std::string> ConfigTemplates::DEVELOPMENT = {
    {"random_seed", "123"},
    {"simulation_timestep", "0.001"},
    {"neuron_count", "1000"},
    {"region_count", "2"},
    {"connection_probability", "0.15"},
    {"stdp_ltp_weight", "0.02"},
    {"stdp_ltd_weight", "0.025"},
    {"stdp_tau", "50.0"},
    {"synaptogenesis_rate", "0.001"},
    {"pruning_rate", "0.0001"},
    {"replay_interval", "50"},
    {"consolidation_interval", "500"}
};

// Production configuration template - optimized for performance
const std::unordered_map<std::string, std::string> ConfigTemplates::PRODUCTION = {
    {"random_seed", "42"},
    {"simulation_timestep", "0.001"},
    {"neuron_count", "2000"},
    {"region_count", "1"},
    {"connection_probability", "0.05"},
    {"stdp_ltp_weight", "0.005"},
    {"stdp_ltd_weight", "0.006"},
    {"stdp_tau", "10.0"},
    {"synaptogenesis_rate", "0.00001"},
    {"pruning_rate", "0.000001"},
    {"replay_interval", "200"},
    {"consolidation_interval", "2000"}
};

// Minimal configuration template - fastest startup
const std::unordered_map<std::string, std::string> ConfigTemplates::MINIMAL = {
    {"random_seed", "42"},
    {"simulation_timestep", "0.001"},
    {"neuron_count", "100"},
    {"region_count", "1"},
    {"connection_probability", "0.2"},
    {"stdp_ltp_weight", "0.01"},
    {"stdp_ltd_weight", "0.012"},
    {"stdp_tau", "20.0"},
    {"synaptogenesis_rate", "0"},
    {"pruning_rate", "0"},
    {"replay_interval", "1000"},
    {"consolidation_interval", "10000"}
};

} // namespace nlm
