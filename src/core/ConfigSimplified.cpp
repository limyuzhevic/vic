// Implementation of simplified configuration with backward compatibility
// Combines easy-to-use API with original complex Config class

#include "ConfigSimplified.hpp"
#include <algorithm>
#include <unordered_set>

namespace nlm {

struct ConfigSimplified::Impl {
    // Simple map for configuration values
    struct ValueStorage {
        bool boolValue;
        int intValue;
        int64_t int64Value;
        double doubleValue;
        std::string stringValue;
        bool isBool;
        bool isInt;
        bool isInt64;
        bool isDouble;
        bool isString;
        
        ValueStorage() 
            : boolValue(false), intValue(0), int64Value(0), doubleValue(0.0), 
              stringValue(), isBool(false), isInt(false), isInt64(false), 
              isDouble(false), isString(false) {}
    };
    
    std::unordered_map<std::string, ValueStorage> values;
    std::vector<std::pair<std::string, ConfigSource>> sources;
    
    // Statistics
    size_t getEntryCount() const { return values.size(); }
};

ConfigSimplified::ConfigSimplified() : pImpl(std::make_unique<Impl>()) {}

ConfigSimplified::~ConfigSimplified() = default;

ConfigSimplified::ConfigSimplified(ConfigSimplified&&) noexcept = default;

ConfigSimplified& ConfigSimplified::operator=(ConfigSimplified&&) noexcept = default;

void ConfigSimplified::setBool(const std::string& key, bool value, ConfigSource source) {
    pImpl->values[key].boolValue = value;
    pImpl->values[key].isBool = true;
    pImpl->sources.push_back({key, source});
}

void ConfigSimplified::setInt(const std::string& key, int value, ConfigSource source) {
    pImpl->values[key].intValue = value;
    pImpl->values[key].isInt = true;
    pImpl->sources.push_back({key, source});
}

void ConfigSimplified::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    pImpl->values[key].int64Value = value;
    pImpl->values[key].isInt64 = true;
    pImpl->sources.push_back({key, source});
}

void ConfigSimplified::setDouble(const std::string& key, double value, ConfigSource source) {
    pImpl->values[key].doubleValue = value;
    pImpl->values[key].isDouble = true;
    pImpl->sources.push_back({key, source});
}

void ConfigSimplified::setString(const std::string& key, const std::string& value, ConfigSource source) {
    pImpl->values[key].stringValue = value;
    pImpl->values[key].isString = true;
    pImpl->sources.push_back({key, source});
}

bool ConfigSimplified::getBool(const std::string& key, bool defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it != pImpl->values.end() && it->second.isBool) {
        return it->second.boolValue;
    }
    return defaultValue;
}

int ConfigSimplified::getInt(const std::string& key, int defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it != pImpl->values.end() && it->second.isInt) {
        return it->second.intValue;
    }
    return defaultValue;
}

int64_t ConfigSimplified::getInt64(const std::string& key, int64_t defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it != pImpl->values.end() && it->second.isInt64) {
        return it->second.int64Value;
    }
    return defaultValue;
}

double ConfigSimplified::getDouble(const std::string& key, double defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it != pImpl->values.end() && it->second.isDouble) {
        return it->second.doubleValue;
    }
    return defaultValue;
}

std::string ConfigSimplified::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it != pImpl->values.end() && it->second.isString) {
        return it->second.stringValue;
    }
    return defaultValue;
}

bool ConfigSimplified::has(const std::string& key) const {
    return pImpl->values.find(key) != pImpl->values.end();
}

void ConfigSimplified::remove(const std::string& key) {
    pImpl->values.erase(key);
    pImpl->sources.erase(
        std::remove_if(pImpl->sources.begin(), pImpl->sources.end(),
            [&key](const auto& pair) { return pair.first == key; }),
        pImpl->sources.end()
    );
}

std::vector<std::string> ConfigSimplified::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->values.size());
    for (const auto& pair : pImpl->values) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigSimplified::clear() {
    pImpl->values.clear();
    pImpl->sources.clear();
}

ConfigSimplified ConfigSimplified::createBrainConfig() {
    ConfigSimplified config;
    
    // Basic brain configuration
    config.setBool("neuron_debug", false);
    config.setInt("neuron_count", 1000);
    config.setInt("region_count", 1);
    config.setDouble("connection_probability", 0.1f);
    config.setDouble("simulation_timestep", 0.001);
    config.setInt64("random_seed", 42);
    
    // STDP parameters
    config.setDouble("stdp_ltp_weight", 0.02f);
    config.setDouble("stdp_ltd_weight", 0.015f);
    config.setDouble("stdp_tau", 20.0f);
    
    // Structural plasticity parameters
    config.setDouble("synaptogenesis_rate", 0.0001f);
    config.setDouble("pruning_rate", 0.00001f);
    
    // Memory system configuration
    config.setInt("working_memory_capacity", 100);
    config.setInt("episodic_memory_max_episodes", 1000);
    
    // Neuromodulation configuration
    config.setBool("enable_reward_modulation", true);
    config.setBool("enable_curiosity", true);
    config.setBool("enable_novelty_detection", true);
    config.setBool("enable_prediction_error", true);
    
    // Development configuration
    config.setBool("enable_development", true);
    config.setInt("development_stage_duration", 1000);
    
    return config;
}

