#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class ConfigExtended;
class ValidationSystem;
class RuntimeConfig;
class ConfigurationManager;

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

// Advanced brain configuration
struct AdvancedBrainConfig {
    size_t neuronCount = 2000;
    size_t regionCount = 4;
    float threshold = -50.0f;
    float restingPotential = -70.0f;
    float resetPotential = -75.0f;
    float membraneTimeConstant = 20.0f;
    float refractoryPeriod = 2.0f;
    std::string neuronModel = "LIF";
    bool enableSpiking = true;
    float decayRate = 0.95f;
    float connectionProbability = 0.05f;
    std::string weightDistribution = "gaussian";
    float weightMean = 0.5f;
    float weightStd = 0.1f;
    float delay = 0.001f;
    bool enablePlasticity = true;
    std::string plasticityType = "STDP";
};

// Neuromodulation configuration
struct NeuromodulationConfig {
    struct DopamineConfig {
        bool enable = true;
        float scale = 2.0f;
        float plasticityFactor = 1.5f;
        float excitabilityFactor = 0.8f;
        bool learningRateModulation = true;
        float rewardPredictionErrorGain = 0.1f;
    };

    struct CuriosityConfig {
        bool enable = true;
        float explorationRate = 0.3f;
        float noveltyThreshold = 0.5f;
        std::string rewardShaping = "inverse";
        float temperature = 1.0f;
        float intrinsicMotivation = 0.5f;
        bool explorationBonus = true;
    };

    DopamineConfig dopamine;
    CuriosityConfig curiosity;
    float neuromodulationStrength = 1.0f;
};

// Memory system configuration
struct MemoryConfig {
    struct WorkingMemoryConfig {
        size_t capacity = 500;
        float decayRate = 0.95f;
        std::string competitionMethod = "soft_max";
        float encodingStrength = 1.0f;
        std::string forgettingMechanism = "competitive";
        bool enablePatternCompletion = true;
    };

    WorkingMemoryConfig workingMemory;
};

// Configuration extension methods (for backward compatibility)
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Original configuration methods
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Enhanced configuration methods
    bool loadFromJSON(const std::string& jsonData);
    std::string toJSON() const;
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Advanced configuration getters
    AdvancedBrainConfig getAdvancedBrainConfig() const;
    NeuromodulationConfig getNeuromodulationConfig() const;
    MemoryConfig getMemoryConfig() const;
    
    // Configuration optimization
    void optimizeForPerformance();
    void optimizeForLearning();
    void optimizeForBiologicalAccuracy();
    void optimizeForSpeed();
    
    // Configuration metadata
    void addParameterMetadata(const std::string& key, const std::string& description,
                             const std::string& unit, const std::string& validationRules);
    std::string getParameterMetadata(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON conversion helpers
    static ConfigValue jsonToConfigValue(const nlohmann::json& j);
    static nlohmann::json configValueToJson(const ConfigValue& value);
};

} // namespace nlm
