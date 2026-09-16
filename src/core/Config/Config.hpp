#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "ConfigError.hpp"

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
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    void validate(ConfigValidationException::Level level = ConfigValidationException::Level::Full);
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Validation schema
    void addValidationRule(const ConfigValidationRule& rule);
    ConfigValidationRule& addValidationRule(const std::string& key, const std::string& name, 
                                           ConfigValidationType type, bool required = false,
                                           std::optional<double> minValue = std::nullopt,
                                           std::optional<double> maxValue = std::nullopt);
    void removeValidationRule(const std::string& key);
    
    // Validation hooks for categories
    void addValidator(std::unique_ptr<ConfigValidatorBase> validator);
    
    // Runtime validation settings
    void setValidationLevel(ConfigValidationException::Level level);
    void setValidationEnabled(bool enabled);
    bool isValidationEnabled() const;
    
    // Pre-validation before use (for critical operations)
    void preValidate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation state
    ConfigValidationException::Level validationLevel;
    bool validationEnabled;
    std::vector<ConfigValidationRule> validationRules;
    std::vector<std::unique_ptr<ConfigValidatorBase>> registeredValidators;
    
    // Validation methods
    void validateKeyType(const std::string& key, const ConfigValue& value) const;
    void validateKeyRange(const std::string& key, const ConfigValue& value) const;
    void validateDependencies(const std::string& key, const Config& config) const;
    void validateRequired(const std::string& key, const Config& config) const;
    void applyCustomValidators(const std::string& key, const ConfigValue& value) const;
    bool isKeyTypeValid(const std::string& key, const ConfigValue& value) const;
    double getNumericValue(const ConfigValue& value) const;
    
    // Category-specific validation hooks
    void validateBrainCategory(const Config& config) const;
    void validatePlasticityCategory(const Config& config) const;
    void validateNeuromodulationCategory(const Config& config) const;
    void validateMemoryCategory(const Config& config) const;
    void validateCognitionCategory(const Config& config) const;
    void validateDevelopmentCategory(const Config& config) const;
}; 

} // namespace nlm
