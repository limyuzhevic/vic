#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    float,
    size_t,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<float>
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
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, size_t value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Config key constants
    static const std::string NEURON_MAX_SPIKE_HISTORY;
    static const std::string NEURON_TIME_CONSTANT;
    static const std::string NEURON_MEMBRANE_CAPACITANCE;
    static const std::string NEURON_REFRACTORY_CHECK_RATE;
    static const std::string NEURON_MAX_MEMBRANE_POTENTIAL;
    static const std::string NEURON_MIN_MEMBRANE_POTENTIAL;
    static const std::string NEURON_ADAPTATION_DECAY;
    static const std::string NEURON_ADAPTATION_FACTOR_DECAY;
    static const std::string NEURON_SPIKE_ADAPTATION_INCREMENT;
    static const std::string NEURON_TEST_CURRENT_AMPLITUDE;
    
    static const std::string SYNAPSE_WEIGHT_MIN;
    static const std::string SYNAPSE_WEIGHT_MAX;
    static const std::string SYNAPSE_STP_FACILITATION_TIME_CONSTANT;
    static const std::string SYNAPSE_STP_DEPRESSION_TIME_CONSTANT;
    static const std::string SYNAPSE_STP_UTILIZATION_MAX;
    static const std::string SYNAPSE_MAX_SPIKE_HISTORY;
    static const std::string SYNAPSE_ELIGIBILITY_DECAY_RATE;
    static const std::string SYNAPSE_ELIGIBILITY_ZERO_THRESHOLD;
    static const std::string SYNAPSE_EFFICIENCY_MAX;
    static const std::string SYNAPSE_EFFICIENCY_MAX_FOR_CLAMP;
    static const std::string SYNAPSE_DECAY_RATE_MAX;
    static const std::string SYNAPSE_DEPRESSION_RECOVERY_RATE;
    static const std::string SYNAPSE_INITIAL_DEPRESSION;
    static const std::string SYNAPSE_FACILITATION_RECOVERY_RATE;
    static const std::string SYNAPSE_INITIAL_FACILITATION;
    static const std::string SYNAPSE_INITIAL_EFFICIENCY;
    static const std::string SYNAPSE_SPIKE_TIME_RESOLUTION;
    static const std::string SYNAPSE_TIME_SINCE_ACTIVITY_MAX;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
