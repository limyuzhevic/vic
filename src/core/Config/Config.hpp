#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigPreset;

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

// Configuration parameter validation info
struct ConfigParameterInfo {
    std::string name;
    std::string type;  // "int", "double", "string", "bool"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required;
    std::string validation;  // For additional validation rules
};

// Configuration preset base class
class ConfigPreset {
public:
    virtual ~ConfigPreset() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual const std::unordered_map<std::string, ConfigValue>& getDefaults() const = 0;
    virtual bool isExperimental() const { return false; }
};

// Experimental preset
class ExperimentalPreset : public ConfigPreset {
public:
    std::string getName() const override { return "experimental"; }
    std::string getDescription() const override { return "High plasticity and exploration for rapid learning"; }
    bool isExperimental() const override { return true; }
    const std::unordered_map<std::string, ConfigValue>& getDefaults() const override;
};

// Performance preset
class PerformancePreset : public ConfigPreset {
public:
    std::string getName() const override { return "performance"; }
    std::string getDescription() const override { return "Optimized for speed with reduced exploration"; }
    bool isExperimental() const override { return false; }
    const std::unordered_map<std::string, ConfigValue>& getDefaults() const override;
};

// Research preset
class ResearchPreset : public ConfigPreset {
public:
    std::string getName() const override { return "research"; }
    std::string getDescription() const override { return "Balanced configuration for controlled experiments"; }
    bool isExperimental() const override { return false; }
    const std::unordered_map<std::string, ConfigValue>& getDefaults() const override;
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with JSON support
    bool loadFromFile(const std::string& filepath);
    
    // JSON configuration file support
    static bool loadFromJSON(const std::string& filepath, std::shared_ptr<Config>& config);
    bool saveToJSON(const std::string& filepath) const;
    
    // Load from command line with validation
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgs(int argc, char** argv, const std::unordered_map<std::string, ConfigParameterInfo>& validationRules);
    
    // Save to file (legacy and JSON support)
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
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    bool applyPreset(const std::string& presetName, ConfigSource source = ConfigSource::Runtime);
    void registerPreset(std::unique_ptr<ConfigPreset> preset);
    
    // Inheritance and merging
    bool mergeFrom(const Config& otherConfig, ConfigSource source = ConfigSource::File);
    bool inheritFrom(const Config& parentConfig, const std::vector<std::string>& overrideKeys = {});
    
    // Parameter validation
    bool validateParameter(const std::string& key, const ConfigValue& value, std::string& error) const;
    bool validateAllParameters(std::vector<std::string>& errors) const;
    
    // Parameter info management
    void registerParameter(const ConfigParameterInfo& info);
    std::optional<ConfigParameterInfo> getParameterInfo(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Parameter validation helpers
    static bool validateInt(const ConfigValue& value, const std::string& validation);
    static bool validateDouble(const ConfigValue& value, const std::string& validation);
    static bool validateString(const ConfigValue& value, const std::string& validation);
    static bool validateBool(const ConfigValue& value, const std::string& validation);
    
    // Preset helpers
    static void initializePresets(std::vector<std::unique_ptr<ConfigPreset>>& presets);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& json, const std::string& type);
    
    // File format detection and parsing
    static bool detectFileFormat(const std::string& filepath);
    static bool parseJSONFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& entries);
    static bool parseLegacyFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& entries);
    
    // Legacy file parsing helpers
    static std::vector<std::string> readFileLines(const std::string& filepath);
    static bool parseLegacyLine(const std::string& line, std::unordered_map<std::string, ConfigValue>& entries);
    
    // JSON serialization helpers
    static void addJSONEntry(nlohmann::json& json, const ConfigEntry& entry);
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Experimental preset implementation
const std::unordered_map<std::string, ConfigValue>& ExperimentalPreset::getDefaults() const {
    static const std::unordered_map<std::string, ConfigValue> defaults = {
        {"random_seed", 12345ULL},
        {"neuron_count", 2000},
        {"region_count", 4},
        {"connection_probability", 0.15},
        {"plasticity_learning_rate", 0.05},
        {"stdp_ltp_weight", 0.02},
        {"stdp_ltd_weight", 0.025},
        {"stdp_time_constant", 25.0},
        {"development_synaptogenesis_rate", 0.002},
        {"development_pruning_rate", 0.0002},
        {"dopamine_baseline", 0.2},
        {"reward_discount_factor", 0.95},
        {"environment_name", "GridWorld"},
        {"environment_width", 15},
        {"environment_height", 15},
        {"log_level", "INFO"},
        {"log_to_file", true},
        {"log_filename", "nlm_experimental.log"},
        {"max_simulation_steps", 20000},
        {"simulation_time_limit", 0.0},
        {"visualization_enabled", true},
        {"visualization_update_rate", 60.0}
    };
    return defaults;
}

// Performance preset implementation
const std::unordered_map<std::string, ConfigValue>& PerformancePreset::getDefaults() const {
    static const std::unordered_map<std::string, ConfigValue> defaults = {
        {"random_seed", 54321ULL},
        {"neuron_count", 1500},
        {"region_count", 2},
        {"connection_probability", 0.08},
        {"plasticity_learning_rate", 0.02},
        {"stdp_ltp_weight", 0.015},
        {"stdp_ltd_weight", 0.018},
        {"stdp_time_constant", 20.0},
        {"development_synaptogenesis_rate", 0.001},
        {"development_pruning_rate", 0.0001},
        {"dopamine_baseline", 0.1},
        {"reward_discount_factor", 0.99},
        {"environment_name", "GridWorld"},
        {"environment_width", 8},
        {"environment_height", 8},
        {"log_level", "WARNING"},
        {"log_to_file", true},
        {"log_filename", "nlm_performance.log"},
        {"max_simulation_steps", 10000},
        {"simulation_time_limit", 0.0},
        {"visualization_enabled", false},
        {"visualization_update_rate", 30.0}
    };
    return defaults;
}

// Research preset implementation
const std::unordered_map<std::string, ConfigValue>& ResearchPreset::getDefaults() const {
    static const std::unordered_map<std::string, ConfigValue> defaults = {
        {"random_seed", 99999ULL},
        {"neuron_count", 1000},
        {"region_count", 1},
        {"connection_probability", 0.1},
        {"plasticity_learning_rate", 0.01},
        {"stdp_ltp_weight", 0.01},
        {"stdp_ltd_weight", 0.012},
        {"stdp_time_constant", 20.0},
        {"development_synaptogenesis_rate", 0.001},
        {"development_pruning_rate", 0.0001},
        {"dopamine_baseline", 0.05},
        {"reward_discount_factor", 0.98},
        {"environment_name", "GridWorld"},
        {"environment_width", 10},
        {"environment_height", 10},
        {"log_level", "INFO"},
        {"log_to_file", true},
        {"log_filename", "nlm_research.log"},
        {"max_simulation_steps", 10000},
        {"simulation_time_limit", 0.0},
        {"visualization_enabled", false},
        {"visualization_update_rate", 30.0}
    };
    return defaults;
}

} // namespace nlm
