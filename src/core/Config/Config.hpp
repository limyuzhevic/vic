#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message) 
        : std::runtime_error(message) {}
};

// Configuration type validation result
struct ConfigValidationResult {
    bool valid;
    std::string errorMessage;
    
    static ConfigValidationResult success() {
        return {true, ""};
    }
    
    static ConfigValidationResult error(const std::string& message) {
        return {false, message};
    }
};

// Configuration type constraints
struct ConfigConstraints {
    std::string typeName;
    std::optional<std::string> defaultValue;
    std::optional<std::string> description;
    bool required = false;
    std::function<ConfigValidationResult(const std::string&, const ConfigValue&)> validator;
    
    ConfigConstraints(const std::string& type, bool req = false,
                     const std::string& desc = "",
                     const std::string& def = "")
        : typeName(type), defaultValue(def), description(desc), required(req) {}
};

// Configuration value types (enhanced)
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

// Enhanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeName;  // For validation and documentation
    std::vector<std::string> tags;  // For categorization
    bool deprecated = false;
    std::string deprecationMessage;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), typeName(), deprecated(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& type = "auto")
        : key(k), value(v), source(s), description(desc), typeName(type), deprecated(false) {}
};

// Configuration schema for validation and auto-completion
class ConfigSchema {
public:
    using Validator = std::function<ConfigValidationResult(const std::string&, const ConfigValue&)>
    ;
    
    void addField(const std::string& key, const ConfigConstraints& constraints);
    void addValidator(const std::string& key, Validator validator);
    void removeField(const std::string& key);
    void clear();
    
    ConfigValidationResult validate(const std::string& key, const ConfigValue& value) const;
    std::optional<ConfigConstraints> getConstraints(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    std::vector<std::string> getKeysByTag(const std::string& tag) const;
    std::vector<std::string> getDeprecatedKeys() const;
    
    // Schema for common neural simulation parameters
    static ConfigSchema createNeuralSimulationSchema();
    static ConfigSchema createDebugSchema();
    static ConfigSchema createPerformanceSchema();
    
private:
    std::map<std::string, ConfigConstraints> fields;
    std::map<std::string, std::vector<Validator>> validators;
};

// Enhanced configuration class with validation and error handling
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
    void initializeWithSchema(const ConfigSchema& schema);
    
    // Load from file (enhanced with better error handling)
    bool loadFromFile(const std::string& filepath, const ConfigSchema& schema = ConfigSchema());
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, const ConfigSchema& schema = ConfigSchema());
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation and better error handling
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;
    
    // Get with validation
    template<typename T>
    std::optional<T> getWithValidation(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const ConfigSchema& schema = ConfigSchema());
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const ConfigSchema& schema = ConfigSchema());
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const ConfigSchema& schema = ConfigSchema());
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const ConfigSchema& schema = ConfigSchema());
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const ConfigSchema& schema = ConfigSchema());
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by type
    std::vector<std::string> getKeysByType(const std::string& typeName) const;
    
    // Get keys by tag
    std::vector<std::string> getKeysByTag(const std::string& tag) const;
    
    // Get deprecated keys
    std::vector<std::string> getDeprecatedKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with more details
    std::string summary() const;
    std::string summary(const ConfigSchema& schema) const;
    
    // Validate entire configuration
    std::vector<ConfigValidationResult> validateAll(const ConfigSchema& schema) const;
    bool isValid(const ConfigSchema& schema) const;
    
    // Set schema
    void setSchema(const ConfigSchema& schema);
    
    // Get schema
    const ConfigSchema& getSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string valueToString(const ConfigValue& value);
    static ConfigValidationResult validateValueType(const std::string& key, const ConfigValue& value,
                                                   const ConfigConstraints& constraints);
    
    // Get constraints from schema or auto-detect
    ConfigConstraints getConstraintsForKey(const std::string& key) const;
    
    // Enhanced JSON/YAML parsing (placeholder for future implementation)
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Helper for enum validation
    static bool isValidEnumValue(const std::string& value, const std::vector<std::string>& validValues);
};

// Helper function to convert string to ConfigValue
ConfigValue stringToConfigValue(const std::string& str);

// Helper function to get type name from ConfigValue
std::string configValueTypeName(const ConfigValue& value);

} // namespace nlm