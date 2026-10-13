#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <string>
#include <limits>

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

// Configuration validation error
enum class ConfigValidationError {
    None,
    OutOfRange,
    InvalidType,
    MissingDependency,
    InconsistentValues
};

// Configuration validation result
struct ConfigValidationResult {
    ConfigValidationError error = ConfigValidationError::None;
    std::string message;
    std::string key;
    
    ConfigValidationResult() : error(ConfigValidationError::None), message(), key() {}
    ConfigValidationResult(ConfigValidationError err, const std::string& msg, const std::string& k)
        : error(err), message(msg), key(k) {}
    
    bool isValid() const { return error == ConfigValidationError::None; }
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigValidationRule(const std::string& k, std::function<bool(const ConfigValue&)> val,
                        const std::string& err = "Validation failed")
        : key(k), validator(val), errorMessage(err) {}
};

// Configuration cross-validation rule (dependencies between keys)
struct ConfigCrossValidationRule {
    std::vector<std::string> dependentKeys;
    std::function<bool(const std::vector<const ConfigValue*>&)> validator;
    std::string errorMessage;
    
    ConfigCrossValidationRule(const std::vector<std::string>& keys,
                              std::function<bool(const std::vector<const ConfigValue*>&)> val,
                              const std::string& err = "Cross-validation failed")
        : dependentKeys(keys), validator(val), errorMessage(err) {}
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
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Configuration validation
    ConfigValidationResult validate(const std::string& key) const;
    std::vector<ConfigValidationResult> validateAll() const;
    bool isValid(const std::string& key) const;
    bool isValidAll() const;
    
    // Add validation rules
    void addValidationRule(const ConfigValidationRule& rule);
    void addCrossValidationRule(const ConfigCrossValidationRule& rule);
    
    // Remove validation rules
    void removeValidationRule(const std::string& key);
    void clearValidationRules();
    
    // Get validation errors for specific key
    std::vector<ConfigValidationResult> getValidationErrors(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateCrossDependencies(const std::string& key) const;
    static bool validateRange(const ConfigValue& value, double min, double max, const std::string& key);
    static bool validateIntRange(const ConfigValue& value, int64_t min, int64_t max, const std::string& key);
    static bool validatePositive(const ConfigValue& value, const std::string& key);
    static bool validateNonNegative(const ConfigValue& value, const std::string& key);
    static bool validateBetweenZeroOne(const ConfigValue& value, const std::string& key);
    
    // Cross-validation helpers
    static bool validateConsistentTimestep(const std::vector<const ConfigValue*>& values);
    static bool validateConsistentNeuronCounts(const std::vector<const ConfigValue*>& values);
    static bool validateConsistentPlasticity(const std::vector<const ConfigValue*>& values);
};

// Validation rule predicates
inline bool isPositive(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return std::get<int>(value) > 0;
    if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value) > 0;
    if (std::holds_alternative<double>(value)) return std::get<double>(value) > 0.0;
    if (std::holds_alternative<float>(value)) return std::get<float>(value) > 0.0f;
    return false;
}

inline bool isNonNegative(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return std::get<int>(value) >= 0;
    if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value) >= 0;
    if (std::holds_alternative<double>(value)) return std::get<double>(value) >= 0.0;
    if (std::holds_alternative<float>(value)) return std::get<float>(value) >= 0.0f;
    return false;
}

inline bool isBetweenZeroOne(const ConfigValue& value) {
    if (std::holds_alternative<double>(value)) {
        double v = std::get<double>(value);
        return v >= 0.0 && v <= 1.0;
    }
    if (std::holds_alternative<float>(value)) {
        float v = std::get<float>(value);
        return v >= 0.0f && v <= 1.0f;
    }
    if (std::holds_alternative<int>(value)) {
        int v = std::get<int>(value);
        return v >= 0 && v <= 1;
    }
    if (std::holds_alternative<int64_t>(value)) {
        int64_t v = std::get<int64_t>(value);
        return v >= 0 && v <= 1;
    }
    return false;
}

inline bool isFinite(const ConfigValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::isfinite(std::get<double>(value));
    }
    if (std::holds_alternative<float>(value)) {
        return std::isfinite(std::get<float>(value));
    }
    return true;  // int types are always finite
}

// Configuration schema validation
class ConfigSchema {
public:
    static const std::vector<ConfigValidationRule> getPhase2ValidationRules();
    static const std::vector<ConfigCrossValidationRule> getPhase2CrossValidationRules();
    
private:
    static ConfigValidationResult validateRandomSeed(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validateTimestep(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validateNeuronCount(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validateRegionCount(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validateConnectionProbability(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validateSTDPParameters(const ConfigValue& value, const std::string& key);
    static ConfigValidationResult validatePlasticityParameters(const ConfigValue& value, const std::string& key);
};

} // namespace nlm
