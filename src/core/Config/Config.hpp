#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <stdexcept>

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
    bool valid;
    std::string errorMessage;
    std::string warningMessage;
    
    ValidationResult(bool v = true, const std::string& error = "", const std::string& warning = "")
        : valid(v), errorMessage(error), warningMessage(warning) {}
};

// Configuration range constraints
struct ConfigConstraint {
    std::string key;
    double minValue;
    double maxValue;
    bool minInclusive = true;
    bool maxInclusive = true;
    std::string description;
    
    ConfigConstraint(const std::string& k, double min, double max, const std::string& desc = "")
        : key(k), minValue(min), maxValue(max), description(desc) {}
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationError;
    bool wasValidated = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationError(), wasValidated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationError(), wasValidated(false) {}
};

// Type-safe getter functions with validation
template<typename T>
T getValidated(const ConfigValue& value, const std::string& key, const std::vector<ConfigConstraint>& constraints);

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
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate specific key
    ValidationResult validateKey(const std::string& key) const;
    
    // Validate all keys
    ValidationResult validateAll() const;
    
    // Add constraint for key validation
    void addConstraint(const std::string& key, double min, double max, const std::string& desc = "", 
                       bool minInclusive = true, bool maxInclusive = true);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation status
    std::string summary() const;
    
    // Get validation errors
    std::string getValidationErrors() const;
    
    // Get warnings
    std::string getValidationWarnings() const;
    
    // Check if there are any validation errors
    bool hasValidationErrors() const;
    
    // Check if there are any warnings
    bool hasValidationWarnings() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static double stringToDouble(const std::string& str);
    
    // Validation helpers
    template<typename T>
    ValidationResult validateValue(const T& value, const std::string& key) const;
    
    template<typename T>
    ValidationResult validateRange(const T& value, const ConfigConstraint& constraint) const;
};

} // namespace nlm
