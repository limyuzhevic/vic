#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Configuration validation
enum class ValidationError {
    None = 0,
    TypeMismatch,
    ValueOutOfRange,
    MissingRequired,
    InvalidFormat,
    OutOfBounds
};

// Schema validation constraints
struct ValidationConstraint {
    std::string key;
    std::string constraintType; // "range", "enum", "format", "required", "pattern"
    std::string constraintValue;
    std::string errorMessage;
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
    
    // Validation methods
    ValidationError validate() const;
    std::string getValidationError() const;
    bool addValidationConstraint(const ValidationConstraint& constraint);
    bool removeValidationConstraint(const std::string& key);
    std::vector<ValidationConstraint> getValidationConstraints() const;
    
    // Schema management
    bool loadSchemaFromFile(const std::string& filepath);
    bool saveSchemaToFile(const std::string& filepath) const;
    std::string getSchemaSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static ValidationError validateValue(const ConfigEntry& entry, const ValidationConstraint& constraint);
    static bool parseRangeConstraint(const std::string& constraint, double& min, double& max);
    static bool parseEnumConstraint(const std::string& constraint, std::vector<std::string>& values);
    static bool parsePatternConstraint(const std::string& constraint, std::regex& pattern);
};

// Configuration schema class for managing validation rules
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add validation rules
    bool addRequiredField(const std::string& key, const std::string& description = "");
    bool addRangeValidation(const std::string& key, double min, double max, const std::string& description = "");
    bool addEnumValidation(const std::string& key, const std::vector<std::string>& validValues, const std::string& description = "");
    bool addPatternValidation(const std::string& key, const std::string& pattern, const std::string& description = "");
    
    // Remove validation rules
    bool removeField(const std::string& key);
    
    // Check if schema is valid
    bool isValid() const;
    
    // Get field information
    bool hasField(const std::string& key) const;
    std::string getFieldDescription(const std::string& key) const;
    
    // Get all fields
    std::vector<std::string> getFields() const;
    
    // Save/load schema
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Summary
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
