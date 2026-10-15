#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <functional>
#include <map>
#include <unordered_map>
#include <limits>

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", const ConfigValue& value = {}, ConfigSource source = ConfigSource::Runtime)
        : std::runtime_error(message), key(key), value(value), source(source) {}
    
    std::string key;
    ConfigValue value;
    ConfigSource source;
};

// Parameter bounds
struct ParameterBounds {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    bool allowNegative = false;
    bool allowZero = true;
    std::string unit = "";
    
    ParameterBounds() = default;
    ParameterBounds(double min, double max, bool allowNeg = false, bool allowZ = true, const std::string& unit = "")
        : min(min), max(max), allowNegative(allowNeg), allowZero(allowZ), unit(unit) {}
    
    bool isValid(double value) const {
        if (value < min || value > max) return false;
        if (!allowNegative && value < 0) return false;
        if (!allowZero && value == 0) return false;
        return true;
    }
};

// Common parameter sets
namespace ParamSets {
    // Brain configuration parameters
    const std::unordered_map<std::string, ParameterBounds> brain = {
        {"neuron_count", ParameterBounds(1, 1000000, true, false, "neurons")},
        {"region_count", ParameterBounds(1, 1000, true, false, "regions")},
        {"connection_probability", ParameterBounds(0.0, 1.0, false, false, "probability")},
        {"random_seed", ParameterBounds(0, std::numeric_limits<uint64_t>::max(), false, false, "seed")},
        {"simulation_timestep", ParameterBounds(0.0001, 1.0, false, false, "s")}
    };
    
    // Plasticity configuration parameters
    const std::unordered_map<std::string, ParameterBounds> plasticity = {
        {"stdp_ltp_weight", ParameterBounds(0.0, 0.1, false, false, "weight")},
        {"stdp_ltd_weight", ParameterBounds(0.0, 0.1, false, false, "weight")},
        {"stdp_tau", ParameterBounds(1.0, 100.0, false, false, "ms")},
        {"synaptogenesis_rate", ParameterBounds(0.0, 0.1, false, false, "rate")},
        {"pruning_rate", ParameterBounds(0.0, 0.01, false, false, "rate")},
        {"learning_rate", ParameterBounds(0.0, 1.0, false, false, "rate")},
        {"max_weight", ParameterBounds(0.0, 1.0, false, false, "weight")}
    };
    
    // Neuromodulation configuration parameters
    const std::unordered_map<std::string, ParameterBounds> neuromodulation = {
        {"dopamine_level", ParameterBounds(0.0, 1.0, false, false, "level")},
        {"curiosity_level", ParameterBounds(0.0, 1.0, false, false, "level")},
        {"novelty_threshold", ParameterBounds(0.0, 1.0, false, false, "threshold")},
        {"novice_threshold", ParameterBounds(0.0, 1.0, false, false, "threshold")},
        {"decay_rate", ParameterBounds(0.0, 1.0, false, false, "rate")}
    };
    
    // Development configuration parameters
    const std::unordered_map<std::string, ParameterBounds> development = {
        {"maturation_rate", ParameterBounds(0.0, 1.0, false, false, "rate")},
        {"pruning_threshold", ParameterBounds(0.0, 1.0, false, false, "threshold")},
        {"refinement_rate", ParameterBounds(0.0, 1.0, false, false, "rate")}
    };
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

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::string description;
    std::unordered_map<std::string, ParameterBounds> parameterBounds;
    std::unordered_map<std::string, std::string> defaultValues;
    std::function<bool(const ConfigEntry&)> validator;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& n, const std::string& desc = "")
        : name(n), description(desc) {}
    
    bool validate(const ConfigEntry& entry) const {
        if (validator) {
            return validator(entry);
        }
        
        auto boundsIt = parameterBounds.find(entry.key);
        if (boundsIt == parameterBounds.end()) {
            return true;  // No bounds defined for this key
        }
        
        const auto& bounds = boundsIt->second;
        
        try {
            if (std::holds_alternative<int>(entry.value)) {
                double val = static_cast<double>(std::get<int>(entry.value));
                return bounds.isValid(val);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                double val = static_cast<double>(std::get<int64_t>(entry.value));
                return bounds.isValid(val);
            } else if (std::holds_alternative<double>(entry.value)) {
                double val = std::get<double>(entry.value);
                return bounds.isValid(val);
            } else if (std::holds_alternative<float>(entry.value)) {
                double val = static_cast<double>(std::get<float>(entry.value));
                return bounds.isValid(val);
            } else if (std::holds_alternative<bool>(entry.value)) {
                return true;  // Boolean values always valid
            } else if (std::holds_alternative<std::string>(entry.value)) {
                return true;  // String values always valid
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                const auto& vec = std::get<std::vector<int>>(entry.value);
                return std::all_of(vec.begin(), vec.end(), 
                    [&bounds](int v) { return bounds.isValid(static_cast<double>(v)); });
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                const auto& vec = std::get<std::vector<double>>(entry.value);
                return std::all_of(vec.begin(), vec.end(),
                    [&bounds](double v) { return bounds.isValid(v); });
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                return true;  // String vector values always valid
            }
        } catch (...) {
            return false;
        }
        
        return false;
    }
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
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Add schema for parameter validation
    void addSchema(const std::string& category, const ConfigSchema& schema);
    
    // Validate single parameter
    bool validateParameter(const std::string& key) const;
    
    // Validate all parameters
    bool validateAll() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
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
    
    // === Enhanced API ===
    
    // Batch operations
    void setBatch(const std::unordered_map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Copy and merge
    void merge(const Config& other);
    Config copy() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::vector<std::string> findDifferences(const Config& other) const;
    
    // Preset configurations
    static Config createMinimalBrainConfig();
    static Config createStandardConfig();
    static Config createHighPlasticityConfig();
    static Config createDevelopmentConfig();
    static Config createNeuromodulationConfig();
    
    // Export/import utilities
    std::string exportToString() const;
    bool importFromString(const std::string& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJsonString(const ConfigValue& value);
    static ConfigValue fromJsonString(const std::string& str);
    static std::string serializeToJson() const;
    static bool deserializeFromJson(const std::string& json);
    
    // Schema validation
    bool validateWithSchema(const std::string& key, const ConfigValue& value, ConfigSource source) const;
};

} // namespace nlm

// Specialized template for float
namespace nlm {
    template<typename T>
    std::optional<T> Config::get(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            if constexpr (std::is_same_v<T, float>) {
                if (std::holds_alternative<double>(it->value)) {
                    return static_cast<float>(std::get<double>(it->value));
                }
            }
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
}
