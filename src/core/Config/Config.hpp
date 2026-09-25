// Enhanced Config.hpp with better defaults and validation
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
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

// Enhanced configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::string validationRegex;
    std::string exampleValue;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          required(false), validationRegex(), exampleValue() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false,
                const std::string& regex = "", const std::string& example = "")
        : key(k), value(v), source(s), description(desc), 
          required(req), validationRegex(regex), exampleValue(example) {}
};

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& msg) : std::runtime_error("Configuration validation error: " + msg) {}
};

// Default configuration presets
struct DefaultConfig {
    static std::shared_ptr<Config> createDefaultForLearning() {
        auto config = std::make_shared<Config>();
        
        // Neural parameters
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("max_neurons_per_region", static_cast<int64_t>(300), ConfigSource::Default);
        
        // Learning parameters
        config->set("learning_rate", 0.01f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Development parameters
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("developmental_stage", static_cast<int64_t>(1), ConfigSource::Default);
        
        // Neuromodulation parameters
        config->set("curiosity_strength", 0.3f, ConfigSource::Default);
        config->set("novelty_threshold", 0.5f, ConfigSource::Default);
        config->set("reward_decay", 0.95f, ConfigSource::Default);
        
        // Memory parameters
        config->set("working_memory_capacity", 100, ConfigSource::Default);
        config->set("episodic_memory_limit", 1000, ConfigSource::Default);
        
        // Simulation parameters
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("replay_interval", 100, ConfigSource::Default);
        config->set("consolidation_interval", 1000, ConfigSource::Default);
        
        // Input/output parameters
        config->set("sensory_neuron_ratio", 0.25f, ConfigSource::Default);
        config->set("motor_neuron_ratio", 0.25f, ConfigSource::Default);
        
        // Performance parameters
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("structural_plasticity_enabled", true, ConfigSource::Default);
        
        return config;
    }
    
    static std::shared_ptr<Config> createDefaultForExploration() {
        auto config = std::make_shared<Config>();
        
        // More exploratory configuration
        config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(6), ConfigSource::Default);
        config->set("connection_probability", 0.25f, ConfigSource::Default);  // More connections
        config->set("max_neurons_per_region", static_cast<int64_t>(400), ConfigSource::Default);
        
        // Exploration parameters
        config->set("learning_rate", 0.005f, ConfigSource::Default);  // Slower learning
        config->set("curiosity_strength", 0.5f, ConfigSource::Default);  // Higher curiosity
        config->set("novelty_threshold", 0.3f, ConfigSource::Default);  // More sensitive to novelty
        config->set("epsilon_greedy", 0.2f, ConfigSource::Default);  // More exploration
        
        // Development
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("developmental_stage", static_cast<int64_t>(2), ConfigSource::Default);  // Critical period
        
        // Plasticity
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("structural_plasticity_enabled", true, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);  // More growth
        
        return config;
    }
    
    static std::shared_ptr<Config> createDefaultForStability() {
        auto config = std::make_shared<Config>();
        
        // Stable configuration
        config->set("neuron_count", static_cast<int64_t>(800), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);  // Fewer connections
        config->set("max_neurons_per_region", static_cast<int64_t>(250), ConfigSource::Default);
        
        // Stability parameters
        config->set("learning_rate", 0.001f, ConfigSource::Default);  // Very slow learning
        config->set("curiosity_strength", 0.1f, ConfigSource::Default);  // Low curiosity
        config->set("novelty_threshold", 0.8f, ConfigSource::Default);  // Less sensitive to novelty
        config->set("epsilon_greedy", 0.05f, ConfigSource::Default);  // More exploitation
        
        // Development
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("developmental_stage", static_cast<int64_t>(4), ConfigSource::Default);  // Adult stage
        
        // Plasticity
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("structural_plasticity_enabled", false, ConfigSource::Default);  // No structural changes
        config->set("pruning_rate", 0.00005f, ConfigSource::Default);  // Some pruning
        
        return config;
    }
    
    static std::shared_ptr<Config> createDefaultForBalanced() {
        auto config = std::make_shared<Config>();
        
        // Balanced configuration
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("max_neurons_per_region", static_cast<int64_t>(300), ConfigSource::Default);
        
        // Balanced parameters
        config->set("learning_rate", 0.01f, ConfigSource::Default);
        config->set("curiosity_strength", 0.3f, ConfigSource::Default);
        config->set("novelty_threshold", 0.5f, ConfigSource::Default);
        config->set("epsilon_greedy", 0.1f, ConfigSource::Default);
        
        // Development
        config->set("development_enabled", true, ConfigSource::Default);
        config->set("developmental_stage", static_cast<int64_t>(3), ConfigSource::Default);  // Maturation
        
        // Plasticity
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("structural_plasticity_enabled", true, ConfigSource::Default);
        
        return config;
    }
    
    static std::shared_ptr<Config> createDefaultForResearch() {
        auto config = std::make_shared<Config>();
        
        // Research configuration with many parameters
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(8), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("max_neurons_per_region", static_cast<int64_t>(500), ConfigSource::Default);
        
        // Research parameters
        config->set("learning_rate", 0.005f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        config->set("stdp_tau", 25.0f, ConfigSource::Default);
        config->set("curiosity_strength", 0.4f, ConfigSource::Default);
        config->set("novelty_threshold", 0.4f, ConfigSource::Default);
        config->set("reward_decay", 0.9f, ConfigSource::Default);
        
        // Memory parameters
        config->set("working_memory_capacity", 200, ConfigSource::Default);
        config->set("episodic_memory_limit", 5000, ConfigSource::Default);
        config->set("replay_interval", 50, ConfigSource::Default);  // More frequent replay
        config->set("consolidation_interval", 500, ConfigSource::Default);
        
        // Performance
        config->set("plasticity_enabled", true, ConfigSource::Default);
        config->set("structural_plasticity_enabled", true, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.00015f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        
        // Input/output
        config->set("sensory_neuron_ratio", 0.3f, ConfigSource::Default);
        config->set("motor_neuron_ratio", 0.2f, ConfigSource::Default);
        
        return config;
    }
};

// Enhanced main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters with validation
    void setInt(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setFloat(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Validation methods
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Get default configuration
    static std::shared_ptr<Config> getDefaultConfig(const std::string& preset = "balanced");
    
    // Configuration presets
    static std::shared_ptr<Config> getLearningConfig();
    static std::shared_ptr<Config> getExplorationConfig();
    static std::shared_ptr<Config> getStabilityConfig();
    static std::shared_ptr<Config> getBalancedConfig();
    static std::shared_ptr<Config> getResearchConfig();
    
    // Version info
    static std::string getVersion();
    
    // Help and documentation
    static std::string getHelp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesRegex(const std::string& value, const std::string& regex);
    static std::optional<ConfigValue> parseValue(const std::string& str);
    
    // Validation helpers
    void validateValue(const ConfigEntry& entry) const;
    static std::vector<std::string> getDefaultValidationErrors();
};
