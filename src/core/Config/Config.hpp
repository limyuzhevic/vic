#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <chrono>
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

// Configuration source with priority
enum class ConfigSource {
    Default,      // Default values (lowest priority)
    File,         // Loaded from configuration file
    CommandLine,  // Override from command line
    Runtime       // Set at runtime (highest priority)
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getFullMessage() const {
        std::stringstream ss;
        if (!errors.empty()) {
            ss << "Validation Errors:";
            for (const auto& error : errors) ss << "\n  - " << error;
        }
        if (!warnings.empty()) {
            if (!errors.empty()) ss << "\n\n";
            ss << "Validation Warnings:";
            for (const auto& warning : warnings) ss << "\n  - " << warning;
        }
        return ss.str();
    }
};

// Configuration entry with enhanced features
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    std::map<std::string, std::string> validationParams;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationRegex(), customValidator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), customValidator(nullptr) {}
    
    // Validate this configuration entry
    ValidationResult validate() const {
        ValidationResult result;
        
        // Check if key is empty
        if (key.empty()) {
            result.addError("Configuration key cannot be empty");
        }
        
        // Validate against regex if specified
        if (!validationRegex.empty() && std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            // Simple regex validation - would use a proper regex library in production
            if (!validateAgainstRegex(strValue, validationRegex)) {
                result.addError("Value '" + strValue + "' does not match pattern '" + validationRegex + "'");
            }
        }
        
        // Validate against custom validator if specified
        if (customValidator && !customValidator(value)) {
            result.addError("Custom validation failed for key: " + key);
        }
        
        return result;
    }
    
    // Helper for regex validation (simplified)
    static bool validateAgainstRegex(const std::string& value, const std::string& pattern) {
        // Simple pattern matching - in production would use std::regex
        if (pattern == "^[0-9]+$") {
            return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit);
        } else if (pattern == "^[0-9]+(\.[0-9]+)?$") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (pattern == "^[a-zA-Z0-9_]+$") {
            return !value.empty() && std::all_of(value.begin(), value.end(), 
                [](char c) { return std::isalnum(c) || c == '_'; });
        }
        return true; // If no specific pattern, assume valid
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "bool", "string", "vector"
    std::string description;
    ConfigValue defaultValue;
    std::optional<ConfigValue> minValue;
    std::optional<ConfigValue> maxValue;
    std::string validationRegex;
    bool required = false;
    
    ConfigSchema() = default;
    
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc, 
                 const ConfigValue& def, bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
    
    // Validate a value against schema constraints
    ValidationResult validateValue(const ConfigValue& value) const {
        ValidationResult result;
        
        // Check type
        if (!valueMatchesType(value, type)) {
            result.addError("Value type mismatch for key '" + key + "': expected " + type + ", got " + getTypeName(value));
        }
        
        // Check range constraints if value is numeric
        if (minValue.has_value() && maxValue.has_value()) {
            if (!valueInRange(value, minValue.value(), maxValue.value())) {
                result.addError("Value for key '" + key + "' is out of range: " + 
                              std::to_string(getNumericValue(value)) + " must be between " +
                              std::to_string(getNumericValue(minValue.value())) + " and " +
                              std::to_string(getNumericValue(maxValue.value())));
            }
        } else if (minValue.has_value()) {
            if (!valueGreaterThanOrEqual(value, minValue.value())) {
                result.addError("Value for key '" + key + "' must be >= " + 
                              std::to_string(getNumericValue(minValue.value())));
            }
        } else if (maxValue.has_value()) {
            if (!valueLessThanOrEqual(value, maxValue.value())) {
                result.addError("Value for key '" + key + "' must be <= " + 
                              std::to_string(getNumericValue(maxValue.value())));
            }
        }
        
        // Validate against regex
        if (!validationRegex.empty() && std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            if (!ConfigEntry::validateAgainstRegex(strValue, validationRegex)) {
                result.addError("Value '" + strValue + "' does not match pattern '" + validationRegex + "'");
            }
        }
        
        return result;
    }
    
