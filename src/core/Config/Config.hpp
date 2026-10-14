#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <sstream>

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

// Configuration categories for better organization
enum class ConfigCategory {
    Core,
    Neural,
    Memory,
    Plasticity,
    Development,
    Neuromodulation,
    Prediction,
    Cognition,
    Sensory,
    Motor,
    Environment,
    Logging,
    Simulation,
    Visualization,
    Custom
};

// Configuration validation
struct ConfigValidation {
    std::string key;
    std::string validator;  // Type of validation (e.g., "positive_int", "range[0-1]")
    std::string errorMessage;
};

// Advanced configuration options for different phases
struct AdvancedConfig {
    // Plasticity settings
    float stdp_ltp_weight = 0.02f;
    float stdp_ltd_weight = 0.015f;
    float stdp_time_constant = 20.0f;
    float hebbian_learning_rate = 0.01f;
    float reward_modulation_strength = 1.0f;

    // Development settings
    float synaptogenesis_rate = 0.001f;
    float pruning_rate = 0.0001f;
    float critical_period_multiplier = 1.5f;
    float maturation_rate = 0.1f;

    // Neuromodulation settings
    float dopamine_baseline = 0.1f;
    float curiosity_strength = 0.5f;
    float novelty_threshold = 0.7f;
    float prediction_error_decay = 0.95f;

    // Memory settings
    size_t working_memory_capacity = 1000;
    size_t episodic_memory_capacity = 5000;
    size_t associative_memory_patterns = 100;

    // Prediction settings
    float prediction_time_constant = 0.1f;
    float confidence_threshold = 0.8f;

    // Performance settings
    bool simd_enabled = true;
    bool parallel_enabled = true;
    size_t vectorization_width = 4;

    // Visualization settings
    bool visualization_enabled = false;
    float visualization_update_rate = 30.0f;

    // Logging settings
    std::string log_level = "INFO";
    bool log_to_file = false;
    std::string log_filename = "nlm.log";

    // Environment settings
    std::string environment_name = "GridWorld";
    size_t environment_width = 10;
    size_t environment_height = 10;

    // Simulation settings
    size_t max_simulation_steps = 10000;
    double simulation_time_limit = 0.0;
    double timestep = 0.001;

    // Agent settings
    float max_energy = 100.0f;
    float energy_decay_rate = 0.01f;
    float health_degradation_rate = 0.001f;

    // Development tracking
    bool track_development_stages = true;
    bool enable_structural_plasticity = true;
    bool enable_curiosity = true;
    bool enable_reward_modulation = true;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries = 0;
    size_t entriesBySource[4] = {0};  // Default, File, CommandLine, Runtime
    std::string mostUsedKey;
    double averageValueSize = 0.0;
    std::vector<std::string> validationErrors;
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

    // Advanced configuration methods
    const ConfigEntry* getEntry(const std::string& key) const;
    void setWithCategory(const std::string& key, const ConfigValue& value, 
                        ConfigSource source, ConfigCategory category, 
                        const std::string& description = "");
    bool setValidated(const std::string& key, const ConfigValue& value, 
                     ConfigSource source, const std::string& validator = "");
    std::vector<ConfigEntry> getByCategory(ConfigCategory category) const;
    std::vector<std::string> validateAll() const;
    void applyDefaults(int phase);
    std::string getFileContent(const std::string& filepath) const;
    static bool isFileFormatSupported(const std::string& filepath);
    static std::vector<std::string> getSupportedFileExtensions();
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    std::shared_ptr<Config> clone() const;
    static std::map<int, std::vector<std::string>> getPhaseConfigKeys();
    static std::string getPhaseDocumentation(int phase);

    // Advanced configuration
    void applyAdvancedConfig(const AdvancedConfig& advConfig);
    AdvancedConfig getAdvancedConfig() const;
    ConfigStats getStats() const;
    std::vector<ConfigEntry> search(const std::string& pattern) const;
    std::vector<ConfigEntry> getBySource(ConfigSource source) const;
    bool hasAny(const std::vector<std::string>& keys) const;
    bool equals(const Config& other) const;
    double similarity(const Config& other) const;
    std::vector<std::string> suggestOptimizations() const;
    std::string generateOptimalConfig(int phase) const;
    template<typename T>
    T getSafe(const std::string& key, const T& defaultValue = T(), 
              const std::string& validator = "") const;
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> 
        diff(const Config& other) const;
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidationRule(const std::string& key, const std::string& validator);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // File format parsers (private implementations)
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    bool loadFromKeyValueFile(const std::string& filepath);

    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateKey(const std::string& key);
    static ConfigValue parseValue(const std::string& valueStr);
    static std::string valueToString(const ConfigValue& value);
    static std::string valueToString(const std::vector<int>& values);
    static std::string valueToString(const std::vector<double>& values);
    static std::string valueToString(const std::vector<std::string>& values);
};

} // namespace nlm
