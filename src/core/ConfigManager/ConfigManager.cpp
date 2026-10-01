// Configuration Manager Implementation

#include "ConfigManager.hpp"
#include <sstream>

namespace nlm {
    
void ConfigManager::applyConfigOverride(const std::string& key, const std::string& value) {
    std::istringstream iss(value);
    
    // Try different types based on the key
    if (key == "random_seed") {
        int64_t intVal;
        if (iss >> intVal) {
            config->set(key, intVal, ConfigSource::CommandLine);
        }
    } else if (key == "simulation_timestep") {
        double doubleVal;
        if (iss >> doubleVal) {
            config->set(key, doubleVal, ConfigSource::CommandLine);
        }
    } else if (key == "neuron_count") {
        int64_t intVal;
        if (iss >> intVal) {
            config->set(key, intVal, ConfigSource::CommandLine);
        }
    } else if (key == "region_count") {
        int64_t intVal;
        if (iss >> intVal) {
            config->set(key, intVal, ConfigSource::CommandLine);
        }
    } else if (key == "connection_probability") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    } else if (key == "stdp_ltp_weight") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    } else if (key == "stdp_ltd_weight") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    } else if (key == "stdp_tau") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    } else if (key == "synaptogenesis_rate") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    } else if (key == "pruning_rate") {
        float floatVal;
        if (iss >> floatVal) {
            config->set(key, floatVal, ConfigSource::CommandLine);
        }
    }
    // Note: String values could be added here if needed
}

} // namespace nlm