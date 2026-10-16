#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <type_traits>

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

// Parameter validation function signature
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Parameter metadata for validation and defaults
struct ParameterMeta {
    std::string name;
    std::string description;
    ConfigValue defaultValue;
    ValidationFunction validator;
    bool required;
    
    ParameterMeta() : name(), description(), validator(nullptr), required(false) {}
    ParameterMeta(const std::string& n, const std::string& desc, const ConfigValue& def,
                  const ValidationFunction& val, bool req = false)
        : name(n), description(desc), defaultValue(def), validator(val), required(req) {}
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
    
    // Parameter registration and validation
    void registerParameter(const std::string& section, const ParameterMeta& param);
    void setParameter(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    template<typename T>
    std::optional<T> getParameter(const std::string& section, const std::string& key) const;
    template<typename T>
    T getParameterOr(const std::string& section, const std::string& key, const T& defaultValue) const;
    bool validateParameter(const std::string& section, const std::string& key, const ConfigValue& value) const;
    void ensureRequiredParameters(const std::string& section) const;
    
    // Neuron configuration parameters
    static ParameterMeta NEURON_MEMBRANE_CAPACITANCE;
    static ParameterMeta NEURON_TIME_CONSTANT;
    static ParameterMeta NEURON_MAX_SPIKE_HISTORY;
    static ParameterMeta NEURON_RESTING_POTENTIAL_DEFAULT;
    static ParameterMeta NEURON_THRESHOLD_DEFAULT;
    static ParameterMeta NEURON_RESET_POTENTIAL_DEFAULT;
    static ParameterMeta NEURON_LEAK_CONDUCTANCE_DEFAULT;
    static ParameterMeta NEURON_ADAPTATION_RATE;
    static ParameterMeta NEURON_REFRACTORY_PERIOD_DEFAULT;
    
    // Synapse configuration parameters
    static ParameterMeta SYNAPSE_WEIGHT_MIN;
    static ParameterMeta SYNAPSE_WEIGHT_MAX;
    static ParameterMeta SYNAPSE_STP_FACILITATION_TAU;
    static ParameterMeta SYNAPSE_STP_DEPRESSION_TAU;
    static ParameterMeta SYNAPSE_STP_U_MAX;
    static ParameterMeta SYNAPSE_MAX_SPIKE_HISTORY;
    static ParameterMeta SYNAPSE_EFFICIENCY_RANGE_MIN;
    static ParameterMeta SYNAPSE_EFFICIENCY_RANGE_MAX;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isInRange(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max);
};

// Range validation helper template
namespace config_validation {
    template<typename T>
    ValidationFunction createRangeValidator(T min, T max) {
        return [min, max](const ConfigValue& value) -> bool {
            try {
                T val = std::get<T>(value);
                return val >= min && val <= max;
            } catch (const std::bad_variant_access&) {
                return false;
            }
        };
    }
    
    ValidationFunction createFloatRangeValidator(float min, float max);
    ValidationFunction createIntRangeValidator(int min, int max);
}

} // namespace nlm
