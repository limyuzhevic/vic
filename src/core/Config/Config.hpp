#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <any>
#include <chrono>
#include <functional>
#include <set>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Config;
struct ConfigEntry;
struct ValidationResult;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, ConfigValue>
>;

// Forward declaration for converter functions
nlohmann::json configValueToJson(const ConfigValue& value);
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
YAML::Node configValueToYaml(const ConfigValue& value);
ConfigValue yamlToConfigValue(const YAML::Node& yamlValue);

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with path and metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string path; // Nested path using dot notation, e.g., "plasticity.stdp_ltp_weight"
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), path() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& p = "")
        : key(k), value(v), source(s), description(desc), path(p) {}
};

// Parameter validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string warningMessage;
    
    ValidationResult(bool valid, const std::string& error = "", const std::string& warning = "")
        : isValid(valid), errorMessage(error), warningMessage(warning) {}
};

// Parameter validation function type
using ParameterValidator = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Parameter range for numeric values
struct ParameterRange {
    double min;
    double max;
    double defaultValue;
    ParameterRange(double m = 0.0, double ma = 0.0, double def = 0.0) : min(m), max(ma), defaultValue(def) {}
};

// Parameter metadata
struct ParameterMetadata {
    std::string name;
    std::string description;
    std::string type;
    std::any defaultValue;
    bool required;
    ParameterRange range;
    std::set<std::string> allowedValues;
    
    ParameterMetadata(const std::string& n = "", const std::string& desc = "", const std::string& t = "",
                     const std::any& def = nullptr, bool req = false)
        : name(n), description(desc), type(t), defaultValue(def), required(req) {}
};

// Main configuration class with JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or backward compatible key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from YAML string  
    bool loadFromYaml(const std::string& yamlStr);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get nested values using path notation
    template<typename T>
    std::optional<T> get(const std::string& key, const std::string& path) const;
    
    template<typename T>
    T getOr(const std::string& key, const std::string& path, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& path = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& path = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& path = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& path = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& path = "");
    
    // Check existence
    bool has(const std::string& key) const;
    bool has(const std::string& key, const std::string& path) const;
    
    // Remove key
    void remove(const std::string& key);
    void remove(const std::string& key, const std::string& path);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys at a specific path
    std::vector<std::string> getKeys(const std::string& path) const;
    
    // Clear all
    void clear();
    
    // Clear all entries at a specific path
    void clear(const std::string& path);
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration as JSON string
    std::string toJson() const;
    
    // Get configuration as YAML string
    std::string toYaml() const;
    
    // Parameter validation
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateAll() const;
    
    // Register parameter validator for a specific key
    void registerValidator(const std::string& key, ParameterValidator validator);
    
    // Register parameter metadata
    void registerParameter(const std::string& key, const ParameterMetadata& metadata);
    
    // Merge with another configuration (overwrites existing values)
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Apply overrides from a source configuration
    void applyOverrides(const Config& base, const Config& overrides, ConfigSource source = ConfigSource::CommandLine);
    
    // Get config by section (for backward compatibility with old key=value format)
    Config getSection(const std::string& sectionPrefix) const;
    
    // Set default values for known configuration parameters
    void setDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractPath(const std::string& key);
    static std::string extractKey(const std::string& key);
    static std::string extractBasename(const std::string& path);
    static std::string extractParentPath(const std::string& path);
};

// Backward compatibility: parse old key=value format
namespace legacy {
    bool parseLegacyConfig(const std::string& content, std::map<std::string, std::string>& output);
}

} // namespace nlm
