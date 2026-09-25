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
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>,
    size_t
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
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get commonly used types
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<float> getFloat(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    
    // Set commonly used types
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setFloat(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validateConfig() const;
    
    // Default configuration initialization
    static Config getDefaultConfig();
    
    // Configuration version
    static constexpr const char* getConfigVersion() { return "1.0.0"; }
    
    // Configuration keys namespace (grouping related parameters)
    namespace ConfigKeys {
        // Neuron parameters
        const std::string NEURON_MEMBRANE_CAPACITANCE = "neuron.membrane_capacitance";
        const std::string NEURON_TIME_CONSTANT = "neuron.time_constant";
        const std::string NEURON_MAX_SPIKE_HISTORY = "neuron.max_spike_history";
        const std::string NEURON_RESTING_POTENTIAL = "neuron.resting_potential";
        const std::string NEURON_THRESHOLD = "neuron.threshold";
        const std::string NEURON_RESET_POTENTIAL = "neuron.reset_potential";
        const std::string NEURON_REFACTORY_PERIOD = "neuron.refractory_period";
        const std::string NEURON_ADAPTATION_STRENGTH = "neuron.adaptation_strength";
        const std::string NEURON_SPIKE_HISTORY_SIZE = "neuron.spike_history_size";
        
        // Plasticity parameters
        const std::string PLASTICITY_STDP_LTP_WEIGHT = "plasticity.stdp.ltp_weight";
        const std::string PLASTICITY_STDP_LTD_WEIGHT = "plasticity.stdp.ltd_weight";
        const std::string PLASTICITY_STDP_TIME_CONSTANT = "plasticity.stdp.time_constant";
        
        // Development parameters
        const std::string DEVELOPMENT_BASE_PLASTICITY_RATE = "development.base_plasticity_rate";
        const std::string DEVELOPMENT_CRITICAL_PERIOD_DURATION = "development.critical_period_duration";
        const std::string DEVELOPMENT_MATURATION_RATE = "development.maturation_rate";
        
        // Memory parameters
        const std::string MEMORY_Episodic_CAPACITY = "memory.episodic_capacity";
        const std::string MEMORY_WORKING_DURATION = "memory.working_memory_duration";
        
        // Neuromodulation parameters
        const std::string NEUROMODULATION_DOPAMINE_SCALE = "neuromodulation.dopamine_scale";
        const std::string NEUROMODULATION_NOVELTY_THRESHOLD = "neuromodulation.novelty_threshold";
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
