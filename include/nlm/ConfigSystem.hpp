#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration value types for JSON compatibility
using JsonValue = nlohmann::json;

// Configuration system with JSON file support
class ConfigSystem {
public:
    ConfigSystem();
    ~ConfigSystem();
    
    // Load configuration from JSON file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void set(const std::string& key, const JsonValue& value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration summary as JSON
    JsonValue summary() const;
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Default configuration template
dstruct DefaultConfig {
    static JsonValue getDefaults() {
        return JsonValue{
            {"random_seed", 42},
            {"simulation_timestep", 0.001},
            {"neuron_count", 500},
            {"region_count", 1},
            {"connection_probability", 0.15},
            {"stdp_ltp_weight", 0.01},
            {"stdp_ltd_weight", 0.012},
            {"stdp_tau", 20.0},
            {"synaptogenesis_rate", 0.0001},
            {"pruning_rate", 0.00001},
            {"enable_reward_modulation", true},
            {"enable_structural_plasticity", true},
            {"enable_development", true},
            {"enable_curiosity", true},
            {"enable_novelty", true},
            {"max_steps", 1000},
            {"checkpoint_path", "/tmp/nlm_checkpoint.bin"}
        };
    }
};

} // namespace nlm