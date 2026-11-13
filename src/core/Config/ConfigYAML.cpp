#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

namespace nlm {

bool Config::loadFromYAML(const std::string& filepath) {
    // YAML support would go here if we had a YAML library
    // For now, this is a placeholder
    NLM_LOG_WARN("YAML config loading not implemented yet, using simple parser");
    return loadFromFile(filepath);
}

bool Config::saveToYAML(const std::string& filepath) const {
    // YAML support would go here if we had a YAML library
    // For now, this is a placeholder that uses JSON format
    return saveToJSON(filepath);
}

bool Config::validate() const {
    // Basic validation - can be extended with schema validation
    for (const auto& entry : pImpl->entries) {
        // Validate based on key
        if (entry.key == "neuron_count" && !hasValidNeuronCount()) {
            NLM_LOG_ERROR("Invalid neuron_count value: " + std::to_string(getOr<int64_t>(entry.key, 0)));
            return false;
        }
        
        if (entry.key == "connection_probability" && !hasValidConnectionProbability()) {
            NLM_LOG_ERROR("Invalid connection_probability value: " + std::to_string(getOr<double>(entry.key, 0.0)));
            return false;
        }
        
        if (entry.key == "simulation_timestep" && !hasValidTimestep()) {
            NLM_LOG_ERROR("Invalid simulation_timestep value: " + std::to_string(getOr<double>(entry.key, 0.0)));
            return false;
        }
    }
    return true;
}

bool Config::hasValidNeuronCount() const {
    int64_t count = getOr<int64_t>("neuron_count", 0);
    return count > 0 && count <= 100000;  // Reasonable limits
}

bool Config::hasValidConnectionProbability() const {
    double prob = getOr<double>("connection_probability", 0.0);
    return prob >= 0.0 && prob <= 1.0;
}

bool Config::hasValidTimestep() const {
    double timestep = getOr<double>("simulation_timestep", 0.0);
    return timestep > 0.0 && timestep <= 1.0;  // Reasonable timestep
}

} // namespace nlm