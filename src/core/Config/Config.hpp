#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
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
    stdvector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "") 
        : std::runtime_error(message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

// Configuration type validator
class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&)>;
    using ErrorHandler = std::function<void(const ConfigValidationError&)>;
    
    ConfigValidator() = default;
    
    // Add a validation rule for a key
    void addRule(const std::string& key, ValidationRule rule, 
                 ErrorHandler errorHandler = nullptr, const std::string& description = "") {
        rules_[key] = {rule, errorHandler, description};
    }
    
    // Validate a configuration value
    bool validate(const std::string& key, const ConfigValue& value) const {
        auto it = rules_.find(key);
        if (it == rules_.end()) {
            return true; // No validation rule for this key
        }
        
        const auto& rule = it->second;
        if (!rule.validationRule(value)) {
            if (rule.errorHandler) {
                rule.errorHandler(ConfigValidationError(
                    "Configuration validation failed for key: " + key, key));
            }
            return false;
        }
        return true;
    }
    
    // Get validation description for a key
    std::string getDescription(const std::string& key) const {
        auto it = rules_.find(key);
        if (it != rules_.end()) {
            return it->second.description;
        }
        return "";
    }
    
    // Get all registered validation rules
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(rules_.size());
        for (const auto& pair : rules_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Clear all validation rules
    void clear() {
        rules_.clear();
    }
    
private:
    struct ValidationRuleInfo {
        ValidationRule validationRule;
        ErrorHandler errorHandler;
        std::string description;
    };
    
    std::map<std::string, ValidationRuleInfo> rules_;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validatorKey; // Reference to validator
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validatorKey() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& validator = "")
        : key(k), value(v), source(s), description(desc), validatorKey(validator) {}
};

// Configuration presets for different use cases
class ConfigPresets {
public:
    static std::map<std::string, std::vector<std::pair<std::string, ConfigValue>>> getDefaultPresets() {
        return {
            {"development", {
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"neuron_count", 1000},
                {"region_count", 1},
                {"connection_probability", 0.1f},
                {"plasticity.stdp.enable", true},
                {"plasticity.hebbian.enable", true},
                {"neuromod.dopamine.scale", 1.0f},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true}
            }},
            {"production", {
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"neuron_count", 5000},
                {"region_count", 2},
                {"connection_probability", 0.15f},
                {"plasticity.stdp.enable", true},
                {"plasticity.hebbian.enable", true},
                {"neuromod.dopamine.scale", 1.2f},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true}
            }},
            {"research", {
                {"random_seed", 12345},
                {"simulation_timestep", 0.001},
                {"neuron_count", 2000},
                {"region_count", 1},
                {"connection_probability", 0.12f},
                {"plasticity.stdp.enable", true},
                {"plasticity.stdp.learning_rate", 0.015f},
                {"plasticity.hebbian.enable", true},
                {"neuromod.dopamine.scale", 1.5f},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true},
                {"enable_checkpointing", true},
                {"checkpoint_dir", "./checkpoints"}
            }},
            {"minimal", {
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"neuron_count", 100},
                {"region_count", 1},
                {"connection_probability", 0.05f}
            }},
            {"debug", {
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"neuron_count", 50},
                {"region_count", 1},
                {"connection_probability", 0.2f},
                {"plasticity.stdp.enable", true},
                {"plasticity.hebbian.enable", true},
                {"neuromod.dopamine.scale", 1.0f},
                {"neuromod.curiosity.enable", false}, // Disable for predictable debugging
                {"neuromod.novelty.enable", false}
            }}
        };
    }
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with validation (new methods)
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Apply preset configuration
    bool applyPreset(const std::string& presetName, bool validate = true);
    
    // Merge with another configuration
    void merge(const Config& other, bool override = true);
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Get validation status
    bool hasValidationErrors() const { return !validationErrors_.empty(); }
    
    // Get validator
    const ConfigValidator& getValidator() const { return validator_; }
    ConfigValidator& getValidator() { return validator_; }
    
    // Set validator
    void setValidator(const ConfigValidator& validator) { validator_ = validator; }
    
    // Internal helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static bool isNumeric(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigValidator validator_;
    std::vector<std::string> validationErrors_;
    
    // Internal helper methods
    void addValidationError(const std::string& error);
    void clearValidationErrors();
    void validateCurrent() const;
    
    // Private setters for internal use
    void setInternal(const std::string& key, const ConfigValue& value, ConfigSource source);
};

// Template implementations
namespace ConfigImpl {
    template<typename T>
    std::optional<T> getValue(const ConfigValue& value) {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    std::vector<T> variantVectorToStdVector(const ConfigValue& value) {
        std::vector<T> result;
        try {
            if constexpr (std::is_same_v<T, int>) {
                const auto& vec = std::get<std::vector<int>>(value);
                result = vec;
            } else if constexpr (std::is_same_v<T, double>) {
                const auto& vec = std::get<std::vector<double>>(value);
                result = vec;
            } else if constexpr (std::is_same_v<T, std::string>) {
                const auto& vec = std::get<std::vector<std::string>>(value);
                result = vec;
            }
        } catch (const std::bad_variant_access&) {
            // Return empty vector for type mismatch
        }
        return result;
    }
}

} // namespace nlm
