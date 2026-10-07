#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;
class ValidationResult;

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
    Runtime,
    SchemaValidation
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::set<std::string> dependencies;
    bool validated = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& message) {
        valid = false;
        errors.push_back(message);
    }
    
    void addWarning(const std::string& message) {
        warnings.push_back(message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
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
    
    // Load from environment variables
    bool loadFromEnv(const std::map<std::string, std::string>& envVars);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
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
    
    // Get keys by source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    ValidationResult validate(const ConfigSchema& schema) const;
    
    // Batch operations
    void applyBatch(const std::map<std::string, ConfigValue>& batch, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration status
    bool isValid(const ConfigSchema& schema) const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getValidationErrors(const ConfigSchema& schema, const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validate individual entry
    std::vector<std::string> validateEntry(const ConfigEntry& entry, const ConfigSchema& schema) const;
    
    // Update dependencies
    void updateDependencies(const std::string& key, const std::set<std::string>& newDeps);
};

} // namespace nlm
