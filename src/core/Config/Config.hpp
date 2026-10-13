#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration validation and schema definition
struct ConfigSchema {
    std::string key;
    ConfigSource source;
    bool required;
    std::string description;
    std::string type; // "int", "double", "bool", "string", "list"
    
    // Validation constraints
    std::optional<int> minInt;
    std::optional<int> maxInt;
    std::optional<double> minDouble;
    std::optional<double> maxDouble;
    std::vector<std::string> allowedValues;
    
    ConfigSchema() : key(), source(ConfigSource::Default), required(false), description() {}
};

namespace ConfigValidation {

    // Validate configuration against default schema
    bool validate(const Config& config);
    
    // Validate configuration against custom schema
    bool validate(const Config& config, const std::vector<ConfigSchema>& schema);
    
    // Get validation errors
    std::vector<std::string> getErrors(const Config& config);
    
    // Get validation errors with custom schema
    std::vector<std::string> getErrors(const Config& config, const std::vector<ConfigSchema>& schema);
    
    // Check if configuration is valid
    bool isValid(const Config& config);
    
    // Validate configuration file
    bool validateFile(const std::string& filepath);
    
    // Get default schema
    std::vector<ConfigSchema> getDefaultSchema();
    
    // Add validation to Config::loadFromFile
    bool loadFromFileWithValidation(Config& config, const std::string& filepath);
}

// Configuration methods with validation
public:
    // Load configuration from file with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from file with validation and error checking
    bool loadFromFileWithValidation(const std::string& filepath);
    
    // Validate configuration against default schema
    bool validate() const;
    
    // Validate configuration against custom schema
    bool validate(const std::vector<ConfigSchema>& schema) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get validation errors with custom schema
    std::vector<std::string> getValidationErrors(const std::vector<ConfigSchema>& schema) const;
    
    // Check if configuration is valid
    bool isValid() const;
    
private:
    // Internal validation helper
    std::vector<std::string> validateInternal(const std::vector<ConfigSchema>& schema) const;

} // namespace nlm
