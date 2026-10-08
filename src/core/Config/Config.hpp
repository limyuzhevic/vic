#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
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

// Configuration validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ValidationFunction validator;  // Optional validation function
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const ValidationFunction& validator = nullptr)
        : key(k), value(v), source(s), description(desc), validator(validator) {}
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// Validation helper functions
namespace Validation {
    // Validation functions for common types
    bool isInRange(double value, double min, double max);
    bool isPositive(double value);
    bool isNonNegative(double value);
    bool isInteger(double value);
    bool isString(const std::string& value);
    bool isValidNeuronCount(double value);
    bool isValidSynapseCount(double value);
    bool isValidConnectionProbability(double value);
    bool isValidTimeStep(double value);
    bool isValidPlasticityRate(double value);
    bool isValidWeight(double value);
    bool isValidBias(double value);
    bool isValidTemperature(double value);
    
    // Create validator with range
    ValidationFunction rangeValidator(double min, double max);
    ValidationFunction positiveValidator();
    ValidationFunction nonNegativeValidator();
    ValidationFunction integerValidator();
}

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
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const ValidationFunction& validator = nullptr);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const ValidationFunction& validator = nullptr);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const ValidationFunction& validator = nullptr);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const ValidationFunction& validator = nullptr);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const ValidationFunction& validator = nullptr);
    
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
    
    // Validate all configuration values
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
