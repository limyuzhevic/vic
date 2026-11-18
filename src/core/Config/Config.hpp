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

// Validation result
enum class ValidationResult {
    Success,
    InvalidType,
    ValueOutOfRange,
    MissingRequired,
    InvalidFormat
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required;
    
    ConfigValidationRule(const std::string& k, const std::function<ValidationResult(const ConfigValue&)>& v, 
                         const std::string& err, bool req = false)
        : key(k), validator(v), errorMessage(err), required(req) {}
};

// Configuration schema
class ConfigSchema {
public:
    // Add validation rule
    void addRule(const ConfigValidationRule& rule);
    
    // Validate configuration
    ValidationResult validate(const Config& config, std::string& errorMessage) const;
    
    // Get all validation errors
    std::vector<std::string> getValidationErrors(const Config& config) const;
    
    // Check if all rules are satisfied
    bool isValid(const Config& config) const;
    
    // Get required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get validation error count
    size_t getErrorCount(const Config& config) const;
    
private:
    std::vector<ConfigValidationRule> rules;
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation methods
    bool validate() const;
    std::string getValidationErrors() const;
    void setSchema(const ConfigSchema& schema);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Configuration schema for validation
    ConfigSchema schema;
};

} // namespace nlm
