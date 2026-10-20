#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <numeric>
#include <limits>
#include <typeinfo>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
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

// Configuration schema definition
struct ConfigSchema {
    struct Field {
        std::string name;
        std::string type;  // "int64_t", "double", "float", "string", "bool"
        bool required;
        std::string defaultValue;
        std::function<bool(const std::string&)> validator;
        std::string description;
    };
    
    static const std::vector<Field>& getSchema() {
        static const std::vector<Field> schema = {
            // Phase 2: Real Neural Computation fields
            {"random_seed", "int64_t", true, "42", 
             [](const std::string& v) { 
                 try { 
                     size_t seed = std::stoull(v); 
                     return seed > 0 && seed < UINT64_MAX; 
                 } catch (...) { return false; } 
             }, 
             "Random seed for neural network initialization"},
            
            {"simulation_timestep", "double", true, "0.001",
             [](const std::string& v) {
                 try {
                     double t = std::stod(v);
                     return t > 0.0 && t < 1.0;  // Reasonable timestep range
                 } catch (...) { return false; }
             },
             "Simulation time step in seconds"},
            
            {"neuron_count", "int64_t", true, "1000",
             [](const std::string& v) {
                 try {
                     int64_t count = std::stoll(v);
                     return count > 0 && count < 100000;
                 } catch (...) { return false; }
             },
             "Number of neurons in the brain"},
            
            {"region_count", "int64_t", true, "1",
             [](const std::string& v) {
                 try {
                     int64_t count = std::stoll(v);
                     return count > 0 && count < 1000;
                 } catch (...) { return false; }
             },
             "Number of neural regions"},
            
            {"connection_probability", "float", true, "0.1",
             [](const std::string& v) {
                 try {
                     float p = std::stof(v);
                     return p >= 0.0f && p <= 1.0f;
                 } catch (...) { return false; }
             },
             "Probability of connection between neurons (0-1)"},
            
            {"stdp_ltp_weight", "float", true, "0.01",
             [](const std::string& v) {
                 try {
                     float w = std::stof(v);
                     return w >= 0.0f && w <= 1.0f;
                 } catch (...) { return false; }
             },
             "STDP LTP weight parameter"},
            
            {"stdp_ltd_weight", "float", true, "0.012",
             [](const std::string& v) {
                 try {
                     float w = std::stof(v);
                     return w >= 0.0f && w <= 1.0f;
                 } catch (...) { return false; }
             },
             "STDP LTD weight parameter"},
            
            {"stdp_tau", "float", true, "20.0",
             [](const std::string& v) {
                 try {
                     float tau = std::stof(v);
                     return tau > 0.0f && tau < 1000.0f;
                 } catch (...) { return false; }
             },
             "STDP time constant"},
            
            {"synaptogenesis_rate", "float", true, "0.0001",
             [](const std::string& v) {
                 try {
                     float r = std::stof(v);
                     return r >= 0.0f && r <= 0.01f;
                 } catch (...) { return false; }
             },
             "Synaptogenesis rate"},
            
            {"pruning_rate", "float", true, "0.00001",
             [](const std::string& v) {
                 try {
                     float r = std::stof(v);
                     return r >= 0.0f && r <= 0.001f;
                 } catch (...) { return false; }
             },
             "Pruning rate"},
            
            {"replay_interval", "int64_t", true, "100",
             [](const std::string& v) {
                 try {
                     int64_t steps = std::stoll(v);
                     return steps > 0 && steps < 1000000;
                 } catch (...) { return false; }
             },
             "Replay interval in simulation steps"},
            
            {"consolidation_interval", "int64_t", true, "1000",
             [](const std::string& v) {
                 try {
                     int64_t steps = std::stoll(v);
                     return steps > 0 && steps < 1000000;
                 } catch (...) { return false; }
             },
             "Memory consolidation interval"},
            
            {"checkpoint_dir", "string", true, "./checkpoints",
             [](const std::string& v) {
                 return !v.empty() && v.find('\0') == std::string::npos;
             },
             "Checkpoint directory"},
            
            {"reward_modulation", "bool", false, "false",
             [](const std::string& v) {
                 return toLower(v) == "true" || toLower(v) == "1" || toLower(v) == "yes";
             },
             "Enable reward modulation"},
            
            {"structural_plasticity", "bool", false, "false",
             [](const std::string& v) {
                 return toLower(v) == "true" || toLower(v) == "1" || toLower(v) == "yes";
             },
             "Enable structural plasticity"},
            
            {"development_enabled", "bool", false, "false",
             [](const std::string& v) {
                 return toLower(v) == "true" || toLower(v) == "1" || toLower(v) == "yes";
             },
             "Enable development system"},
            
            {"curiosity_enabled", "bool", false, "false",
             [](const std::string& v) {
                 return toLower(v) == "true" || toLower(v) == "1" || toLower(v) == "yes";
             },
             "Enable curiosity system"},
            
            {"learning_rate", "float", false, "0.01",
             [](const std::string& v) {
                 try {
                     float r = std::stof(v);
                     return r >= 0.0f && r <= 1.0f;
                 } catch (...) { return false; }
             },
             "Learning rate"},
            
            {"innovation_rate", "float", false, "0.05",
             [](const std::string& v) {
                 try {
                     float r = std::stof(v);
                     return r >= 0.0f && r <= 1.0f;
                 } catch (...) { return false; }
             },
             "Innovation rate"},
            
            {"exploration_rate", "float", false, "0.2",
             [](const std::string& v) {
                 try {
                     float r = std::stof(v);
                     return r >= 0.0f && r <= 1.0f;
                 } catch (...) { return false; }
             },
             "Exploration rate"},
            
            {"max_memory_traces", "int64_t", false, "1000",
             [](const std::string& v) {
                 try {
                     int64_t traces = std::stoll(v);
                     return traces > 0 && traces < 1000000;
                 } catch (...) { return false; }
             },
             "Maximum working memory traces"},
            
            {"max_episodes", "int64_t", false, "10000",
             [](const std::string& v) {
                 try {
                     int64_t episodes = std::stoll(v);
                     return episodes > 0 && episodes < 100000;
                 } catch (...) { return false; }
             },
             "Maximum episodic memory episodes"},
            
            {"planning_depth", "int64_t", false, "5",
             [](const std::string& v) {
                 try {
                     int64_t depth = std::stoll(v);
                     return depth > 0 && depth < 20;
                 } catch (...) { return false; }
             },
             "Planning depth for neural planner"},
            
            {"inhibition_strength", "float", false, "0.5",
             [](const std::string& v) {
                 try {
                     float strength = std::stof(v);
                     return strength >= 0.0f && strength <= 2.0f;
                 } catch (...) { return false; }
             },
             "Attention inhibition strength"},
            
            {"excitation_strength", "float", false, "1.5",
             [](const std::string& v) {
                 try {
                     float strength = std::stof(v);
                     return strength >= 0.0f && strength <= 3.0f;
                 } catch (...) { return false; }
             },
             "Attention excitation strength"}
        };
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
    
    // New type-safe validated access
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue = T()) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Configuration validation methods
    bool validate() const;
    bool validateValue(const std::string& key, const std::string& value) const;
    std::string getValidationError(const std::string& key, const std::string& value) const;
    
    // Static validation methods
    static bool validateInt64(const std::string& value, int64_t min = INT64_MIN, int64_t max = INT64_MAX);
    static bool validateDouble(const std::string& value, double min = -std::numeric_limits<double>::infinity(), 
                               double max = std::numeric_limits<double>::infinity());
    static bool validateFloat(const std::string& value, float min = -std::numeric_limits<float>::infinity(),
                              float max = std::numeric_limits<float>::infinity());
    static bool validateString(const std::string& value, size_t minLength = 0, size_t maxLength = std::string::npos);
    static bool validateBool(const std::string& value);
    
    // Specialized validation methods
    static bool validateNeuronCount(const std::string& value);
    static bool validateRegionCount(const std::string& value);
    static bool validateConnectionProbability(const std::string& value);
    static bool validateSTDPParameters(const std::string& value, const std::string& paramType);
    static bool validatePlasticityRate(const std::string& value, const std::string& rateType);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
