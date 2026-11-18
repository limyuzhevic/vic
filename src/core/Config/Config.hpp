#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <cmath>

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

// Configuration validation system
class ConfigurationValidator {
public:
    // Configuration parameter schema
    struct ParameterSchema {
        std::string name;              ///< Parameter name
        std::string type;              ///< Parameter type ("int", "int64_t", "double", "float", "bool", "string", "vector")
        std::string description;       ///< Human-readable description
        bool required;                 ///< Whether parameter is required
        std::string default_value;     ///< Default value as string
        std::string validation_expr;   ///< Expression for validation (e.g., "0 <= value <= 1000")
        
        ParameterSchema() : name(), type(), description(), required(false), default_value(), validation_expr() {}
        ParameterSchema(const std::string& n, const std::string& t, const std::string& desc, bool req = false,
                       const std::string& def = "", const std::string& expr = "")
            : name(n), type(t), description(desc), required(req), default_value(def), validation_expr(expr) {}
    };
    
    // Configuration validation result
    struct ValidationResult {
        bool valid;                      ///< Whether configuration is valid
        std::vector<std::string> errors; ///< List of validation error messages
        std::vector<std::string> warnings; ///< List of validation warnings
        
        ValidationResult() : valid(true) {}
        ValidationResult(bool v, const std::vector<std::string>& errs, const std::vector<std::string>& warns)
            : valid(v), errors(errs), warnings(warns) {}
    };
    
    // Validation function type
    using ValidationFunction = std::function<bool(const ConfigValue&)>;
    
    ConfigurationValidator();
    ~ConfigurationValidator();
    
    // Disable copying, enable moving
    ConfigurationValidator(const ConfigurationValidator&) = delete;
    ConfigurationValidator& operator=(const ConfigurationValidator&) = delete;
    ConfigurationValidator(ConfigurationValidator&&) noexcept;
    ConfigurationValidator& operator=(ConfigurationValidator&&) noexcept;
    
    // Add parameter schema
    void addParameter(const std::string& name, const std::string& type, const std::string& description,
                     bool required = false, const std::string& default_value = "", 
                     const std::string& validation_expr = "");
    
    // Add custom validation function
    void addCustomValidation(const std::string& parameter_name, 
                           const ValidationFunction& validator,
                           const std::string& error_message = "");
    
    // Validate configuration
    ValidationResult validate(const Config& config) const;
    
    // Validate specific parameter
    ValidationResult validateParameter(const Config& config, const std::string& parameter_name) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get parameter schema
    std::optional<ParameterSchema> getParameterSchema(const std::string& parameter_name) const;
    
    // Check if parameter exists in schema
    bool hasParameter(const std::string& parameter_name) const;
    
    // Get validation errors for parameter
    std::vector<std::string> getParameterErrors(const Config& config, const std::string& parameter_name) const;
    
    // Get validation warnings for parameter
    std::vector<std::string> getParameterWarnings(const Config& config, const std::string& parameter_name) const;
    
    // Clear all schemas
    void clearSchemas();
    
    // Get default configuration
    std::unordered_map<std::string, ConfigValue> getDefaultConfiguration() const;
    
    // Create complete configuration from current config
    Config createCompleteConfiguration(const Config& config) const;
    
    // Check if configuration is complete (all required parameters present)
    bool isConfigurationComplete(const Config& config) const;
    
    // Get validation report
    std::string getValidationReport(const Config& config) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    static bool validateParameterType(const ConfigValue& value, const std::string& type);
    static bool evaluateValidationExpression(const ConfigValue& value, const std::string& expression);
    static std::string convertValueToString(const ConfigValue& value);
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Get global validator instance
    static ConfigurationValidator& getValidator();
    
    // Validate this configuration
    bool validate() const;
    
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
};

} // namespace nlm