ConfigSimplified ConfigSimplified::createAdvancedConfig() {
    ConfigSimplified config = createBrainConfig();
    
    // Advanced configuration for experienced users
    config.setBool("neuron_debug", true);
    config.setInt("neuron_count", 5000);
    config.setInt("region_count", 4);
    config.setDouble("connection_probability", 0.15f);
    config.setDouble("simulation_timestep", 0.0005);
    config.setInt64("random_seed", 12345);
    
    // Advanced STDP parameters
    config.setDouble("stdp_ltp_weight", 0.05f);
    config.setDouble("stdp_ltd_weight", 0.04f);
    config.setDouble("stdp_tau", 25.0f);
    
    // Advanced structural plasticity
    config.setDouble("synaptogenesis_rate", 0.0005f);
    config.setDouble("pruning_rate", 0.00005f);
    
    // Advanced memory systems
    config.setInt("working_memory_capacity", 500);
    config.setInt("episodic_memory_max_episodes", 5000);
    
    // Advanced neuromodulation
    config.setBool("enable_reward_modulation", true);
    config.setBool("enable_curiosity", true);
    config.setBool("enable_novelty_detection", true);
    config.setBool("enable_prediction_error", true);
    
    // Advanced development
    config.setBool("enable_development", true);
    config.setInt("development_stage_duration", 5000);
    
    // Performance optimizations
    config.setBool("use_synaptic_efficacy", true);
    config.setDouble("eligibility_trace_decay", 0.001f);
    config.setInt("max_synapses_per_neuron", 500);
    config.setInt64("max_total_synapses", 10000000);
    
    return config;
}

ConfigSimplified ConfigSimplified::createDevelopmentConfig() {
    ConfigSimplified config;
    
    // Development-focused configuration
    config.setBool("neuron_debug", false);
    config.setInt("neuron_count", 500);
    config.setInt("region_count", 2);
    config.setDouble("connection_probability", 0.2f);
    config.setDouble("simulation_timestep", 0.002);
    config.setInt64("random_seed", 999);
    
    // Development-oriented STDP
    config.setDouble("stdp_ltp_weight", 0.03f);
    config.setDouble("stdp_ltd_weight", 0.025f);
    config.setDouble("stdp_tau", 30.0f);
    
    // Developmental structural plasticity
    config.setDouble("synaptogenesis_rate", 0.0002f);
    config.setDouble("pruning_rate", 0.00002f);
    
    // Development memory systems
    config.setInt("working_memory_capacity", 200);
    config.setInt("episodic_memory_max_episodes", 2000);
    
    // Development neuromodulation
    config.setBool("enable_reward_modulation", true);
    config.setBool("enable_curiosity", true);
    config.setBool("enable_novelty_detection", true);
    config.setBool("enable_prediction_error", true);
    
    // Enable development
    config.setBool("enable_development", true);
    config.setInt("development_stage_duration", 2000);
    config.setBool("enable_critical_period", true);
    config.setBool("enable_maturation", true);
    
    // Development-specific parameters
    config.setDouble("development_plasticity_mod", 1.5f);
    config.setBool("use_developmental_stages", true);
    
    return config;
}

bool ConfigSimplified::validate() const {
    // Basic validation
    if (pImpl->values.empty()) {
        return false;  // Empty configuration
    }
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;  // Missing required key
        }
    }
    
    // Validate ranges
    int neuronCount = getInt("neuron_count");
    if (neuronCount <= 0 || neuronCount > 1000000) {
        return false;  // Invalid neuron count
    }
    
    float connectionProb = static_cast<float>(getDouble("connection_probability"));
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        return false;  // Invalid connection probability
    }
    
    float timestep = static_cast<float>(getDouble("simulation_timestep"));
    if (timestep <= 0.0f || timestep > 1.0f) {
        return false;  // Invalid timestep
    }
    
    return true;  // All validation passed
}

void ConfigSimplified::merge(const ConfigSimplified& other) {
    // Merge other config into this one
    for (const auto& key : other.getKeys()) {
        if (!has(key)) {
            // Copy value from other config
            if (other.getBool(key, false)) {
                setBool(key, other.getBool(key));
            } else if (other.getInt(key, 0) != 0) {
                setInt(key, other.getInt(key));
            } else if (other.getInt64(key, 0) != 0) {
                setInt64(key, other.getInt64(key));
            } else if (other.getDouble(key, 0.0) != 0.0) {
                setDouble(key, other.getDouble(key));
            } else if (!other.getString(key, "").empty()) {
                setString(key, other.getString(key));
            }
        }
    }
}

} // namespace nlm