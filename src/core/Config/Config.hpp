#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <functional>
#include <unordered_set>
#include <any>
#include <chrono>

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

// Validation error types
struct ValidationError {
    std::string key;
    std::string message;
    ValidationError(const std::string& k, const std::string& m) : key(k), message(m) {}
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required = false;
    ValidationRule(const std::string& k, std::function<bool(const ConfigValue&)> v, 
                  const std::string& err, bool req = false) 
        : key(k), validator(v), errorMessage(err), required(req) {}
};

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::vector<ValidationRule> rules;
    std::map<std::string, ConfigValue> defaults;
    std::string description;
    
    ConfigSchema(const std::string& n, const std::string& desc = "") 
        : name(n), description(desc) {}
    
    void addRule(const std::string& key, std::function<bool(const ConfigValue&)> validator,
                 const std::string& errorMsg, bool required = false) {
        rules.emplace_back(key, validator, errorMsg, required);
    }
    
    void addDefault(const std::string& key, const ConfigValue& defaultValue) {
        defaults[key] = defaultValue;
    }
};

// Configuration template for different phases
struct ConfigTemplate {
    std::string phase;
    std::string description;
    std::map<std::string, ConfigValue> values;
    
    ConfigTemplate(const std::string& p, const std::string& desc = "") 
        : phase(p), description(desc) {}
    
    void set(const std::string& key, const ConfigValue& value) {
        values[key] = value;
    }
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

// Validation error types
struct ValidationError {
    std::string key;
    std::string message;
    ValidationError(const std::string& k, const std::string& m) : key(k), message(m) {}
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required = false;
    ValidationRule(const std::string& k, std::function<bool(const ConfigValue&)> v, 
                  const std::string& err, bool req = false) 
        : key(k), validator(v), errorMessage(err), required(req) {}
};

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::vector<ValidationRule> rules;
    std::map<std::string, ConfigValue> defaults;
    std::string description;
    
    ConfigSchema(const std::string& n, const std::string& desc = "") 
        : name(n), description(desc) {}
    
    void addRule(const std::string& key, std::function<bool(const ConfigValue&)> validator,
                 const std::string& errorMsg, bool required = false) {
        rules.emplace_back(key, validator, errorMsg, required);
    }
    
    void addDefault(const std::string& key, const ConfigValue& defaultValue) {
        defaults[key] = defaultValue;
    }
};

// Configuration template for different phases
struct ConfigTemplate {
    std::string phase;
    std::string description;
    std::map<std::string, ConfigValue> values;
    
    ConfigTemplate(const std::string& p, const std::string& desc = "") 
        : phase(p), description(desc) {}
    
    void set(const std::string& key, const ConfigValue& value) {
        values[key] = value;
    }
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
    
    // Load from file (JSON or YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from configuration file (backward compatibility)
    bool loadFromFileSimple(const std::string& filepath);
    
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
    
    // Validation methods
    bool validate(const ConfigSchema& schema, std::vector<ValidationError>& errors) const;
    bool validate() const; // Validate against current schema
    
    // Configuration source precedence handling
    bool hasValue(const std::string& key, ConfigSource source) const;
    std::optional<ConfigValue> getWithSource(const std::string& key, ConfigSource source) const;
    std::optional<ConfigValue> getEffectiveValue(const std::string& key) const;
    
    // Template methods
    bool applyTemplate(const ConfigTemplate& template);
    
    // Command-line options
    static bool parseCommandLineOptions(int argc, char** argv, 
        std::map<std::string, std::string>& options,
        std::vector<std::pair<std::string, std::string>>& setValues);
    
    // Configuration file path resolution
    static std::string resolveConfigPath(const std::string& configFile = "configs/default.cfg");
    
    // Get configuration source precedence for debugging
    std::string getSourcePrecedence() const;
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    
    // Utility methods
    static std::string trim(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string toLower(const std::string& str);
    static bool parseSimpleConfig(const std::string& content);
    static ConfigValue parseSimpleValue(const std::string& valueStr);
    static std::string configValueToString(const ConfigValue& value) const;
    
    // JSON parsing helpers
    static bool parseJSON(const std::string& content);
    static ConfigValue parseJSONValue(const std::string& jsonStr);
    
    // Schema validation helpers
    static bool validateAgainstSchema(const ConfigSchema& schema, const std::vector<ConfigEntry>& entries,
                                    std::vector<ValidationError>& errors);
};

} // namespace nlm
