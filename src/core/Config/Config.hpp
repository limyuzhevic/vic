#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

namespace nlm {

// Centralized configuration parameters and defaults
namespace ConfigParams {
    // Neural system parameters
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    constexpr const char* RANDOM_SEED = "random_seed";
    
    // Neuron parameters
    constexpr const char* NEURON_THRESHOLD = "neuron_threshold";
    constexpr const char* NEURON_RESTING_POTENTIAL = "neuron_resting_potential";
    constexpr const char* NEURON_RESET_POTENTIAL = "neuron_reset_potential";
    constexpr const char* NEURON_LEAK_CONDUCTANCE = "neuron_leak_conductance";
    constexpr const char* NEURON_REFECTORY_PERIOD = "neuron_refractory_period";
    constexpr const char* NEURON_ADAPTATION_RATE = "neuron_adaptation_rate";
    
    // Synapse parameters
    constexpr const char* SYNAPSE_WEIGHT = "synapse_weight";
    constexpr const char* SYNAPSE_DELAY = "synapse_delay";
    constexpr const char* SYNAPSE_TYPE = "synapse_type";
    constexpr const char* SYNAPSE_MIN_WEIGHT = "synapse_min_weight";
    constexpr const char* SYNAPSE_MAX_WEIGHT = "synapse_max_weight";
    
    // Plasticity parameters
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TAU = "stdp_tau";
    constexpr const char* HEBBIAN_STRENGTH = "hebbian_strength";
    constexpr const char* STRUCTURAL_PLASTICITY_RATE = "structural_plasticity_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    
    // Memory system parameters
    constexpr const char* WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    constexpr const char* EPISODIC_MEMORY_MAX_EPISODES = "episodic_memory_max_episodes";
    constexpr const char* ASSOCIATION_THRESHOLD = "association_threshold";
    
    // Neuromodulation parameters
    constexpr const char* DOPAMINE_BASELINE = "dopamine_baseline";
    constexpr const char* CURIOUSITY_THRESHOLD = "curiosity_threshold";
    constexpr const char* NOVELTY_THRESHOLD = "novelty_threshold";
    constexpr const char* PREDICTION_ERROR_SENSITIVITY = "prediction_error_sensitivity";
    
    // Development parameters
    constexpr const char* DEVELOPMENTAL_STAGE = "developmental_stage";
    constexpr const char* PLASTICITY_DEVELOPMENT_FACTOR = "plasticity_development_factor";
    constexpr const char* CRITICAL_PERIOD_DURATION = "critical_period_duration";
    
    // Simulation parameters
    constexpr const char* CHECKPOINT_DIR = "checkpoint_dir";
    constexpr const char* CHECKPOINT_INTERVAL = "checkpoint_interval";
    constexpr const char* MAX_CHECKPOINTS = "max_checkpoints";
    constexpr const char* REPLAY_INTERVAL = "replay_interval";
    constexpr const char* CONSOLIDATION_INTERVAL = "consolidation_interval";
    
    // Visualization parameters
    constexpr const char* VISUALIZATION_ENABLED = "visualization_enabled";
    constexpr const char* VISUALIZATION_UPDATE_INTERVAL = "visualization_update_interval";
    constexpr const char* PLOTTING_RESOLUTION = "plotting_resolution";
    
    // Performance parameters
    constexpr const char* PERFORMANCE_MODE = "performance_mode";
    constexpr const char* SIMD_ENABLE = "simd_enable";
    constexpr const char* PARALLEL_THREADS = "parallel_threads";
    
    // Experimental parameters
    constexpr const char* EXPERIMENT_MODE = "experiment_mode";
    constexpr const char* STIMULUS_PATTERN = "stimulus_pattern";
    constexpr const char* REWARD_FUNCTION = "reward_function";
    
    // Logging parameters
    constexpr const char* LOG_LEVEL = "log_level";
    constexpr const char* LOG_FILE = "log_file";
    constexpr const char* LOG_TIMESTAMP = "log_timestamp";
}

// Constants for default values
namespace DefaultValues {
    constexpr size_t NEURON_COUNT = 1000;
    constexpr size_t REGION_COUNT = 1;
    constexpr float CONNECTION_PROBABILITY = 0.1f;
    constexpr double SIMULATION_TIMESTEP = 0.001;
    constexpr uint64_t RANDOM_SEED = 42;
    
    constexpr float NEURON_THRESHOLD = -55.0f;
    constexpr float NEURON_RESTING_POTENTIAL = -70.0f;
    constexpr float NEURON_RESET_POTENTIAL = -70.0f;
    constexpr float NEURON_LEAK_CONDUCTANCE = 10.0f;
    constexpr uint32_t NEURON_REFECTORY_PERIOD = 5;
    constexpr float NEURON_ADAPTATION_RATE = 0.01f;
    
    constexpr float SYNAPSE_WEIGHT = 0.0f;
    constexpr uint32_t SYNAPSE_DELAY = 1;
    constexpr uint8_t SYNAPSE_TYPE = 0; // Excitatory
    constexpr float SYNAPSE_MIN_WEIGHT = -1.0f;
    constexpr float SYNAPSE_MAX_WEIGHT = 1.0f;
    
    constexpr float STDP_LTP_WEIGHT = 0.02f;
    constexpr float STDP_LTD_WEIGHT = 0.015f;
    constexpr float STDP_TAU = 20.0f;
    constexpr float HEBBIAN_STRENGTH = 0.1f;
    constexpr float STRUCTURAL_PLASTICITY_RATE = 0.0001f;
    constexpr float PRUNING_RATE = 0.00001f;
    
    constexpr size_t WORKING_MEMORY_CAPACITY = 100;
    constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;
    constexpr float ASSOCIATION_THRESHOLD = 0.5f;
    
    constexpr float DOPAMINE_BASELINE = 0.1f;
    constexpr float CURIOUSITY_THRESHOLD = 0.5f;
    constexpr float NOVELTY_THRESHOLD = 0.1f;
    constexpr float PREDICTION_ERROR_SENSITIVITY = 1.0f;
    
    constexpr uint8_t DEVELOPMENTAL_STAGE = 0; // Initial
    constexpr float PLASTICITY_DEVELOPMENT_FACTOR = 1.0f;
    constexpr float CRITICAL_PERIOD_DURATION = 100.0f;
    
    constexpr const char* CHECKPOINT_DIR = "./checkpoints";
    constexpr size_t CHECKPOINT_INTERVAL = 1000;
    constexpr size_t MAX_CHECKPOINTS = 10;
    constexpr size_t REPLAY_INTERVAL = 100;
    constexpr size_t CONSOLIDATION_INTERVAL = 1000;
    
    constexpr bool VISUALIZATION_ENABLED = false;
    constexpr double VISUALIZATION_UPDATE_INTERVAL = 0.1;
    constexpr uint32_t PLOTTING_RESOLUTION = 1920;
    
    constexpr const char* PERFORMANCE_MODE = "balanced";
    constexpr bool SIMD_ENABLE = true;
    constexpr int32_t PARALLEL_THREADS = -1; // Auto-detect
    
    constexpr const char* EXPERIMENT_MODE = "default";
    constexpr const char* STIMULUS_PATTERN = "random";
    constexpr const char* REWARD_FUNCTION = "linear";
    
    constexpr const char* LOG_LEVEL = "info";
    constexpr const char* LOG_FILE = "nlm.log";
    constexpr bool LOG_TIMESTAMP = true;
}

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
};

} // namespace nlm
