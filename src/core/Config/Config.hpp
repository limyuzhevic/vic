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

// Parameter validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Parameter metadata
struct ParameterMetadata {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    bool required;
    ValidationFunction validator;
    
    ParameterMetadata() : key(), description(), defaultSource(ConfigSource::Default), 
                          required(false), validator(nullptr) {}
    
    ParameterMetadata(const std::string& k, const std::string& desc, ConfigSource source = ConfigSource::Default,
                     bool req = false, const ValidationFunction& val = nullptr)
        : key(k), description(desc), defaultSource(source), required(req), validator(val) {}
    
    bool validate(const ConfigValue& value) const {
        if (validator) {
            return validator(value);
        }
        return true; // No validation means always valid
    }
};

// Configuration exception types
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& key, const std::string& message)
        : ConfigException("Configuration validation failed for key '" + key + "': " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& filepath, const std::string& message)
        : ConfigException("Failed to load configuration from file '" + filepath + "': " + message) {}
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

// Main configuration class with enhanced validation and error handling
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Register parameter schema with validation
    template<typename T>
    void registerParameter(const std::string& key, const std::string& description,
                           const T& defaultValue, ConfigSource source = ConfigSource::Default,
                           bool required = false, const ValidationFunction& validator = nullptr);
    
    // Get parameter metadata
    std::optional<ParameterMetadata> getParameterMetadata(const std::string& key) const;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Validate entire configuration
    std::vector<std::string> validate() const;
    
    // Get configuration errors
    std::vector<std::string> getErrors() const { return errors_; }
    
    // Clear errors
    void clearErrors() { errors_.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Parameter registry for validation
    std::vector<ParameterMetadata> parameterRegistry_;
    
    // Validation errors
    std::vector<std::string> errors_;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseBool(const std::string& str);
    
    // Internal validation helpers
    template<typename T>
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    void addError(const std::string& error) { errors_.push_back(error); }
};

// Predefined validation functions
namespace ConfigValidation {
    inline bool positiveDouble(const ConfigValue& value) {
        if (auto* v = std::get_if<double>(&value)) {
            return *v > 0.0;
        }
        return false;
    }
    
    inline bool nonNegativeDouble(const ConfigValue& value) {
        if (auto* v = std::get_if<double>(&value)) {
            return *v >= 0.0;
        }
        return false;
    }
    
    inline bool positiveInt(const ConfigValue& value) {
        if (auto* v = std::get_if<int>(&value)) {
            return *v > 0;
        }
        return false;
    }
    
    inline bool rangeDouble(double min, double max) {
        return [min, max](const ConfigValue& value) {
            if (auto* v = std::get_if<double>(&value)) {
                return *v >= min && *v <= max;
            }
            return false;
        };
    }
    
    inline bool rangeInt(int min, int max) {
        return [min, max](const ConfigValue& value) {
            if (auto* v = std::get_if<int>(&value)) {
                return *v >= min && *v <= max;
            }
            return false;
        };
    }
    
    inline bool stringNotEmpty(const ConfigValue& value) {
        if (auto* v = std::get_if<std::string>(&value)) {
            return !v->empty();
        }
        return false;
    }
    
    inline bool boolValue(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
}

} // namespace nlm
