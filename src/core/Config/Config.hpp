#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>

#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
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
};

// Configuration schema for validation
struct ConfigSchema {
    struct Field {
        std::string name;
        std::string type; // "int", "double", "bool", "string", "int64_t"
        std::string description;
        bool required;
        ConfigValue defaultValue;
        
        Field(std::string n, std::string t, std::string d = "", bool req = false, 
              ConfigValue def = std::string(""))
            : name(std::move(n)), type(std::move(t)), description(std::move(d)), 
              required(req), defaultValue(std::move(def)) {}
        
        bool isValidType(const ConfigValue& value) const {
            switch (type[0]) {
                case 'i': return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
                case 'd': return std::holds_alternative<double>(value);
                case 'b': return std::holds_alternative<bool>(value);
                case 's': return std::holds_alternative<std::string>(value);
                default: return false;
            }
        }
    };
    
    static const std::vector<Field> getDefaultSchema() {
        return {
            {"random_seed", "int64_t", "Random seed for reproducible experiments", false, int64_t(42)},
            {"simulation_timestep", "double", "Simulation timestep in seconds", false, 0.001},
            {"neuron_count", "int64_t", "Number of neurons in the brain", false, int64_t(1000)},
            {"region_count", "int64_t", "Number of brain regions", false, int64_t(2)},
            {"connection_probability", "double", "Connection probability for random connectivity", false, 0.1},
            {"stdp_ltp_weight", "double", "STDP LTP weight", false, 0.01},
            {"stdp_ltd_weight", "double", "STDP LTD weight", false, 0.012},
            {"stdp_tau", "double", "STDP time constant", false, 20.0},
            {"synaptogenesis_rate", "double", "Synaptogenesis rate", false, 0.0001},
            {"pruning_rate", "double", "Pruning rate", false, 0.00001},
            {"neuromodulation_dopamine_baseline", "double", "Dopamine baseline for neuromodulation", false, 0.1},
            {"reward_discount_factor", "double", "Reward discount factor", false, 0.99},
            {"environment_name", "string", "Environment name", false, std::string("SimpleWorld")},
            {"environment_width", "int64_t", "Environment width", false, int64_t(16)},
            {"environment_height", "int64_t", "Environment height", false, int64_t(16)},
            {"log_level", "string", "Log level (Debug, Info, Warning, Error, Critical)", false, std::string("INFO")},
            {"log_to_file", "bool", "Whether to log to file", false, false},
            {"log_filename", "string", "Log filename", false, std::string("nlm.log")},
            {"max_simulation_steps", "int64_t", "Maximum simulation steps", false, int64_t(5000)},
            {"simulation_time_limit", "double", "Simulation time limit in seconds", false, 0.0},
            {"working_memory_capacity", "int64_t", "Working memory capacity", false, int64_t(100)},
            {"episodic_memory_max_episodes", "int64_t", "Maximum episodic memory episodes", false, int64_t(1000)},
            {"enable_checkpointing", "bool", "Enable checkpointing", false, true},
            {"checkpoint_path", "string", "Checkpoint file path", false, std::string("./checkpoints/nlm_checkpoint")},
            {"replay_interval", "int64_t", "Replay interval in steps", false, int64_t(100)},
            {"consolidation_interval", "int64_t", "Consolidation interval in steps", false, int64_t(1000)},
        };
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
    
    // Load from JSON string
    void loadFromJSON(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
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
    
    // Validate configuration
    static bool validate(const Config& config, std::vector<std::string>& errors);
    static std::string getValidationErrors(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
