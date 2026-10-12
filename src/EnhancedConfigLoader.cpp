// EnhancedConfigLoader implementation
// Moved from main.cpp to separate implementation file

#include "EnhancedConfigLoader.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>

namespace nlm {

bool EnhancedConfigLoader::loadConfig(Config& config, const std::string& configFile,
                                     const std::vector<std::string>& extraConfigFiles) {
    // Check if file exists
    std::ifstream file(configFile);
    if (!file.good()) {
        return false;
    }
    
    // Simple key=value parser (backward compatibility)
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config.set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool EnhancedConfigLoader::loadFromEnvironment(Config& config) {
    bool loaded = false;
    
    for (const auto& envPair : environmentPrefixes) {
        const char* envValue = std::getenv(envPair.first.c_str());
        if (envValue != nullptr) {
            std::string envStr(envValue);
            
            // Try to parse as different types
            try {
                // Try as integer
                size_t pos;
                int intValue = std::stoi(envStr, &pos);
                if (pos == envStr.size()) {
                    config.set(envPair.second, intValue, ConfigSource::Environment);
                    loaded = true;
                    continue;
                }
                
                // Try as float
                float floatValue = std::stof(envStr, &pos);
                if (pos == envStr.size()) {
                    config.set(envPair.second, floatValue, ConfigSource::Environment);
                    loaded = true;
                    continue;
                }
                
                // Try as boolean
                if (envStr == "true" || envStr == "1" || envStr == "yes") {
                    config.set(envPair.second, true, ConfigSource::Environment);
                    loaded = true;
                    continue;
                } else if (envStr == "false" || envStr == "0" || envStr == "no") {
                    config.set(envPair.second, false, ConfigSource::Environment);
                    loaded = true;
                    continue;
                }
                
                // Default to string
                config.set(envPair.second, envStr, ConfigSource::Environment);
                loaded = true;
                
            } catch (const std::exception&) {
                // Parse as string
                config.set(envPair.second, envStr, ConfigSource::Environment);
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool EnhancedConfigLoader::validateConfig(const Config& config) const {
    // Check for required keys with defaults
    std::vector<std::pair<std::string, ConfigValue>> defaults = {
        {"random_seed", int64_t(42)},
        {"simulation_timestep", 0.001},
        {"neuron_count", int64_t(500)},
        {"region_count", int64_t(1)},
        {"connection_probability", 0.15f},
        {"stdp_ltp_weight", 0.02f},
        {"stdp_ltd_weight", 0.015f},
        {"synaptogenesis_rate", 0.0001f},
        {"pruning_rate", 0.00001f},
        {"dopamine_baseline", 0.1f}
    };
    
    bool valid = true;
    for (const auto& defaultPair : defaults) {
        if (!config.has(defaultPair.first)) {
            NLM_LOG_WARNING("Missing configuration: " + defaultPair.first + ", using default");
            // Note: In real implementation, we would set defaults here
        }
    }
    
    return valid;
}

void EnhancedConfigLoader::printConfigSummary(const Config& config) const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    // Get important configuration values
    auto printConfig = [&](const std::string& key, const std::string& description) {
        if (config.has(key)) {
            auto val = config.get<std::string>(key);
            if (val.has_value()) {
                NLM_LOG_INFO("  " + description + ": " + val.value());
            }
        }
    };
    
    printConfig("random_seed", "Random seed");
    printConfig("simulation_timestep", "Simulation timestep (s)");
    printConfig("neuron_count", "Number of neurons");
    printConfig("region_count", "Number of regions");
    printConfig("connection_probability", "Connection probability");
    printConfig("stdp_ltp_weight", "STDP LTP weight");
    printConfig("stdp_ltd_weight", "STDP LTD weight");
    printConfig("plasticity_learning_rate", "Plasticity learning rate");
    printConfig("synaptogenesis_rate", "Synaptogenesis rate");
    printConfig("pruning_rate", "Pruning rate");
    printConfig("dopamine_baseline", "Dopamine baseline");
    printConfig("environment_name", "Environment name");
    
    NLM_LOG_INFO("");
}

bool EnhancedConfigLoader::addConfigFile(const std::string& filename) {
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        NLM_LOG_ERROR("Config file not found: " + filename);
        return false;
    }
    
    // Add to loaded files list
    loadedFiles.push_back(filename);
    return true;
}

std::vector<std::string> EnhancedConfigLoader::getAllParameterNames() const {
    std::vector<std::string> paramNames;
    for (const auto& pair : parameters) {
        paramNames.push_back(pair.first);
    }
    return paramNames;
}

size_t EnhancedConfigLoader::getFileCount() const {
    return loadedFiles.size();
}

size_t EnhancedConfigLoader::getTotalParameters() const {
    return parameters.size();
}

template<typename T>
T EnhancedConfigLoader::getOr(const std::string& name, T defaultValue) const {
    // In a real implementation, this would parse parameters
    // For now, return default value
    return defaultValue;
}

template<typename T>
T EnhancedConfigLoader::convertStringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (iss >> value) {
        return value;
    }
    return T();
}

void EnhancedConfigLoader::addParameter(const std::string& name, const std::string& value) {
    parameters[name] = value;
}

bool EnhancedConfigLoader::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

std::string EnhancedConfigLoader::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return it->second;
    }
    return "";
}

} // namespace nlm