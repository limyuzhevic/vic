#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <string>
#include <filesystem>

// Optional dependencies
#ifdef NLM_WITH_JSON
#include "nlohmann/json.hpp"
#endif

#ifdef NLM_WITH_YAML
#include "yaml-cpp/yaml.h"
#endif

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
    
    // Convert to JSON value
    nlohmann::json toJson() const {
#ifdef NLM_WITH_JSON
        return value;
#else
        return "N/A: JSON library not enabled";
#endif
    }
    
    // Convert from JSON value
    void fromJson(const nlohmann::json& j) {
#ifdef NLM_WITH_JSON
        // Attempt to convert JSON to appropriate ConfigValue type
        if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_number_integer()) {
            value = j.get<int64_t>();
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_array()) {
            // Try to infer array type
            const auto& arr = j;
            if (!arr.empty()) {
                if (arr[0].is_boolean()) {
                    std::vector<bool> boolVec;
                    for (const auto& item : arr) boolVec.push_back(item.get<bool>());
                    value = boolVec;
                } else if (arr[0].is_number_integer()) {
                    std::vector<int64_t> intVec;
                    for (const auto& item : arr) intVec.push_back(item.get<int64_t>());
                    value = intVec;
                } else if (arr[0].is_number_float()) {
                    std::vector<double> doubleVec;
                    for (const auto& item : arr) doubleVec.push_back(item.get<double>());
                    value = doubleVec;
                } else if (arr[0].is_string()) {
                    std::vector<std::string> stringVec;
                    for (const auto& item : arr) stringVec.push_back(item.get<std::string>());
                    value = stringVec;
                } else {
                    value = std::vector<std::string>(); // Default
                }
            } else {
                value = std::vector<std::string>(); // Empty array
            }
        } else {
            value = std::string("N/A: Unsupported JSON type");
        }
#else
        value = "N/A: JSON library not enabled";
#endif
    }
    
    // Convert to YAML node
    YAML::Node toYaml() const {
#ifdef NLM_WITH_YAML
        YAML::Node node;
        std::visit([&node](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int> >) {
                for (const auto& item : arg) node.push_back(item);
            } else if constexpr (std::is_same_v<T, std::vector<double> >) {
                for (const auto& item : arg) node.push_back(item);
            } else if constexpr (std::is_same_v<T, std::vector<bool> >) {
                for (const auto& item : arg) node.push_back(item);
            } else if constexpr (std::is_same_v<T, std::vector<std::string> >) {
                for (const auto& item : arg) node.push_back(item);
            } else {
                node = "N/A: Unsupported type";
            }
        }, value);
        return node;
#else
        YAML::Node node;
        node = "N/A: YAML library not enabled";
        return node;
#endif
    }
    
    // Convert from YAML node
    void fromYaml(const YAML::Node& node) {
#ifdef NLM_WITH_YAML
        if (node.IsScalar()) {
            if (node.IsBoolean()) {
                value = node.as<bool>();
            } else if (node.IsInteger()) {
                value = node.as<int64_t>();
            } else if (node.IsDouble()) {
                value = node.as<double>();
            } else {
                value = node.as<std::string>();
            }
        } else if (node.IsSequence()) {
            // Determine array type by first element
            if (node.size() > 0) {
                if (node[0].IsBoolean()) {
                    std::vector<bool> boolVec;
                    boolVec.reserve(node.size());
                    for (const auto& item : node) boolVec.push_back(item.as<bool>());
                    value = boolVec;
                } else if (node[0].IsInteger()) {
                    std::vector<int64_t> intVec;
                    intVec.reserve(node.size());
                    for (const auto& item : node) intVec.push_back(item.as<int64_t>());
                    value = intVec;
                } else if (node[0].IsDouble()) {
                    std::vector<double> doubleVec;
                    doubleVec.reserve(node.size());
                    for (const auto& item : node) doubleVec.push_back(item.as<double>());
                    value = doubleVec;
                } else {
                    std::vector<std::string> stringVec;
                    stringVec.reserve(node.size());
                    for (const auto& item : node) stringVec.push_back(item.as<std::string>());
                    value = stringVec;
                }
            } else {
                value = std::vector<std::string>(); // Empty array
            }
        } else {
            value = "N/A: Unsupported YAML node type";
        }
#else
        value = "N/A: YAML library not enabled";
#endif
    }
};

// Structured configuration classes for different systems
struct BrainConfig {
    int64_t neuron_count = 1000;
    int64_t region_count = 2;
    double connection_probability = 0.1;
    std::string structure_type = "random";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["neuron_count"] = neuron_count;
        j["region_count"] = region_count;
        j["connection_probability"] = connection_probability;
        j["structure_type"] = structure_type;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("neuron_count")) neuron_count = j["neuron_count"];
        if (j.contains("region_count")) region_count = j["region_count"];
        if (j.contains("connection_probability")) connection_probability = j["connection_probability"];
        if (j.contains("structure_type")) structure_type = j["structure_type"];
    }
};

struct PlasticityConfig {
    double learning_rate = 0.01;
    double stdp_ltp_weight = 0.01;
    double stdp_ltd_weight = 0.012;
    double stdp_time_constant = 20.0;
    std::string rule_type = "stdp";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["learning_rate"] = learning_rate;
        j["stdp_ltp_weight"] = stdp_ltp_weight;
        j["stdp_ltd_weight"] = stdp_ltd_weight;
        j["stdp_time_constant"] = stdp_time_constant;
        j["rule_type"] = rule_type;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("learning_rate")) learning_rate = j["learning_rate"];
        if (j.contains("stdp_ltp_weight")) stdp_ltp_weight = j["stdp_ltp_weight"];
        if (j.contains("stdp_ltd_weight")) stdp_ltd_weight = j["stdp_ltd_weight"];
        if (j.contains("stdp_time_constant")) stdp_time_constant = j["stdp_time_constant"];
        if (j.contains("rule_type")) rule_type = j["rule_type"];
    }
};

