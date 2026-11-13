#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
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
    bool isValid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult(bool valid = true, const std::string& error = "", 
                    const std::vector<std::string>& warnings = std::vector<std::string>())
        : isValid(valid), errorMessage(error), warnings(warnings) {}
};

// Configuration validation function type
using ValidationFunc = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ValidationFunc validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validator(nullptr) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const ValidationFunc& val = nullptr)
        : key(k), value(v), source(s), description(desc), validator(val) {}
};

// Neural network configuration constants
struct NeuralNetworkDefaults {
    static constexpr int64_t defaultNeuronCount = 500;
    static constexpr int64_t defaultRegionCount = 1;
    static constexpr double defaultSimulationTimestep = 0.001;
    static constexpr int64_t defaultRandomSeed = 42;
    static constexpr float defaultConnectionProbability = 0.15f;
    static constexpr float defaultStdpLtpWeight = 0.02f;
    static constexpr float defaultStdpLtdWeight = 0.015f;
    static constexpr float defaultStdpTau = 20.0f;
    static constexpr float defaultSynaptogenesisRate = 0.0001f;
    static constexpr float defaultPruningRate = 0.00001f;
    static constexpr int64_t defaultReplayInterval = 100;
    static constexpr int64_t defaultConsolidationInterval = 1000;
    static constexpr int64_t defaultMaxMemoryMB = 0;  // Unlimited
    static constexpr size_t defaultWorkingMemoryCapacity = 50;
    static constexpr size_t defaultMaxEpisodes = 1000;
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
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
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
    
    // Set with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    ValidationResult validate() const;
    
    // Get configuration description
    std::string getDescription(const std::string& key) const;
    
    // Get configuration source
    ConfigSource getSource(const std::string& key) const;
    
    // Check if configuration has been overridden
    bool isOverridden(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractJsonValue(const std::string& jsonString, const std::string& key);
};

// Configuration validation functions
ValidationResult validateNeuronCount(const std::string& key, const ConfigValue& value);
ValidationResult validateRegionCount(const std::string& key, const ConfigValue& value);
ValidationResult validateSimulationTimestep(const std::string& key, const ConfigValue& value);
ValidationResult validateRandomSeed(const std::string& key, const ConfigValue& value);
ValidationResult validateConnectionProbability(const std::string& key, const ConfigValue& value);
ValidationResult validateStdpLtpWeight(const std::string& key, const ConfigValue& value);
ValidationResult validateStdpLtdWeight(const std::string& key, const ConfigValue& value);
ValidationResult validateStdpTau(const std::string& key, const ConfigValue& value);
ValidationResult validateSynaptogenesisRate(const std::string& key, const ConfigValue& value);
ValidationResult validatePruningRate(const std::string& key, const ConfigValue& value);
ValidationResult validateReplayInterval(const std::string& key, const ConfigValue& value);
ValidationResult validateConsolidationInterval(const std::string& key, const ConfigValue& value);
ValidationResult validateMaxMemoryMB(const std::string& key, const ConfigValue& value);
ValidationResult validateWorkingMemoryCapacity(const std::string& key, const ConfigValue& value);
ValidationResult validateMaxEpisodes(const std::string& key, const ConfigValue& value);

} // namespace nlm
