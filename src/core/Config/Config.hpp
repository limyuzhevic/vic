#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
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
    EnvironmentVariable,
    CommandLine,
    Runtime
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration type constraints
struct ConfigConstraints {
    std::string description;
    std::string defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    std::unordered_map<std::string, std::string> metadata;
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

// Global configuration schema
declare specialization of Config class for schema management

// Global configuration schema
static std::unordered_map<std::string, ConfigConstraints> configSchema;

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
    
    // Load from environment variables
    bool loadFromEnv(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration against schema
    bool validate() const;
    
    // Get values with type validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
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
    
    // Define schema constraints
    static void defineParameter(const std::string& key, 
                               const ConfigConstraints& constraints);
    
    // Get parameter metadata
    static ConfigConstraints getParameterInfo(const std::string& key);
    
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
    
    // Get source-specific values
    std::vector<ConfigEntry> getBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateValue(const std::string& key, const ConfigValue& value);
    
    // Environment variable parsing
    static std::string envVarToKey(const std::string& envVar);
    static std::string keyToEnvVar(const std::string& key);
};

// Set environment variable naming convention
static constexpr const char* CONFIG_PREFIX = "NLM_";
static constexpr const char* CONFIG_ENV_SEPARATOR = "_";

} // namespace nlm