struct NeuromodulationConfig {
    double dopamine_baseline = 0.0;
    double dopamine_decay_rate = 0.95;
    double reward_scale = 1.0;
    double prediction_error_scale = 1.0;
    std::string novelty_threshold_type = "adaptive";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["dopamine_baseline"] = dopamine_baseline;
        j["dopamine_decay_rate"] = dopamine_decay_rate;
        j["reward_scale"] = reward_scale;
        j["prediction_error_scale"] = prediction_error_scale;
        j["novelty_threshold_type"] = novelty_threshold_type;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("dopamine_baseline")) dopamine_baseline = j["dopamine_baseline"];
        if (j.contains("dopamine_decay_rate")) dopamine_decay_rate = j["dopamine_decay_rate"];
        if (j.contains("reward_scale")) reward_scale = j["reward_scale"];
        if (j.contains("prediction_error_scale")) prediction_error_scale = j["prediction_error_scale"];
        if (j.contains("novelty_threshold_type")) novelty_threshold_type = j["novelty_threshold_type"];
    }
};

struct MemoryConfig {
    int64_t working_memory_size = 100;
    int64_t episodic_memory_capacity = 1000;
    int64_t semantic_memory_capacity = 5000;
    double consolidation_rate = 0.01;
    std::string consolidation_rule = "lstm";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["working_memory_size"] = working_memory_size;
        j["episodic_memory_capacity"] = episodic_memory_capacity;
        j["semantic_memory_capacity"] = semantic_memory_capacity;
        j["consolidation_rate"] = consolidation_rate;
        j["consolidation_rule"] = consolidation_rule;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("working_memory_size")) working_memory_size = j["working_memory_size"];
        if (j.contains("episodic_memory_capacity")) episodic_memory_capacity = j["episodic_memory_capacity"];
        if (j.contains("semantic_memory_capacity")) semantic_memory_capacity = j["semantic_memory_capacity"];
        if (j.contains("consolidation_rate")) consolidation_rate = j["consolidation_rate"];
        if (j.contains("consolidation_rule")) consolidation_rule = j["consolidation_rule"];
    }
};

struct VisualizationConfig {
    bool enabled = false;
    double update_rate = 30.0;
    std::string backend = "none";
    std::string output_format = "text";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["enabled"] = enabled;
        j["update_rate"] = update_rate;
        j["backend"] = backend;
        j["output_format"] = output_format;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("enabled")) enabled = j["enabled"];
        if (j.contains("update_rate")) update_rate = j["update_rate"];
        if (j.contains("backend")) backend = j["backend"];
        if (j.contains("output_format")) output_format = j["output_format"];
    }
};

struct EnvironmentConfig {
    std::string name = "GridWorld";
    int64_t width = 10;
    int64_t height = 10;
    std::string type = "deterministic";
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["name"] = name;
        j["width"] = width;
        j["height"] = height;
        j["type"] = type;
        return j;
    }
    
    void fromJson(const nlohmann::json& j) {
        if (j.contains("name")) name = j["name"];
        if (j.contains("width")) width = j["width"];
        if (j.contains("height")) height = j["height"];
        if (j.contains("type")) type = j["type"];
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
    
    // Load from file (JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON or YAML format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJson() const;
    
    // Save to YAML string
    std::string saveToYaml() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Configuration validation
    bool validate() const;
    
    // Internal validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    
    // Apply structured configuration templates
    void applyBrainConfig(const BrainConfig& brainCfg);
    void applyPlasticityConfig(const PlasticityConfig& plasticityCfg);
    void applyNeuromodulationConfig(const NeuromodulationConfig& neuromodCfg);
    void applyMemoryConfig(const MemoryConfig& memoryCfg);
    void applyVisualizationConfig(const VisualizationConfig& visCfg);
    void applyEnvironmentConfig(const EnvironmentConfig& envCfg);
    
    // Get structured configurations
    BrainConfig getBrainConfig() const;
    PlasticityConfig getPlasticityConfig() const;
    NeuromodulationConfig getNeuromodulationConfig() const;
    MemoryConfig getMemoryConfig() const;
    VisualizationConfig getVisualizationConfig() const;
    EnvironmentConfig getEnvironmentConfig() const;
    
    // Configuration error types
    enum class ConfigError {
        None,
        FileNotFound,
        ParseError,
        InvalidFormat,
        ValidationError,
        UnsupportedFileFormat
    };
    
    // Get last error
    ConfigError getLastError() const { return lastError; }
    
    // Get error message
    std::string getErrorMessage() const;
    
    // Configuration format detection
    static std::string detectFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable ConfigError lastError = ConfigError::None;
    mutable std::string lastErrorMsg;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // Parsing helpers
#ifdef NLM_WITH_JSON
    nlohmann::json parseJsonFile(const std::string& filepath) const;
#endif
#ifdef NLM_WITH_YAML
    YAML::Node parseYamlFile(const std::string& filepath) const;
#endif
    
    // Validation helpers
    bool validateNeuronCount(int64_t count) const;
    bool validateConnectionProbability(double prob) const;
    bool validateTimestep(double timestep) const;
};

} // namespace nlm
