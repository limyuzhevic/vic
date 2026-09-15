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
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
// Configuration validation rules
enum class ValidationRule {
    Required,        // Value must be present
    Positive,        // Value must be positive
    NonNegative,     // Value must be non-negative
    Range,           // Value must be within a range
    Choice,          // Value must be one of specified choices
    Pattern,         // Value must match pattern
    Callback         // Value must pass custom validation function
};

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    ValidationRule rule;
    std::string expected;
    std::string actual;
};

// Validation condition
struct ValidationCondition {
    ValidationRule rule;
    std::variant<std::string, double, int, std::vector<std::string>> value;
    std::function<bool(const ConfigValue&)> validator;
};

// Configuration validation system
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add validation rule
    void addRule(const std::string& key, ValidationRule rule, 
                 const std::string& message = "",
                 const std::variant<std::string, double, int, std::vector<std::string>>& value = {});
    
    // Add custom validator
    void addValidator(const std::string& key, 
                     std::function<bool(const ConfigValue&)> validator,
                     const std::string& message = "");
    
    // Validate configuration
    std::vector<ValidationError> validate(const Config& config) const;
    
    // Check if configuration is valid
    bool isValid(const Config& config) const;
    
    // Get all errors for a key
    std::vector<ValidationError> getErrorsForKey(const Config& config, const std::string& key) const;
    
    // Clear all rules
    void clear();
    
    // Load validation rules from config file
    bool loadFromFile(const std::string& filepath);
    
    // Save validation rules to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get validation summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
