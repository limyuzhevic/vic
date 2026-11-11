#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
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
    
    #include <functional>
#include <unordered_map>
#include <string>

namespace nlm {

/**
 * Configuration validation system
 * 
 * Provides validation for configuration values with custom validators for
 * different types of configuration parameters.
 */
class ConfigValidator {
public:
    using Validator = std::function<bool(const ConfigValue&, std::string& errorMessage)>;
    
    ConfigValidator() = default;
    
    /**
     * Register a validator for a configuration key
     * @param key Configuration key to validate
     * @param validator Function that returns true if valid, false with error message if invalid
     * @param description Description of the validation rule
     */
    void registerValidator(const std::string& key, 
                          Validator validator,
                          const std::string& description = "");
    
    /**
     * Validate a configuration value for a key
     * @param key Configuration key
     * @param value Configuration value to validate
     * @return true if valid, false with error message if invalid
     */
    bool validate(const std::string& key, const ConfigValue& value, 
                  std::string& errorMessage) const;
    
    /**
     * Validate an entire configuration
     * @param config Configuration to validate
     * @return true if valid, false with error message if invalid
     */
    bool validate(const Config& config, std::string& errorMessage) const;
    
    /**
     * Validate a configuration file
     * @param filepath Path to configuration file (JSON)
     * @return true if valid, false with error message if invalid
     */
    bool validateFile(const std::string& filepath, std::string& errorMessage) const;
    
    /**
     * Get all registered validators
     * @return Vector of pairs (key, description)
     */
    std::vector<std::pair<std::string, std::string>> getRegisteredValidators() const;
    
    /**
     * Remove validator for a key
     * @param key Configuration key
     */
    void removeValidator(const std::string& key);
    
    /**
     * Clear all validators
     */
    void clearValidators();
    
    /**
     * Get default validators for NLM configuration
     */
    void setupDefaultValidators();
    
    /**
     * Add validator for numeric range
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @param inclusive Whether to include min/max in valid range
     */
    static Validator makeRangeValidator(double min, double max, bool inclusive = true);
    
    /**
     * Add validator for positive values
     */
    static Validator makePositiveValidator();
    
    /**
     * Add validator for string length range
     */
    static Validator makeStringLengthValidator(size_t minLength, size_t maxLength);
    
    /**
     * Add validator for enum values
     */
    static Validator makeEnumValidator(const std::vector<std::string>& validValues);
    
    /**
     * Add validator for connectivity probability (0.0 to 1.0)
     */
    static Validator makeConnectivityValidator();
    
    /**
     * Add validator for timestep (non-negative, reasonable value)
     */
    static Validator makeTimestepValidator();
    
    /**
     * Add validator for neuron count (reasonable range)
     */
    static Validator makeNeuronCountValidator();
    
    /**
     * Add validator for region count (reasonable range)
     */
    static Validator makeRegionCountValidator();
    
    /**
     * Add validator for plasticity parameters (0.0 to 1.0)
     */
    static Validator makePlasticityValidator();
    
    /**
     * Add validator for neuromodulation levels (0.0 to 1.0)
     */
    static Validator makeNeuromodulationValidator();
    
    /**
     * Add validator for random seed (reasonable range)
     */
    static Validator makeRandomSeedValidator();
    
    /**
     * Add validator for simulation steps (reasonable range)
     */
    static Validator makeSimulationStepsValidator();
    
    /**
     * Add validator for checkpoint directory
     */
    static Validator makeCheckpointDirValidator();
    
    /**
     * Add validator for logging level
     */
    static Validator makeLogLevelValidator();
    
    /**
     * Add validator for visualization settings
     */
    static Validator makeVisualizationValidator();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
