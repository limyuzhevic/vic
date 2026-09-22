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
    
    // Configuration version
    void setVersion(int version) { version_ = version; }
    int getVersion() const { return version_; }
    
    // Feature flags
    void setFeatureFlag(const std::string& feature, bool enabled) {
        featureFlags_[feature] = enabled;
    }
    bool isFeatureEnabled(const std::string& feature) const {
        auto it = featureFlags_.find(feature);
        return it != featureFlags_.end() ? it->second : false;
    }
    
    // Get all feature flags
    std::map<std::string, bool> getFeatureFlags() const { return featureFlags_; }
    
    // Advanced options
    void setAdvancedOption(const std::string& option, const std::string& value) {
        advancedOptions_[option] = value;
    }
    std::string getAdvancedOption(const std::string& option) const {
        auto it = advancedOptions_.find(option);
        return it != advancedOptions_.end() ? it->second : "";
    }
    
    // Get all advanced options
    std::map<std::string, std::string> getAdvancedOptions() const { return advancedOptions_; }
    
    // Simulation settings
    void setMaxSteps(size_t steps) { maxSimulationSteps_ = steps; }
    size_t getMaxSteps() const { return maxSimulationSteps_; }
    
    void setTimeLimit(double time) { simulationTimeLimit_ = time; }
    double getTimeLimit() const { return simulationTimeLimit_; }
    
    void setCheckpointInterval(size_t interval) { checkpointInterval_ = interval; }
    size_t getCheckpointInterval() const { return checkpointInterval_; }
    
    void setMemoryCapacity(size_t capacity) { memoryCapacity_ = capacity; }
    size_t getMemoryCapacity() const { return memoryCapacity_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Metadata
    int version_;
    
    // Feature flags
    std::map<std::string, bool> featureFlags_;
    
    // Advanced options
    std::map<std::string, std::string> advancedOptions_;
    
    // Simulation settings
    size_t maxSimulationSteps_;
    double simulationTimeLimit_;
    size_t checkpointInterval_;
    size_t memoryCapacity_;
};

} // namespace nlm
