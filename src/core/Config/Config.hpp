#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>

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

// Validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string warningMessage;
    
    ValidationResult(bool valid = true, const std::string& error = "",
                     const std::string& warning = "")
        : isValid(valid), errorMessage(error), warningMessage(warning) {}
};

// Configuration constraint
struct ConfigConstraint {
    std::string name;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string warningMessage;
    
    ConfigConstraint(const std::string& n, 
                     const std::function<ValidationResult(const ConfigValue&)>& v,
                     const std::string& err = "",
                     const std::string& warn = "")
        : name(n), validator(v), errorMessage(err), warningMessage(warn) {}
};

// Default configuration parameters
struct DefaultConfigParams {
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    double simulationTimestep = 0.001;
    int64_t randomSeed = 42;
    
    // STDP parameters
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTau = 20.0f;
    
    // Plasticity parameters
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Neuromodulation parameters
    float dopamineSensitivity = 1.0f;
    float curiosityThreshold = 0.3f;
    float noveltyDecay = 0.99f;
    
    // Memory parameters
    size_t workingMemoryCapacity = 1000;
    size_t episodicMemoryCapacity = 500;
    
    // Development parameters
    double criticalPeriodDuration = 60.0;  // seconds
    double maturationDuration = 300.0;    // seconds
    
    // Performance parameters
    bool enableSIMD = true;
    bool enableParallel = false;
    size_t maxThreads = 4;
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
