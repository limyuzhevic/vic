#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>

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

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::vector<std::string>& errs) 
        : isValid(valid), errors(errs) {}
    ValidationResult(bool valid, const std::vector<std::string>& errs, const std::vector<std::string>& warns)
        : isValid(valid), errors(errs), warnings(warns) {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type; // For validation (e.g., "int", "double", "string", "bool")
    std::string unit; // For units (e.g., "ms", "V", "Hz")
    bool required; // Whether this parameter is required
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), unit(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const std::string& t = "", const std::string& u = "", bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), unit(u), required(req) {}
};

// Configuration validation function type
using ValidationFunction = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration schema for validation
class ConfigSchema {
public:
    ConfigSchema();
    
    // Add a configuration parameter with validation
    void addParameter(const std::string& key, const std::string& description, 
                     const std::string& type, bool required = false,
                     const std::string& unit = "",
                     const ValidationFunction& validator = nullptr);
    
    // Add a parameter with default value
    void addParameterWithDefault(const std::string& key, const std::string& description,
                                 const ConfigValue& defaultValue, const std::string& type,
                                 ConfigSource source = ConfigSource::Default,
                                 const std::string& unit = "",
                                 const ValidationFunction& validator = nullptr);
    
    // Get parameter metadata
    const ConfigEntry* getParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getParameterKeys() const;
    
    // Validate a configuration value against schema
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Validate entire configuration
    ValidationResult validateAll(const std::vector<ConfigEntry>& entries) const;
    
    // Get help text for all parameters
    std::string getHelpText() const;
    
    // Get parameter summary
    std::string getParameterSummary(const std::string& key) const;
    
    // Check if parameter exists in schema
    bool hasParameter(const std::string& key) const;
    
    // Get default value for parameter
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set configuration schema
    void setSchema(std::unique_ptr<ConfigSchema> schema);
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
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
    
    // Validate configuration against schema
    ValidationResult validate() const;
    
    // Get help text for configuration
    std::string getHelpText() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get validation warnings
    std::vector<std::string> getValidationWarnings() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Configuration schema
    std::unique_ptr<ConfigSchema> schema;
};

} // namespace nlm
