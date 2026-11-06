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

// Configuration parameter validation result
enum class ValidationResult {
    Success,
    InvalidType,
    OutOfRange,
    MissingRequired,
    InvalidFormat,
    SchemaViolation
};

// Configuration parameter schema
struct ParameterSchema {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> validator;
    
    ParameterSchema() : key(), description(), defaultSource(ConfigSource::Default), validator(nullptr) {}
    
    ParameterSchema(const std::string& k, const std::string& desc, ConfigSource src = ConfigSource::Default,
                   const std::vector<std::string>& allowed = {})
        : key(k), description(desc), defaultSource(src), allowedValues(allowed) {
        validator = [this](const ConfigValue& value) -> bool {
            return validateValue(value);
        };
    }
    
    bool validateValue(const ConfigValue& value) const {
        if (validator) {
            return validator(value);
        }
        return true;
    }
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable ValidationResult validationResult;
    mutable std::string validationError;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    validationResult(ValidationResult::Success), validationError() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          validationResult(ValidationResult::Success), validationError() {}
};

// Exception hierarchy for configuration errors
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& key, ValidationResult result, const std::string& error)
        : ConfigException("Configuration validation error for key \"" + key + "\": " + error),
          key_(key), result_(result), error_(error) {}
    
    const std::string& key() const { return key_; }
    ValidationResult result() const { return result_; }
    const std::string& error() const { return error_; }
    
private:
    std::string key_;
    ValidationResult result_;
    std::string error_;
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& filepath, const std::string& error)
        : ConfigException("Failed to load configuration from \"" + filepath + "\": " + error) {}
};

class ConfigSaveException : public ConfigException {
public:
    explicit ConfigSaveException(const std::string& filepath, const std::string& error)
        : ConfigException("Failed to save configuration to \"" + filepath + "\": " + error) {}
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
    
    // Set values with validation
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
    
    // Validation methods
    ValidationResult validate(const std::string& key) const;
    std::string validationError(const std::string& key) const;
    bool isValid(const std::string& key) const;
    std::vector<std::string> getInvalidKeys() const;
    
    // Schema management
    void registerParameterSchema(const ParameterSchema& schema);
    bool hasSchema(const std::string& key) const;
    const ParameterSchema& getSchema(const std::string& key) const;
    
    // Parameter validation helpers
    static bool validateIntRange(const ConfigValue& value, int min, int max);
    static bool validateDoubleRange(const ConfigValue& value, double min, double max);
    static bool validateStringChoices(const ConfigValue& value, const std::vector<std::string>& choices);
    static bool validateVectorSize(const ConfigValue& value, size_t minSize, size_t maxSize);
    
    // Configuration parameter constants
    static const std::string NEURON_COUNT;
    static const std::string SYNAPSE_DENSITY;
    static const std::string CONNECTION_PROBABILITY;
    static const std::string INITIAL_WEIGHT_MEAN;
    static const std::string INITIAL_WEIGHT_STD;
    static const std::string V_THRESH;
    static const std::string V_REST;
    static const std::string V_RESET;
    static const std::string TAU_MEM;
    static const std::string TAU_REF;
    static const std::string STDP_ENABLE;
    static const std::string STDP_LEARNING_RATE;
    static const std::string STDP_TAU_PLUS;
    static const std::string STDP_TAU_MINUS;
    static const std::string HEBBIAN_ENABLE;
    static const std::string STRUCTURAL_PLASTICITY_ENABLE;
    static const std::string SYNAPTOGENESIS_RATE;
    static const std::string PRUNING_RATE;
    static const std::string DOPAMINE_SCALE;
    static const std::string CURIOUSITY_ENABLE;
    static const std::string NOVELTY_ENABLE;
    static const std::string PREDICTION_ERROR_WEIGHT;
    static const std::string DEVELOPMENT_ENABLE;
    static const std::string DEVELOPMENT_STAGE;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Parameter schema registry
    std::unordered_map<std::string, ParameterSchema> parameterSchemas;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    ValidationResult validateEntry(const ConfigEntry& entry) const;
    void validateAndStore(const std::string& key, const ConfigValue& value, ConfigSource source);
};

} // namespace nlm
