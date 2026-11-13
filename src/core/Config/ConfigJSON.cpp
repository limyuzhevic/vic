#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;

namespace nlm {

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        clear();
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Try to convert based on JSON type
            if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, it->get<uint64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Handle arrays
                if (it->empty()) {
                    set(key, std::vector<double>(), ConfigSource::File);
                } else if (std::all_of(it->begin(), it->end(), [](const auto& v) { return v.is_number_integer(); })) {
                    std::vector<int> intArray;
                    for (const auto& v : *it) {
                        intArray.push_back(v.get<int>());
                    }
                    set(key, intArray, ConfigSource::File);
                } else if (std::all_of(it->begin(), it->end(), [](const auto& v) { return v.is_number_float(); })) {
                    std::vector<double> doubleArray;
                    for (const auto& v : *it) {
                        doubleArray.push_back(v.get<double>());
                    }
                    set(key, doubleArray, ConfigSource::File);
                } else if (std::all_of(it->begin(), it->end(), [](const auto& v) { return v.is_string(); })) {
                    std::vector<std::string> stringArray;
                    for (const auto& v : *it) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    set(key, stringArray, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load JSON config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // YAML support would go here if we had a YAML library
    // For now, this is a placeholder
    NLM_LOG_WARN("YAML config loading not implemented yet, using simple parser");
    return loadFromFile(filepath);
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                            std::is_same_v<T, uint64_t>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[entry.key] = arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);  // 2-space indentation
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save JSON config to " + filepath + ": " + std::string(e.what()));
        return false;
    }
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