#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Configuration validation error
struct ConfigValidationError {
    std::string field;
    std::string message;
    
    ConfigValidationError(const std::string& f, const std::string& m) 
        : field(f), message(m) {}
};

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::string description;
    std::vector<ConfigEntry> defaults;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    ConfigSchema() : name("default"), description("Default configuration schema") {}
    ConfigSchema(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
    
    // Add default value
    void addDefault(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default, const std::string& desc = "") {
        defaults.emplace_back(key, value, source, desc);
    }
    
    // Add validator for specific key
    void addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
        validators[key] = validator;
    }
    
    // Validate configuration against schema
    std::vector<ConfigValidationError> validate(const Config& config) const {
        std::vector<ConfigValidationError> errors;
        
        // Check all configured values
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<ConfigValue>(entry);
            if (value && validators.find(entry) != validators.end()) {
                if (!validators.at(entry)(*value)) {
                    errors.emplace_back(entry, "Value validation failed");
                }
            }
        }
        
        return errors;
    }
};

// Predefined schema for common configuration values
class ConfigSchemas {
public:
    static ConfigSchema createDefaultNLM() {
        ConfigSchema schema("nlm_default", "Default NLM configuration schema");
        
        // Basic settings
        schema.addDefault("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducible experiments");
        schema.addDefault("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
        schema.addDefault("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, "Number of neurons in the brain");
        schema.addDefault("region_count", static_cast<int64_t>(2), ConfigSource::Default, "Number of brain regions");
        
        // Connection settings
        schema.addDefault("connection_probability", 0.1f, ConfigSource::Default, "Connection probability for random connectivity");
        
        // Plasticity settings
        schema.addDefault("plasticity_learning_rate", 0.01, ConfigSource::Default, "General plasticity learning rate");
        schema.addDefault("stdp_ltp_weight", 0.01f, ConfigSource::Default, "STDP long-term potentiation weight");
        schema.addDefault("stdp_ltd_weight", 0.012f, ConfigSource::Default, "STDP long-term depression weight");
        schema.addDefault("stdp_time_constant", 20.0f, ConfigSource::Default, "STDP time constant in milliseconds");
        
        // Development settings
        schema.addDefault("development_synaptogenesis_rate", 0.001, ConfigSource::Default, "Synaptogenesis rate during development");
        schema.addDefault("development_pruning_rate", 0.0001, ConfigSource::Default, "Pruning rate during development");
        
        // Neuromodulation settings
        schema.addDefault("dopamine_baseline", 0.0, ConfigSource::Default, "Baseline dopamine level");
        schema.addDefault("curiosity_baseline", 0.1, ConfigSource::Default, "Baseline curiosity level");
        
        // Reward settings
        schema.addDefault("reward_discount_factor", 0.99, ConfigSource::Default, "Reward discount factor for temporal difference learning");
        
        // Environment settings
        schema.addDefault("environment_name", std::string("GridWorld"), ConfigSource::Default, "Name of the environment");
        schema.addDefault("environment_width", static_cast<int64_t>(10), ConfigSource::Default, "Environment width in cells");
        schema.addDefault("environment_height", static_cast<int64_t>(10), ConfigSource::Default, "Environment height in cells");
        
        // Logging settings
        schema.addDefault("log_level", std::string("INFO"), ConfigSource::Default, "Minimum log level to output");
        schema.addDefault("log_to_file", false, ConfigSource::Default, "Whether to log to file in addition to console");
        schema.addDefault("log_filename", std::string("nlm.log"), ConfigSource::Default, "Log file name");
        
        // Simulation settings
        schema.addDefault("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default, "Maximum number of simulation steps");
        schema.addDefault("simulation_time_limit", 0.0, ConfigSource::Default, "Simulation time limit in seconds (0 = unlimited)");
        
        // Visualization settings
        schema.addDefault("visualization_enabled", false, ConfigSource::Default, "Whether to enable visualization");
        schema.addDefault("visualization_update_rate", 30.0, ConfigSource::Default, "Visualization update rate in Hz");
        
        // Add validators
        schema.addValidator("random_seed", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 0;
        });
        
        schema.addValidator("simulation_timestep", [](const ConfigValue& v) {
            return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
        });
        
        schema.addValidator("neuron_count", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        });
        
        schema.addValidator("region_count", [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
        });
        
        schema.addValidator("connection_probability", [](const ConfigValue& v) {
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f;
        });
        
        return schema;
    }
    
    static ConfigSchema createMinimalNLM() {
        ConfigSchema schema("nlm_minimal", "Minimal NLM configuration schema");
        
        // Only essential settings
        schema.addDefault("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducible experiments");
        schema.addDefault("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
        schema.addDefault("neuron_count", static_cast<int64_t>(100), ConfigSource::Default, "Number of neurons in the brain");
        
        return schema;
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
    
    // Load from file (JSON or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate() const;
    std::vector<ConfigValidationError> validate(const ConfigSchema& schema) const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema) { currentSchema = schema; }
    const ConfigSchema& getSchema() const { return currentSchema; }
    bool hasSchema() const { return hasSchemaFlag; }
    
    // Load from schema defaults
    void applySchemaDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON parsing helpers
    bool loadFromKeyValueFile(std::ifstream& file);
    bool parseJson(const nlohmann::json& json);
    void processJsonObject(const nlohmann::json& json, const std::string& prefix = "");
    void processJsonValue(const nlohmann::json& value, const std::string& key);
    void processJsonArray(const nlohmann::json& array, const std::string& key);
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    
    // Schema support
    ConfigSchema currentSchema;
    bool hasSchemaFlag = false;
};

} // namespace nlm

} // namespace nlm
