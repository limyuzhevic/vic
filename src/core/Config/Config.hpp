#pragma once

/**
 * @file Config.hpp
 * @brief Configuration system header for NLM
 * 
 * This header defines the configuration system that provides type-safe access to all NLM parameters.
 * The configuration system supports loading from configuration files, command-line arguments, and runtime settings.
 * 
 * The configuration system provides:
 * - Type-safe access to all configuration parameters
 * - Validation of configuration values
 * - Loading from file (key=value format)
 * - Loading from command-line arguments
 * - Runtime configuration management
 * 
 * Configuration parameters include:
 * - Simulation parameters (timestep, max steps, time limit)
 * - Brain parameters (neuron count, region count, connectivity)
 * - Plasticity parameters (STDP, Hebbian, structural)
 * - Development parameters (synaptogenesis, pruning, maturation)
 * - Neuromodulation parameters (dopamine, reward, curiosity)
 * - Environment parameters (world dimensions, vision)
 * - Logging and visualization parameters
 * 
 * @author NLM Team
 * @version 0.1.0
 * @date Phase 6 Integration
 */

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
    Default,     ///< Default configuration values
    File,        ///< Loaded from configuration file
    CommandLine, ///< Set via command-line arguments
    Runtime      ///< Set at runtime programmatically
};

// Configuration entry
struct ConfigEntry {
    std::string key;             ///< Configuration key
    ConfigValue value;           ///< Configuration value
    ConfigSource source;         ///< Source of this configuration value
    std::string description;     ///< Human-readable description
    
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
    
    // Validate configuration
    bool validate() const;
    
    // Get values - type-safe access
    int getOrRandomSeed(int defaultValue) const;
    double getOrSimulationTimestep(double defaultValue) const;
    int64_t getOrNeuronCount(int64_t defaultValue) const;
    int64_t getOrRegionCount(int64_t defaultValue) const;
    float getOrConnectionProbability(float defaultValue) const;
    float getOrPlasticityLearningRate(float defaultValue) const;
    float getOrStdpLtpWeight(float defaultValue) const;
    float getOrStdpLtdWeight(float defaultValue) const;
    float getOrStdpTimeConstant(float defaultValue) const;
    float getOrDevelopmentSynaptogenesisRate(float defaultValue) const;
    float getOrDevelopmentPruningRate(float defaultValue) const;
    float getOrDopamineBaseline(float defaultValue) const;
    float getOrRewardDiscountFactor(float defaultValue) const;
    std::string getOrEnvironmentName(const std::string& defaultValue) const;
    int getOrEnvironmentWidth(int defaultValue) const;
    int getOrEnvironmentHeight(int defaultValue) const;
    std::string getOrLogLevel(const std::string& defaultValue) const;
    bool getOrLogToFile(bool defaultValue) const;
    std::string getOrLogFilename(const std::string& defaultValue) const;
    int64_t getOrMaxSimulationSteps(int64_t defaultValue) const;
    double getOrSimulationTimeLimit(double defaultValue) const;
    bool getOrVisualizationEnabled(bool defaultValue) const;
    float getOrVisualizationUpdateRate(float defaultValue) const;
    
    // Set values - type-safe
    void setRandomSeed(int value, ConfigSource source = ConfigSource::Runtime);
    void setSimulationTimestep(double value, ConfigSource source = ConfigSource::Runtime);
    void setNeuronCount(int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setRegionCount(int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setConnectionProbability(float value, ConfigSource source = ConfigSource::Runtime);
    void setPlasticityLearningRate(float value, ConfigSource source = ConfigSource::Runtime);
    void setStdpLtpWeight(float value, ConfigSource source = ConfigSource::Runtime);
    void setStdpLtdWeight(float value, ConfigSource source = ConfigSource::Runtime);
    void setStdpTimeConstant(float value, ConfigSource source = ConfigSource::Runtime);
    void setDevelopmentSynaptogenesisRate(float value, ConfigSource source = ConfigSource::Runtime);
    void setDevelopmentPruningRate(float value, ConfigSource source = ConfigSource::Runtime);
    void setDopamineBaseline(float value, ConfigSource source = ConfigSource::Runtime);
    void setRewardDiscountFactor(float value, ConfigSource source = ConfigSource::Runtime);
    void setEnvironmentName(const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setEnvironmentWidth(int value, ConfigSource source = ConfigSource::Runtime);
    void setEnvironmentHeight(int value, ConfigSource source = ConfigSource::Runtime);
    void setLogLevel(const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setLogToFile(bool value, ConfigSource source = ConfigSource::Runtime);
    void setLogFilename(const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setMaxSimulationSteps(int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setSimulationTimeLimit(double value, ConfigSource source = ConfigSource::Runtime);
    void setVisualizationEnabled(bool value, ConfigSource source = ConfigSource::Runtime);
    void setVisualizationUpdateRate(float value, ConfigSource source = ConfigSource::Runtime);
    
    // Template-based access for custom keys
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set custom key
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
    
    // Internal validation helper
    static bool validateInternal(const Config* config);
};

} // namespace nlm


} // namespace nlm
