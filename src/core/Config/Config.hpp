#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <any>

namespace nlohmann {
    template<typename T, typename... Args>
    class basic_json;
    using json = basic_json<nlohmann::detail::default_content_handler>;
}

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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
    Environment,
    Runtime
};

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    std::string value;
    ConfigSource source;
    
    ConfigValidationError(const std::string& k, const std::string& msg, 
                         const std::string& val, ConfigSource src)
        : key(k), message(msg), value(val), source(src) {}
};

// Configuration type checking
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    VectorInt,
    VectorDouble,
    VectorString,
    Any
};

// Validation rule
struct ValidationRule {
    std::string key;
    ConfigType type;
    std::string description;
    bool required = false;
    std::any defaultValue = std::nullopt;
    std::function<bool(const std::any&)> validator;
    
    ValidationRule(const std::string& k, ConfigType t, const std::string& desc = "")
        : key(k), type(t), description(desc) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a type rule
    ConfigSchema& addRule(const ValidationRule& rule);
    
    // Add a type rule with shorthand
    ConfigSchema& addRule(const std::string& key, ConfigType type, 
                         const std::string& description = "",
                         bool required = false);
    
    // Add range validation for numeric types
    ConfigSchema& addRange(const std::string& key, double min, double max,
                         const std::string& description = "");
    
    // Add enum validation
    ConfigSchema& addEnum(const std::string& key, 
                         const std::vector<std::string>& values,
                         const std::string& description = "");
    
    // Validate configuration
    std::vector<ConfigValidationError> validate(const std::map<std::string, ConfigValue>& config) const;
    
    // Get all keys in schema
    std::vector<std::string> getKeys() const;
    
    // Check if key exists in schema
    bool hasKey(const std::string& key) const;
    
    // Get rule for key
    const ValidationRule* getRule(const std::string& key) const;
    
private:
    std::vector<ValidationRule> rules_;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries = 0;
    std::map<ConfigSource, size_t> entriesBySource;
    std::map<std::string, ConfigSource> keySources;
    double memoryUsage = 0.0;
    
    std::string toString() const;
};

// Hot-reload callback
using ConfigReloadCallback = std::function<void(const std::map<std::string, ConfigValue>& oldConfig, 
                                               const std::map<std::string, ConfigValue>& newConfig)>;

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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Support for nested keys (e.g., "brain.neuron_count")
    std::string getBaseKey() const {
        size_t dotPos = key.find('.');
        return (dotPos != std::string::npos) ? key.substr(0, dotPos) : key;
    }
    
    std::string getNestedKey() const {
        size_t dotPos = key.find('.');
        return (dotPos != std::string::npos) ? key.substr(dotPos + 1) : "";
    }
    
    bool isNested() const {
        return key.find('.') != std::string::npos;
    }
};

// Configuration entry with validation support
struct ConfigEntryWithValidation {
    ConfigEntry entry;
    bool isValid = true;
    std::vector<ConfigValidationError> errors;
    
    ConfigEntryWithValidation(const ConfigEntry& e) : entry(e) {}
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
    
    // Load from file (JSON/YAML/key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_set<std::string>& envVars = {});
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get values with source information
    std::optional<std::pair<ConfigValue, ConfigSource>> getWithSource(const std::string& key) const;
    
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
    
    // Get all keys (including nested)
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    
    // Validate current configuration
    std::vector<ConfigValidationError> validate() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Register callback for hot-reload
    void registerReloadCallback(const ConfigReloadCallback& callback);
    
    // Set override precedence order
    void setOverridePrecedence(const std::vector<ConfigSource>& precedence);
    
    // Get precedence order
    std::vector<ConfigSource> getOverridePrecedence() const;
    
    // Set environment variable prefix
    void setEnvPrefix(const std::string& prefix);
    
    // Get environment variable prefix
    std::string getEnvPrefix() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static ConfigValue parseConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    
    // Key handling helpers
    static std::string normalizeKey(const std::string& key);
    static std::map<std::string, ConfigValue> flattenNestedConfig(const std::map<std::string, ConfigValue>& nested);
    static std::map<std::string, ConfigValue> expandNestedConfig(const std::map<std::string, ConfigValue>& flat);
    
    // File parsing helpers
    static bool parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& config);
    static bool parseCfgFile(const std::string& filepath, std::map<std::string, ConfigValue>& config);
    
    // Environment variable helpers
    static std::string getEnvVarName(const std::string& key, const std::string& prefix);
    static bool parseEnvironmentVariable(const std::string& key, std::map<std::string, ConfigValue>& config, ConfigSource source);
    
    // Command line helpers
    static bool parseCommandLineArgs(int argc, char** argv, std::map<std::string, ConfigValue>& config);
};

} // namespace nlm
