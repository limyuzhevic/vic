#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <ctime>

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

// Main configuration class with advanced features for power users
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters with validation for power users
    void setBrainParameters(int neuronCount, int regionCount);
    void setPlasticityParameters(float stdpLTP, float stdpLTD, float stdpTimeConstant);
    void setNeuromodulationParameters(float dopamineScale, float curiosityRate, float noveltyThreshold);
    void setDevelopmentParameters(bool enableDevelopment, float maturationRate);
    void setPerformanceParameters(int maxSteps, float timestep, bool enableOptimization);
    
    // Advanced setters for expert users
    void setCustomSynapseParameters(float minWeight, float maxWeight, float initialWeightMean, float initialWeightStd);
    void setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate, float connectionProbability);
    void setMemorySystemParameters(size_t workingMemoryCapacity, size_t episodicMemoryCapacity, size_t associativeMemoryCapacity);
    void setPredictionSystemParameters(float predictionHorizon, float predictionAccuracyThreshold);
    void setCognitiveSystemParameters(float attentionStrength, float planningDepth, float conceptFormationRate);
    
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
    
    // Advanced configuration methods
    bool validateConfiguration() const;
    std::string getConfigurationError() const;
    std::string exportConfiguration(const std::string& format = "json") const;
    bool importConfiguration(const std::string& content, const std::string& format = "json");
    
    // Configuration comparison and backup
    bool equals(const Config& other) const;
    std::string getConfigurationHash() const;
    bool backupToFile(const std::string& filepath) const;
    bool restoreFromBackup(const std::string& filepath);
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    double getAverageComplexity() const;
    
    // Configuration evolution and versioning
    void setConfigurationVersion(int version);
    int getConfigurationVersion() const;
    bool isVersionCompatible(int minVersion) const;
    
    // Get current configuration for inspection
    std::vector<ConfigEntry> getEntries() const;
    ConfigEntry getEntry(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidNumber(const std::string& str);
    static bool isValidBoolean(const std::string& str);
    static std::string generateConfigurationHash(const std::vector<ConfigEntry>& entries);
};

} // namespace nlm
