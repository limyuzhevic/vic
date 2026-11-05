// NLM Configuration Validation
// Phase 6: Improved Configuration Management

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * Configuration validation and file I/O improvements
 */
class ConfigManager {
public:
    struct ConfigValidationResult {
        bool valid;
        std::string errorMessage;
        std::vector<std::string> warnings;
    };
    
    struct ParsedConfig {
        std::map<std::string, std::string> stringValues;
        std::map<std::string, double> doubleValues;
        std::map<std::string, int64_t> intValues;
        std::map<std::string, float> floatValues;
        std::map<std::string, bool> boolValues;
        
        // Validate required parameters
        bool validateRequiredParams(const std::vector<std::string>& requiredParams) const {
            for (const auto& param : requiredParams) {
                if (stringValues.find(param) == stringValues.end() &&
                    doubleValues.find(param) == doubleValues.end() &&
                    intValues.find(param) == intValues.end() &&
                    floatValues.find(param) == floatValues.end() &&
                    boolValues.find(param) == boolValues.end()) {
                    return false;
                }
            }
            return true;
        }
        
        // Get value with type checking
        template<typename T>
        T getValue(const std::string& key, const T& defaultValue) const {
            // Implementation would handle type-specific retrieval
            return defaultValue;
        }
        
        // Set value with type conversion
        template<typename T>
        void setValue(const std::string& key, const T& value) {
            // Implementation would handle type-specific storage
        }
    };
    
    static ConfigValidationResult validateConfig(const ParsedConfig& config) {
        ConfigValidationResult result;
        result.valid = true;
        
        // Validate neuron count
        if (config.intValues.find("neuron_count") == config.intValues.end()) {
            result.warnings.push_back("neuron_count not specified, using default (500)");
            // Set default
        } else {
            int64_t neuronCount = config.intValues.at("neuron_count");
            if (neuronCount < 10 || neuronCount > 1000000) {
                result.valid = false;
                result.errorMessage = "neuron_count must be between 10 and 1,000,000";
            }
        }
        
        // Validate region count
        if (config.intValues.find("region_count") == config.intValues.end()) {
            result.warnings.push_back("region_count not specified, using default (1)");
        } else {
            int64_t regionCount = config.intValues.at("region_count");
            if (regionCount < 1 || regionCount > 1000) {
                result.valid = false;
                result.errorMessage = "region_count must be between 1 and 1000";
            }
        }
        
        // Validate connection probability
        if (config.floatValues.find("connection_probability") == config.floatValues.end()) {
            result.warnings.push_back("connection_probability not specified, using default (0.1)");
        } else {
            float connProb = config.floatValues.at("connection_probability");
            if (connProb < 0.0f || connProb > 1.0f) {
                result.valid = false;
                result.errorMessage = "connection_probability must be between 0.0 and 1.0";
            }
        }
        
        // Validate simulation timestep
        if (config.doubleValues.find("simulation_timestep") == config.doubleValues.end()) {
            result.warnings.push_back("simulation_timestep not specified, using default (0.001)");
        } else {
            double timestep = config.doubleValues.at("simulation_timestep");
            if (timestep <= 0.0 || timestep > 1.0) {
                result.valid = false;
                result.errorMessage = "simulation_timestep must be between 0.0 and 1.0";
            }
        }
        
        // Validate random seed
        if (config.intValues.find("random_seed") != config.intValues.end()) {
            int64_t seed = config.intValues.at("random_seed");
            if (seed < 0 || seed > UINT64_MAX) {
                result.valid = false;
                result.errorMessage = "random_seed must be a valid unsigned 64-bit integer";
            }
        }
        
        // Validate STDP parameters
        if (config.floatValues.find("stdp_ltp_weight") != config.floatValues.end()) {
            float ltp = config.floatValues.at("stdp_ltp_weight");
            if (ltp < 0.0f) {
                result.valid = false;
                result.errorMessage = "stdp_ltp_weight must be positive";
            }
        }
        
        if (config.floatValues.find("stdp_ltd_weight") != config.floatValues.end()) {
            float ltd = config.floatValues.at("stdp_ltd_weight");
            if (ltd < 0.0f) {
                result.valid = false;
                result.errorMessage = "stdp_ltd_weight must be positive";
            }
        }
        
        // Validate structural plasticity parameters
        if (config.floatValues.find("synaptogenesis_rate") != config.floatValues.end()) {
            float synRate = config.floatValues.at("synaptogenesis_rate");
            if (synRate < 0.0f || synRate > 0.01f) {
                result.warnings.push_back("synaptogenesis_rate outside typical range (0.0 - 0.01)");
            }
        }
        
        if (config.floatValues.find("pruning_rate") != config.floatValues.end()) {
            float pruneRate = config.floatValues.at("pruning_rate");
            if (pruneRate < 0.0f || pruneRate > 0.0001f) {
                result.warnings.push_back("pruning_rate outside typical range (0.0 - 0.0001)");
            }
        }
        
        return result;
    }
    
    static bool parseConfigFile(const std::string& filename, ParsedConfig& config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key-value pairs
            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                continue; // Skip malformed lines
            }
            
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            
            // Store value based on type detection
            if (value.find('.') != std::string::npos) {
                // Try to parse as float/double
                try {
                    float floatVal = std::stof(value);
                    config.floatValues[key] = floatVal;
                    
                    // Also store as double
                    double doubleVal = std::stod(value);
                    config.doubleValues[key] = doubleVal;
                } catch (...) {
                    // Not a number
                    config.stringValues[key] = value;
                }
            } else if (value == "true" || value == "false") {
                // Boolean
                config.boolValues[key] = (value == "true");
            } else {
                // Try integer
                try {
                    int64_t intVal = std::stoll(value);
                    config.intValues[key] = intVal;
                } catch (...) {
                    // Not an integer
                    config.stringValues[key] = value;
                }
            }
        }
        
        file.close();
        return true;
    }
    
    static bool saveConfig(const ParsedConfig& config, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM CLI" << std::endl;
        file << std::endl;
        
        // Save all parameters
        for (const auto& pair : config.stringValues) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        for (const auto& pair : config.intValues) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        for (const auto& pair : config.floatValues) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        for (const auto& pair : config.doubleValues) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        for (const auto& pair : config.boolValues) {
            file << pair.first << " = " << (pair.second ? "true" : "false") << std::endl;
        }
        
        file.close();
        return true;
    }
};

// Global configuration manager instance
ConfigManager configManager;

// Enhanced configuration loading with validation
bool loadConfigWithValidation(const std::string& filename, std::shared_ptr<Config> config) {
    ConfigManager::ParsedConfig parsed;
    
    if (!ConfigManager::parseConfigFile(filename, parsed)) {
        std::cerr << "Warning: Could not parse config file: " << filename << std::endl;
        return false; // Continue with defaults
    }
    
    ConfigManager::ConfigValidationResult validation = ConfigManager::validateConfig(parsed);
    
    if (!validation.valid) {
        std::cerr << "Config file validation error: " << validation.errorMessage << std::endl;
        for (const auto& warning : validation.warnings) {
            std::cerr << "Warning: " << warning << std::endl;
        }
        return false;
    }
    
    for (const auto& warning : validation.warnings) {
        std::cerr << "Warning: " << warning << std::endl;
    }
    
    // Apply parsed values to config object
    // This would involve converting ParsedConfig values to Config type
    
    return true;
}

} // namespace nlm
