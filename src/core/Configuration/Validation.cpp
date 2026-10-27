#include "Validation.hpp"
#include "core/Logger/Logger.hpp"
#include <string>

using namespace nlm;

std::vector<std::string> ConfigurationValidator::validate(const ConfigEntry& entry) const {
    std::vector<std::string> errors;
    
    if (entry.key.empty()) {
        errors.push_back("Configuration entry has empty key");
        return errors;
    }
    
    // Validate based on key
    if (entry.key == "neuron_count") {
        auto* value = std::get_if<int64_t>(&entry.value);
        if (value && !validateNeuronCount(*value)) {
            errors.push_back("Invalid neuron count: " + std::to_string(*value));
        }
    } else if (entry.key == "region_count") {
        auto* value = std::get_if<int64_t>(&entry.value);
        if (value && !validateRegionCount(*value)) {
            errors.push_back("Invalid region count: " + std::to_string(*value));
        }
    } else if (entry.key == "connection_probability") {
        auto* value = std::get_if<float>(&entry.value);
        if (value && !validateConnectionProbability(*value)) {
            errors.push_back("Invalid connection probability: " + std::to_string(*value));
        }
    } else if (entry.key == "simulation_timestep") {
        auto* value = std::get_if<double>(&entry.value);
        if (value && !validateTimestep(*value)) {
            errors.push_back("Invalid timestep: " + std::to_string(*value));
        }
    }
    
    return errors;
}

bool ConfigurationValidator::validateNeuronCount(int64_t count) const {
    return count > 0 && count <= 100000;
}

bool ConfigurationValidator::validateRegionCount(int64_t count) const {
    return count > 0 && count <= 100;
}

bool ConfigurationValidator::validateConnectionProbability(float prob) const {
    return prob >= 0.0f && prob <= 1.0f;
}

bool ConfigurationValidator::validateTimestep(double timestep) const {
    return timestep > 0.0 && timestep <= 1.0;
}