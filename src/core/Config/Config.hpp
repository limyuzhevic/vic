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
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Enhanced configuration parameters structure
struct AdvancedConfig {
    // Neural network topology parameters
    struct Topology {
        int64_t max_regions;
        int64_t neurons_per_region;
        std::vector<int64_t> region_sizes;
        double connection_probability;
        bool enable_heterogeneous_connectivity;
        double long_range_connection_weight;
        int64_t average_degree;
        double clustering_coefficient_target;
    } topology;
    
    // Neuromodulator control options
    struct Neuromodulation {
        bool enable_dopamine;
        bool enable_curiosity;
        bool enable_novelty;
        bool enable_prediction_error;
        double dopamine_baseline;
        double dopamine_learning_rate;
        double curiosity_strength;
        double novelty_threshold;
        double prediction_error_decay;
        std::vector<double> neuromodulator_thresholds;
    } neuromodulation;
    
    // Plasticity tuning parameters
    struct Plasticity {
        bool enable_stdp;
        bool enable_hebbian;
        bool enable_structural_plasticity;
        double stdp_ltp_weight;
        double stdp_ltd_weight;
        double stdp_time_constant;
        double stdp_trace_decay;
        double hebbian_strength;
        double synaptogenesis_rate;
        double pruning_rate;
        double structural_plasticity_threshold;
        bool enable_adaptive_plasticity;
        double homeostatic_setpoint;
        double homeostatic_learning_rate;
    } plasticity;
    
    // Development stage controls
    struct Development {
        bool enable_development;
        double growth_rate;
        double maturation_rate;
        double pruning_intensity;
        double synaptogenesis_intensity;
        double myelination_rate;
        double connectivity_maturation;
        bool enable_experience_dependent_development;
        double critical_period_start;
        double critical_period_end;
        std::vector<double> stage_transition_thresholds;
    } development;
    
    // Advanced brain features
    struct AdvancedFeatures {
        bool enable_memory_consolidation;
        bool enable_social_learning;
        bool enable_reinforcement_learning;
        bool enable_supervised_learning;
        bool enable_meta_learning;
        double social_influence_strength;
        double memory_consolidation_rate;
        double rl_exploration_rate;
        double rl_discount_factor;
        double sl_learning_rate;
        bool enable_neural_duplicate_detection;
        double duplicate_threshold;
        double interference_reduction;
    } advanced;
};

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
    
    // Validation helpers
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::string& value);
};

} // namespace nlm
