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

// Configuration constants for default values
namespace ConfigConstants {
    // Default simulation parameters
    constexpr double DefaultTimestep = 0.001;
    constexpr int64_t DefaultNeuronCount = 500;
    constexpr int64_t DefaultRegionCount = 1;
    constexpr float DefaultConnectionProbability = 0.15f;
    
    // STDP parameters
    constexpr float STDP_LTPWeight = 0.02f;
    constexpr float STDP_LTDWeight = 0.015f;
    constexpr float STDP_TimeConstant = 20.0f;
    
    // Structural plasticity parameters
    constexpr float SynaptogenesisRate = 0.0001f;
    constexpr float PruningRate = 0.00001f;
    
    // Neuromodulation parameters
    constexpr float DopamineBaseline = 0.1f;
    
    // Reward parameters
    constexpr float RewardDiscountFactor = 0.99f;
    
    // Logging parameters
    constexpr int LogLevel = 1;  // INFO level
    constexpr bool LogToFile = false;
    constexpr const char* LogFilename = "nlm.log";
    
    // Simulation parameters
    constexpr size_t MaxSteps = 10000;
    constexpr double TimeLimit = 0.0;
    
    // Visualization parameters
    constexpr bool VisualizationEnabled = false;
    constexpr float VisualizationUpdateRate = 30.0f;
}

// Configuration validation
class ConfigValidator {
public:
    static bool validateConfig(const Config& config);
    static std::string validateValue(const std::string& key, const ConfigValue& value);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::string& key, const ConfigValue& value);
    static std::string getKeyType(const std::string& key);
};

// Configuration validation constants
namespace ValidationConstants {
    constexpr double MinTimestep = 0.0001;
    constexpr double MaxTimestep = 1.0;
    constexpr int64_t MinNeuronCount = 1;
    constexpr int64_t MaxNeuronCount = 100000;
    constexpr int64_t MinRegionCount = 1;
    constexpr int64_t MaxRegionCount = 1000;
    constexpr float MinConnectionProbability = 0.0f;
    constexpr float MaxConnectionProbability = 1.0f;
    constexpr float MinSTDPWeight = 0.0f;
    constexpr float MaxSTDPWeight = 10.0f;
    constexpr float MinSynaptogenesisRate = 0.0f;
    constexpr float MaxSynaptogenesisRate = 1.0f;
    constexpr float MinPruningRate = 0.0f;
    constexpr float MaxPruningRate = 0.1f;
    constexpr float MinDopamineBaseline = 0.0f;
    constexpr float MaxDopamineBaseline = 10.0f;
    constexpr float MinRewardDiscountFactor = 0.0f;
    constexpr float MaxRewardDiscountFactor = 1.0f;
    constexpr float MinLogLevel = 0;
    constexpr float MaxLogLevel = 4;
    constexpr const char* ValidEnvironmentNames[] = {"GridWorld", "ContinuousSpace", "EmptyWorld", nullptr};
}

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
