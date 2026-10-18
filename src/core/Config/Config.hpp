#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <array>

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

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    std::array<size_t, 4> entriesBySource; // Index corresponds to ConfigSource enum
    size_t stringValues;
    size_t numericValues;
    size_t boolValues;
    size_t arrayValues;
    
    ConfigStats() : totalEntries(0), entriesBySource{{0, 0, 0, 0}}, 
                   stringValues(0), numericValues(0), boolValues(0), arrayValues(0) {}
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
    
    // Enhanced get with fallback
    template<typename T>
    T getWithFallback(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced set with validation
    void setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration management
    void setBatch(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source);
    void applyOverrides(const Config& overrides);
    bool mergeConfig(const Config& otherConfig);
    
    // Filtered views
    std::vector<ConfigEntry> getFilteredEntries(ConfigSource source) const;
    std::string getConfigSummary(ConfigSource source) const;
    
    // Configuration statistics
    ConfigStats getStats() const;
    
    // Validation
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateKeyFormat(const std::string& key);
    
    // Factory methods for common configurations
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createFastSimulationConfig();
    static Config createAccurateSimulationConfig();
    
    // Configuration presets
    static const std::unordered_map<std::string, Config> PRESETS;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool isNumber(const std::string& str);
    bool validateNeuronConfig(const std::string& key, const ConfigValue& value) const;
    bool validateSynapseConfig(const std::string& key, const ConfigValue& value) const;
    bool validateSimulationConfig(const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm

// Template method implementations (inline for efficiency)
namespace nlm {
    template<typename T>
    T Config::getWithFallback(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
}
