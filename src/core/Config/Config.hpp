#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
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

// Validation error types
enum class ValidationErrorType {
    None,
    InvalidType,
    OutOfRange,
    InvalidValue,
    MissingRequired,
    InvalidFormat
};

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, ValidationErrorType type = ValidationErrorType::InvalidValue, 
                   const std::string& key = "", const std::string& expected = "")
        : std::runtime_error(message), errorType(type), configKey(key), expectedValue(expected) {}
    
    ValidationErrorType getType() const { return errorType; }
    const std::string& getKey() const { return configKey; }
    const std::string& getExpected() const { return expectedValue; }
    
private:
    ValidationErrorType errorType;
    std::string configKey;
    std::string expectedValue;
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

// Configuration validation descriptor
struct ConfigValidationDescriptor {
    std::string key;
    std::string type;
    std::string description;
    bool required = false;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    std::string defaultValue;
    
    ConfigValidationDescriptor() = default;
    ConfigValidationDescriptor(const std::string& k, const std::string& t, const std::string& desc, bool req = false)
        : key(k), type(t), description(desc), required(req) {}
};

// Configuration schema for validation
class ConfigSchema {
public:
    void addDescriptor(const ConfigValidationDescriptor& descriptor) {
        descriptors[descriptor.key] = descriptor;
    }
    
    bool hasDescriptor(const std::string& key) const {
        return descriptors.find(key) != descriptors.end();
    }
    
    const ConfigValidationDescriptor* getDescriptor(const std::string& key) const {
        auto it = descriptors.find(key);
        return it != descriptors.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> getRequiredKeys() const {
        std::vector<std::string> required;
        for (const auto& pair : descriptors) {
            if (pair.second.required) {
                required.push_back(pair.first);
            }
        }
        return required;
    }
    
    void clear() {
        descriptors.clear();
    }
    
private:
    std::map<std::string, ConfigValidationDescriptor> descriptors;
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
    
    // Validation methods
    void addValidationDescriptor(const ConfigValidationDescriptor& descriptor);
    bool validate() const;
    std::vector<ValidationError> validateWithDetails() const;
    ValidationError getLastValidationError() const;
    bool setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const;
    void clearSchema();
    
    // Bounds checking helpers
    template<typename T>
    static bool isInBounds(const T& value, const std::optional<double>& min, const std::optional<double>& max);
    
    // Value conversion and validation helpers
    static std::string configValueToString(const ConfigValue& value);
    static bool configValueToBool(const ConfigValue& value);
    static double configValueToDouble(const ConfigValue& value);
    static int64_t configValueToInt64(const ConfigValue& value);
    
    // Configuration key constants
    static const std::string BRAIN_NEURON_COUNT;
    static const std::string BRAIN_SYNAPSE_DENSITY;
    static const std::string BRAIN_CONNECTION_PROBABILITY;
    static const std::string PLASTICITY_STDP_ENABLE;
    static const std::string PLASTICITY_STDP_LEARNING_RATE;
    static const std::string NEUROMOD_DOPAMINE_SCALE;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation internal methods
    static ValidationError validateSingleValue(const std::string& key, const ConfigValue& value, const ConfigValidationDescriptor& descriptor);
    static std::string valueTypeToString(const ConfigValue& value);
    static bool isNumber(const ConfigValue& value, double& outValue);
};

} // namespace nlm
