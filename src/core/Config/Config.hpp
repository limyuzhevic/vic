#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>
#include <map>

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& error = "")
        : valid(v), errorMessage(error) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult error(const std::string& message) { return ValidationResult(false, message); }
};

// Range validation for numeric values
struct RangeValidation {
    double min;
    double max;
    bool inclusive = true;
    
    RangeValidation(double minVal, double maxVal, bool inclusiveVal = true)
        : min(minVal), max(maxVal), inclusive(inclusiveVal) {}
    
    bool isValid(double value) const {
        if (inclusive) {
            return value >= min && value <= max;
        } else {
            return value > min && value < max;
        }
    }
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Configuration default definition
struct DefaultConfig {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    bool required = false;
    
    DefaultConfig(const std::string& k, const ConfigValue& v, const std::string& desc = "")
        : key(k), defaultValue(v), description(desc), required(false) {}
    
    DefaultConfig(const std::string& k, const ConfigValue& v, const std::string& desc, bool req)
        : key(k), defaultValue(v), description(desc), required(req) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::vector<RangeValidation> rangeValidations;
    bool validated = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& typeName = "")
        : key(k), value(v), source(s), description(desc), type(typeName), validated(false) {}
};

// Configuration schema for type validation and constraints
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a configuration definition
    void addDefinition(const std::string& key, const ConfigValue& defaultValue, 
                      const std::string& description, const std::string& type = "auto",
                      const std::vector<RangeValidation>& ranges = {});
    
    // Get default value for key
    std::optional<ConfigValue> getDefault(const std::string& key) const;
    
    // Get description for key
    std::optional<std::string> getDescription(const std::string& key) const;
    
    // Get type for key
    std::optional<std::string> getType(const std::string& key) const;
    
    // Get range validations for key
    std::vector<RangeValidation> getRanges(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Validate a configuration entry
    ValidationResult validate(const ConfigEntry& entry) const;
    
    // Check if key is defined
    bool hasKey(const std::string& key) const;
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Save schema to file
    bool saveToFile(const std::string& filepath) const;
    
private:
    std::map<std::string, ConfigEntry> definitions;
};

// Main configuration class with enhanced features
class Config {
public:
    Config(const ConfigSchema& schema = ConfigSchema());
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from YAML file (if YAML support available)
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file (if JSON support available)
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get with validation
    template<typename T>
    ValidationResult getValidated(const std::string& key, T& outValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool isType(const std::string& key, const std::string& typeName) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Reset to defaults
    void resetToDefaults();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get source of configuration
    ConfigSource getSource(const std::string& key) const;
    
    // Set schema
    void setSchema(const ConfigSchema& schema);
    
    // Get schema
    const ConfigSchema& getSchema() const;
    
    // Merge configurations
    void merge(const Config& other);
    
    // Validate all entries
    std::vector<ValidationResult> validateAll() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectType(const ConfigValue& value);
    static bool parseSimpleLine(const std::string& line, std::string& key, std::string& value);
};

// Configuration exception hierarchy
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) 
        : ConfigException("Configuration validation error: " + message) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& message, const std::string& filepath = "")
        : ConfigException("Failed to load configuration file " + (filepath.empty() ? "" : "'" + filepath + "': ") + message) {}
};

// Convenience template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
