#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Agent-specific configuration
struct AgentConfig {
    // Sensory processing parameters
    double visionScaleFactor = 5.0;
    double touchScaleFactor = 8.0;
    double internalScaleFactor = 5.0;
    double proprioceptionScaleFactor = 3.0;
    
    // Motor decoding parameters
    float activityThreshold = 0.5f;
    float curiosityThreshold = 0.3f;
    float explorationFactor = 0.3f;
    
    // Neuromodulation parameters
    float rewardModulationFactor = 1.0f;
    float dopamineLearningRate = 0.01f;
    
    // Development parameters
    double developmentalStage1End = 60.0;    // ~1 minute
    double developmentalStage2End = 300.0;  // ~5 minutes
    double developmentalStage3End = 900.0;  // ~15 minutes
    
    // Novelty detection
    float noveltyDecayRate = 0.99f;
    
    // Memory parameters
    float memoryRetentionRate = 0.95f;
    
    // Exploration
    float curiositySensitivity = 2.0f;
    float predictionErrorWeight = 0.5f;
    
    // Performance
    size_t sensoryVisionWidth = 16;
    size_t sensoryVisionHeight = 16;
    size_t sensoryTouchCount = 8;
    size_t sensoryInternalCount = 4;
    size_t sensoryProprioceptionCount = 6;
    size_t motorCommandCount = 6;
    
    // Validation
    bool validate() const {
        return visionScaleFactor > 0.0 && touchScaleFactor > 0.0 &&
               internalScaleFactor > 0.0 && proprioceptionScaleFactor > 0.0 &&
               activityThreshold >= 0.0f && activityThreshold <= 1.0f &&
               curiosityThreshold >= 0.0f && curiosityThreshold <= 1.0f &&
               noveltyDecayRate > 0.0f && noveltyDecayRate <= 1.0f;
    }
};

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

// Configuration converter utilities
class ConfigConverter {
public:
    // Convert Config to AgentConfig
    static AgentConfig toAgentConfig(const Config& config);
    
    // Convert AgentConfig to Config
    static void fromAgentConfig(Config& config, const AgentConfig& agentConfig);
    
    // Validate AgentConfig against configuration bounds
    static std::vector<std::string> validateAgentConfig(const AgentConfig& config);
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
