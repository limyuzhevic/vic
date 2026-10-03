#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    
    // === ADVANCED CONFIGURATION FEATURES ===
    
    // Validation functions
    bool validate() const;
    std::string validateAndGetErrors() const;
    
    // Preset configurations for common use cases
    static Config createQuickTestConfig();
    static Config createPerformanceConfig();
    static Config createResearchConfig();
    static Config createVisualizationConfig();
    
    // Validate specific parameter ranges
    static bool validateNeuronCount(size_t count);
    static bool validateSynapseDensity(double density);
    static bool validateLearningRate(double rate);
    static bool validateMemoryCapacity(size_t capacity);
    
    // Get parameter ranges for validation
    static std::pair<size_t, size_t> getNeuronCountRange();
    static std::pair<double, double> getSynapseDensityRange();
    static std::pair<double, double> getLearningRateRange();
    
    // Merge configuration from multiple sources
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Check if configuration is complete (has all required parameters)
    bool isComplete() const;
    
    // Get missing required parameters
    std::vector<std::string> getMissingParameters() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helper methods
    void initializeValidationRules();
    bool validateParameter(const ConfigEntry& entry) const;
    static bool isParameterValid(const std::string& key, const ConfigValue& value);
    
    // Preset configuration implementations
    static Config createQuickTestPreset();
    static Config createPerformancePreset();
    static Config createResearchPreset();
    static Config createVisualizationPreset();
    };

} // namespace nlm
