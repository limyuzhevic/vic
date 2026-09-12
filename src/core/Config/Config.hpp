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
    
    // Configuration validation
    bool validate() const;
    
    // Convenience methods for setting related parameters
    void setMemorySystemSize(size_t workingMemorySize, size_t maxEpisodicEpisodes);
    void setDevelopmentParameters(double criticalPeriodStart, double maturationRate);
    void setNeuromodulationParameters(float dopamineScale, float curiositySensitivity);
    void setPredictionSystemParameters(int horizon, float confidenceThreshold);
    void setStructuralPlasticityParameters(float synaptogenesisProb, float pruningThreshold);
    void setDevelopmentStages(const std::vector<double>& stageTransitions, float agingFactor);
    void setAttentionSystemParameters(float inhibitionStrength, float excitationStrength);
    void setConceptFormationParameters(float patternDiscoveryThreshold);
    void setPlanningParameters(int maxDepth, int complexity);
    void setLearningParameters(float learningRate, float decayConstant);
    
    // Batch setter methods
    void setAllMemoryParameters(const std::string& workingMemoryKey, const std::string& episodicMemoryKey);
    void setAllDevelopmentParameters(const std::string& criticalPeriodKey, const std::string& maturationKey);
    void setAllNeuromodulationParameters(const std::string& dopamineKey, const std::string& curiosityKey);
    void setAllPredictionParameters(const std::string& horizonKey, const std::string& confidenceKey);
    void setAllStructuralPlasticityParameters(const std::string& synaptogenesisKey, const std::string& pruningKey);
    void setAllDevelopmentStageParameters(const std::string& transitionsKey, const std::string& agingKey);
    void setAllAttentionParameters(const std::string& inhibitionKey, const std::string& excitationKey);
    void setAllConceptParameters(const std::string& patternDiscoveryKey);
    void setAllPlanningParameters(const std::string& depthKey, const std::string& complexityKey);
    void setAllLearningParameters(const std::string& learningRateKey, const std::string& decayConstantKey);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