private:
    bool valueMatchesType(const ConfigValue& value, const std::string& type) const {
        if (type == "int" && std::holds_alternative<int>(value)) return true;
        if (type == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
        if (type == "double" && (std::holds_alternative<double>(value) || std::holds_alternative<int>(value))) return true;
        if (type == "bool" && std::holds_alternative<bool>(value)) return true;
        if (type == "string" && std::holds_alternative<std::string>(value)) return true;
        if (type == "vector" && (std::holds_alternative<std::vector<int>>(value) || 
                                 std::holds_alternative<std::vector<double>>(value) ||
                                 std::holds_alternative<std::vector<std::string>>(value))) return true;
        return false;
    }
    
    std::string getTypeName(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
    
    double getNumericValue(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        return 0.0;
    }
    
    bool valueInRange(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max) const {
        if (!valueMatchesType(value, getTypeName(min))) return false;
        double val = getNumericValue(value);
        double minVal = getNumericValue(min);
        double maxVal = getNumericValue(max);
        return val >= minVal && val <= maxVal;
    }
    
    bool valueGreaterThanOrEqual(const ConfigValue& value, const ConfigValue& min) const {
        if (!valueMatchesType(value, getTypeName(min))) return false;
        double val = getNumericValue(value);
        double minVal = getNumericValue(min);
        return val >= minVal;
    }
    
    bool valueLessThanOrEqual(const ConfigValue& value, const ConfigValue& max) const {
        if (!valueMatchesType(value, getTypeName(max))) return false;
        double val = getNumericValue(value);
        double maxVal = getNumericValue(max);
        return val <= maxVal;
    }
};

// Configuration validation exception
class ConfigValidationException : public std::runtime_error {
public:
    ConfigValidationException(const std::string& message) : std::runtime_error(message) {}
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * Load from file (JSON format) with validation
     * 
     * @param filepath Path to configuration file
     * @param strictMode If true, fail on validation errors; if false, continue with warnings
     * @return true if file was loaded successfully (even with validation warnings)
     */
    bool loadFromFile(const std::string& filepath, bool strictMode = false);
    
    /**
     * Load from command line arguments with validation
     * 
     * @param argc Argument count
     * @param argv Argument array
     * @param strictMode If true, fail on validation errors; if false, continue with warnings
     * @return true if arguments were processed successfully (even with validation warnings)
     */
    bool loadFromArgs(int argc, char** argv, bool strictMode = false);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set values with validation
    /**
     * Set configuration value with validation
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     * @param validate Whether to validate against schema
     * @return true if value was set successfully
     */
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Convenience setters with validation
    bool set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation info
    std::string summary() const;
    
    // Configuration validation and schema management
    /**
     * Add configuration schema for validation
     * 
     * @param schema Configuration schema definition
     */
    void addSchema(const ConfigSchema& schema);
    
    /**
     * Validate entire configuration against schema
     * 
     * @return Validation result with errors and warnings
     */
    ValidationResult validate() const;
    
    /**
     * Validate specific key against schema
     * 
     * @param key Configuration key to validate
     * @return Validation result for the specific key
     */
    ValidationResult validateKey(const std::string& key) const;
    
    /**
     * Get validation errors for specific key
     * 
     * @param key Configuration key
     * @return Vector of error messages, empty if valid
     */
    std::vector<std::string> getValidationErrors(const std::string& key) const;
    
    /**
     * Get validation warnings for specific key
     * 
     * @param key Configuration key
     * @return Vector of warning messages
     */
    std::vector<std::string> getValidationWarnings(const std::string& key) const;
    
    /**
     * Override validation for specific key
     * 
     * @param key Configuration key
     * @param override Whether to override validation for this key
     */
    void setValidationOverride(const std::string& key, bool override);
    
    /**
     * Get configuration statistics
     * 
     * @return Map of key to entry count statistics
     */
    std::unordered_map<std::string, int> getStatistics() const;
    
    /**
     * Merge configuration with another config object
     * 
     * @param other Other configuration to merge
     * @param override Whether to override existing values
     * @return true if merge was successful
     */
    bool merge(const Config& other, bool override = true);
    
    /**
     * Export configuration to string
     * 
     * @param format Output format (json, ini, xml)
     * @return Configuration as string
     */
    std::string exportConfig(const std::string& format = "json") const;
    
    /**
     * Import configuration from string
     * 
     * @param configString Configuration string
     * @param format Input format (json, ini, xml)
     * @return true if import was successful
     */
    bool importConfig(const std::string& configString, const std::string& format = "json");
    
    /**
     * Get configuration keys sorted by importance/source
     * 
     * @return Vector of configuration keys
     */
    std::vector<std::string> getSortedKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Command line argument parsing
    struct ArgInfo {
        std::string key;
        std::string value;
        bool isValid;
        std::string error;
    };
    
    static std::vector<ArgInfo> parseCommandLineArgs(int argc, char** argv);
    static std::optional<ConfigValue> convertStringToValue(const std::string& strValue);
};

} // namespace nlm
