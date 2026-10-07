// Example configuration schema for NLM
// This schema validates the default.cfg configuration with proper type checking and range validation

#pragma once

#include "Validation.hpp"
#include <string>

namespace nlm {

// Create a default configuration schema
inline ConfigSchema createDefaultConfigSchema() {
    ConfigSchema schema;
    
    // Random seed validation
    schema.addEntry<int>("random_seed", false, "Random seed for reproducible experiments")
        .addEntry("random_seed", false, std::bind(&ConfigSchema::addEntry<double>, &schema, "random_seed", false, "Random seed", 
            [](const ConfigValue& val) {
                try {
                    double d = std::get<double>(val);
                    return d >= 0.0 && d <= 1000000.0;
                } catch (...) { return false; }
            }));
    
    // Simulation timestep validation  
    schema.addEntry<double>("simulation_timestep", false, "Simulation timestep in seconds")
        .addEntry("simulation_timestep", false, 
            [](const ConfigValue& val) {
                try {
                    double d = std::get<double>(val);
                    return d > 0.0 && d <= 0.1; // Reasonable timestep range
                } catch (...) { return false; }
            });
    
    // Neuron count validation
    schema.addEntry<int>("neuron_count", false, "Number of neurons in the brain")
        .addEntry("neuron_count", false, 
            [](const ConfigValue& val) {
                try {
                    int n = std::get<int>(val);
                    return n > 0 && n <= 100000; // Reasonable neuron count
                } catch (...) { return false; }
            });
    
    // Region count validation
    schema.addEntry<int>("region_count", false, "Number of brain regions")
        .addEntry("region_count", false, 
            [](const ConfigValue& val) {
                try {
                    int r = std::get<int>(val);
                    return r > 0 && r <= 100;
                } catch (...) { return false; }
            });
    
    // Connection probability validation
    schema.addEntry<double>("connection_probability", false, "Connection probability for random connectivity")
        .addEntry("connection_probability", false,
            [](const ConfigValue& val) {
                try {
                    double p = std::get<double>(val);
                    return p >= 0.0 && p <= 1.0;
                } catch (...) { return false; }
            });
    
    // Plasticity settings validation
    schema.addEntry<double>("plasticity_learning_rate", false, "Plasticity learning rate")
        .addEntry("plasticity_learning_rate", false,
            [](const ConfigValue& val) {
                try {
                    double lr = std::get<double>(val);
                    return lr >= 0.0 && lr <= 1.0;
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("stdp_ltp_weight", false, "STDP LTP weight")
        .addEntry("stdp_ltp_weight", false,
            [](const ConfigValue& val) {
                try {
                    double w = std::get<double>(val);
                    return w >= 0.0 && w <= 1.0;
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("stdp_ltd_weight", false, "STDP LTD weight")
        .addEntry("stdp_ltd_weight", false,
            [](const ConfigValue& val) {
                try {
                    double w = std::get<double>(val);
                    return w >= 0.0 && w <= 1.0;
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("stdp_time_constant", false, "STDP time constant")
        .addEntry("stdp_time_constant", false,
            [](const ConfigValue& val) {
                try {
                    double tc = std::get<double>(val);
                    return tc >= 0.0 && tc <= 1000.0;
                } catch (...) { return false; }
            });
    
    // Development settings validation
    schema.addEntry<double>("development_synaptogenesis_rate", false, "Development synaptogenesis rate")
        .addEntry("development_synaptogenesis_rate", false,
            [](const ConfigValue& val) {
                try {
                    double r = std::get<double>(val);
                    return r >= 0.0 && r <= 1.0;
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("development_pruning_rate", false, "Development pruning rate")
        .addEntry("development_pruning_rate", false,
            [](const ConfigValue& val) {
                try {
                    double r = std::get<double>(val);
                    return r >= 0.0 && r <= 1.0;
                } catch (...) { return false; }
            });
    
    // Neuromodulation settings validation
    schema.addEntry<double>("dopamine_baseline", false, "Dopamine baseline value")
        .addEntry("dopamine_baseline", false,
            [](const ConfigValue& val) {
                try {
                    double d = std::get<double>(val);
                    return d >= 0.0 && d <= 10.0;
                } catch (...) { return false; }
            });
    
    // Reward settings validation
    schema.addEntry<double>("reward_discount_factor", false, "Reward discount factor")
        .addEntry("reward_discount_factor", false,
            [](const ConfigValue& val) {
                try {
                    double f = std::get<double>(val);
                    return f >= 0.0 && f <= 1.0;
                } catch (...) { return false; }
            });
    
    // Environment settings validation
    schema.addEntry<std::string>("environment_name", false, "Environment name")
        .addEntry("environment_name", false,
            [](const ConfigValue& val) {
                try {
                    std::string name = std::get<std::string>(val);
                    return !name.empty() && name.length() <= 100;
                } catch (...) { return false; }
            });
    
    schema.addEntry<int>("environment_width", false, "Environment width")
        .addEntry("environment_width", false,
            [](const ConfigValue& val) {
                try {
                    int w = std::get<int>(val);
                    return w > 0 && w <= 1000;
                } catch (...) { return false; }
            });
    
    schema.addEntry<int>("environment_height", false, "Environment height")
        .addEntry("environment_height", false,
            [](const ConfigValue& val) {
                try {
                    int h = std::get<int>(val);
                    return h > 0 && h <= 1000;
                } catch (...) { return false; }
            });
    
    // Logging settings validation
    schema.addEntry<std::string>("log_level", false, "Log level")
        .addEntry("log_level", false,
            [](const ConfigValue& val) {
                try {
                    std::string level = std::get<std::string>(val);
                    static const std::set<std::string> validLevels = {"DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};
                    return validLevels.find(level) != validLevels.end();
                } catch (...) { return false; }
            });
    
    schema.addEntry<bool>("log_to_file", false, "Log to file flag")
        .addEntry("log_to_file", false,
            [](const ConfigValue& val) {
                try {
                    return std::get<bool>(val);
                } catch (...) { return false; }
            });
    
    schema.addEntry<std::string>("log_filename", false, "Log filename")
        .addEntry("log_filename", false,
            [](const ConfigValue& val) {
                try {
                    std::string filename = std::get<std::string>(val);
                    return !filename.empty() && filename.length() <= 255;
                } catch (...) { return false; }
            });
    
    // Simulation settings validation
    schema.addEntry<int>("max_simulation_steps", false, "Maximum simulation steps")
        .addEntry("max_simulation_steps", false,
            [](const ConfigValue& val) {
                try {
                    int steps = std::get<int>(val);
                    return steps > 0 && steps <= 1000000;
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("simulation_time_limit", false, "Simulation time limit")
        .addEntry("simulation_time_limit", false,
            [](const ConfigValue& val) {
                try {
                    double limit = std::get<double>(val);
                    return limit >= 0.0;
                } catch (...) { return false; }
            });
    
    // Visualization settings validation
    schema.addEntry<bool>("visualization_enabled", false, "Visualization enabled flag")
        .addEntry("visualization_enabled", false,
            [](const ConfigValue& val) {
                try {
                    return std::get<bool>(val);
                } catch (...) { return false; }
            });
    
    schema.addEntry<double>("visualization_update_rate", false, "Visualization update rate")
        .addEntry("visualization_update_rate", false,
            [](const ConfigValue& val) {
                try {
                    double rate = std::get<double>(val);
                    return rate > 0.0 && rate <= 1000.0;
                } catch (...) { return false; }
            });
    
    return schema;
}

} // namespace nlm