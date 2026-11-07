#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// JSON library forward declaration (to avoid hard dependency)
namespace json { struct json_value; }

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

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string description;
    ConfigValue defaultValue;
    bool required;
};

// Configuration group for organization
struct ConfigGroup {
    std::string name;
    std::vector<std::string> keys;
    std::string description;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t fromFile;
    size_t fromCommandLine;
    size_t fromDefault;
    size_t fromRuntime;
};

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string description;
    ConfigValue defaultValue;
    bool required;
};

// Configuration group for organization
struct ConfigGroup {
    std::string name;
    std::vector<std::string> keys;
    std::string description;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t fromFile;
    size_t fromCommandLine;
    size_t fromDefault;
    size_t fromRuntime;
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
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
    
// Configuration validation and schema support
    
    // Validation functions for common configuration patterns
    static bool validatePositive(const ConfigValue& value);
    static bool validateNonNegative(const ConfigValue& value);
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateSize(const ConfigValue& value, size_t minSize, size_t maxSize);
    static bool validateString(const ConfigValue& value, size_t minLength = 0);
    static bool validatePositiveInt(const ConfigValue& value);
    static bool validateProbability(const ConfigValue& value);
    static bool validateStringEnum(const ConfigValue& value, 
                                   const std::vector<std::string>& allowedValues);
    static bool validateFilePath(const ConfigValue& value, bool mustExist = false);
    
    // Create validation schema for common configuration patterns
    static std::vector<ConfigSchemaEntry> createCoreSchema();
    static std::vector<ConfigSchemaEntry> createNeuralNetworkSchema();
    static std::vector<ConfigSchemaEntry> createPlasticitySchema();
    static std::vector<ConfigSchemaEntry> createDevelopmentSchema();
    static std::vector<ConfigSchemaEntry> createNeuromodulationSchema();
    
    // Configuration validation utilities
    static std::vector<std::string> getInvalidKeys(const std::vector<ConfigSchemaEntry>& schema) const;
    static std::string getValidationReport(const std::vector<ConfigSchemaEntry>& schema) const;
    static bool isValid(const std::vector<ConfigSchemaEntry>& schema) const;
    
    // Configuration comparison and merging
    static bool configEqual(const ConfigValue& a, const ConfigValue& b);
    static ConfigValue mergeValues(const ConfigValue& base, const ConfigValue& override);
    
    // Configuration transformation
    static ConfigValue toString(const ConfigValue& value);
    static ConfigValue toDouble(const ConfigValue& value);
    static ConfigValue toInt(const ConfigValue& value);
    static ConfigValue toBool(const ConfigValue& value);
};

} // namespace nlm
