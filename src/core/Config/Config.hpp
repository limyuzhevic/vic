#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
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

// Advanced configuration categories
enum class ConfigCategory {
    Basic,
    Learning,
    Development,
    Neuromodulation,
    Memory,
    Prediction,
    Network,
    Environment,
    Debugging,
    Performance
};

// Configuration entry with advanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    std::string unit;
    std::string type;
    std::vector<std::string> validValues;
    double minValue;
    double maxValue;
    bool isAdvanced;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   category(ConfigCategory::Basic), unit(), type(), validValues(),
                   minValue(std::numeric_limits<double>::lowest()), 
                   maxValue(std::numeric_limits<double>::max()),
                   isAdvanced(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                ConfigCategory cat = ConfigCategory::Basic, const std::string& unitStr = "",
                const std::string& typeStr = "double", bool advanced = false)
        : key(k), value(v), source(s), description(desc), category(cat), unit(unitStr), 
          type(typeStr), validValues(), minValue(std::numeric_limits<double>::lowest()), 
          maxValue(std::numeric_limits<double>::max()), isAdvanced(advanced) {}
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
    
    // Get advanced configuration metadata
    std::optional<ConfigEntry> getEntryInfo(const std::string& key) const;
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    
    // Set values with metadata
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigCategory category = ConfigCategory::Basic,
             const std::string& unit = "", const std::string& type = "double", bool advanced = false);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigCategory category = ConfigCategory::Basic,
             const std::string& unit = "", const std::string& type = "string", bool advanced = false);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigCategory category = ConfigCategory::Basic,
             const std::string& unit = "", const std::string& type = "int", bool advanced = false);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigCategory category = ConfigCategory::Basic,
             const std::string& unit = "", const std::string& type = "double", bool advanced = false);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigCategory category = ConfigCategory::Basic,
             const std::string& unit = "", const std::string& type = "bool", bool advanced = false);
    
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
    
    // Get advanced configuration options for specific categories
    std::string getAdvancedOptionsSummary() const;
    
    // Validate configuration
    bool validateConfig() const;
    
    // Reset to defaults
    void resetToDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Load default configuration
    void loadDefaultConfiguration();
    
    // Apply legacy to modern parameter mapping
    void applyLegacyParameterMapping();
    
    // Validate all loaded parameters
    bool validateAllParameters() const;
    
    // Parameter validation functions
    bool validateRandomSeed(const std::string& value) const;
    bool validateTimestep(const std::string& value) const;
    bool validateNeuronCount(const std::string& value) const;
    bool validateRegionCount(const std::string& value) const;
    bool validateConnectionProbability(const std::string& value) const;
    bool validateLearningRate(const std::string& value) const;
    bool validateStdpWeight(const std::string& value) const;
    bool validateTimeConstant(const std::string& value) const;
    bool validateRate(const std::string& value) const;
    bool validateNeuromodulationValue(const std::string& value) const;
    bool validateDiscountFactor(const std::string& value) const;
    bool validateEnvironmentDimension(const std::string& value) const;
    bool validatePositiveFloat(const std::string& value) const;
    
    // Parse config line with validation
    bool parseConfigLine(const std::string& line, size_t lineNumber);
    
    // Is valid config key
    bool isValidConfigKey(const std::string& key) const;
    
    // Is advanced configuration key
    bool isAdvancedConfigKey(const std::string& key) const;
};

} // namespace nlm

// Include implementation headers
#include "ConfigImpl.hpp"
