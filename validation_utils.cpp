// Comprehensive validation and error handling functions
// Provides validation for configuration and brain state

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <regex>

namespace nlm {

/**
 * Validates configuration for NLM operation
 * Checks for consistency and required parameters
 */
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    bool isValid = true;
    
    // Required configuration parameters
    std::vector<std::pair<std::string, std::function<bool(const Config&)>> validationChecks> {
        {"neuron_count", [](const Config& c) {
            auto val = c.getOr<size_t>("neuron_count", 0);
            return val > 0 && val <= 100000;
        }},
        {"region_count", [](const Config& c) {
            auto val = c.getOr<size_t>("region_count", 0);
            return val > 0 && val <= 1000;
        }},
        {"connection_probability", [](const Config& c) {
            auto val = c.getOr<float>("connection_probability", 0.0f);
            return val >= 0.0f && val <= 1.0f;
        }},
        {"simulation_timestep", [](const Config& c) {
            auto val = c.getOr<double>("simulation_timestep", 0.0);
            return val > 0.0 && val <= 0.1;
        }},
        {"random_seed", [](const Config& c) {
            auto val = c.getOr<uint64_t>("random_seed", 0);
            return val != 0; // Should be non-zero
        }},
        {"stdp_ltp_weight", [](const Config& c) {
            auto val = c.getOr<float>("stdp_ltp_weight", 0.0f);
            return val >= 0.0f && val <= 1.0f;
        }},
        {"stdp_ltd_weight", [](const Config& c) {
            auto val = c.getOr<float>("stdp_ltd_weight", 0.0f);
            return val >= 0.0f && val <= 1.0f;
        }},
        {"synaptogenesis_rate", [](const Config& c) {
            auto val = c.getOr<float>("synaptogenesis_rate", 0.0f);
            return val >= 0.0f && val <= 0.1f;
        }},
        {"pruning_rate", [](const Config& c) {
            auto val = c.getOr<float>("pruning_rate", 0.0f);
            return val >= 0.0f && val <= 0.01f;
        }},
    };
    
    std::cout << "Validating configuration..." << std::endl;
    
    for (const auto& [key, validator] : validationChecks) {
        if (validator(*config)) {
            std::cout << "  [OK] " << key << " = " << config->getOr<std::string>(key, "") << std::endl;
        } else {
            std::cout << "  [ERROR] Invalid " << key << " value" << std::endl;
            isValid = false;
        }
    }
    
    // Check inter-parameter dependencies
    auto neuronCount = config->getOr<size_t>("neuron_count", 0);
    auto regionCount = config->getOr<size_t>("region_count", 0);
    
    if (neuronCount > 0 && regionCount > 0) {
        auto neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion < 10) {
            std::cout << "  [WARNING] Very few neurons per region (" << neuronsPerRegion << ")" << std::endl;
        }
    }
    
    return isValid;
}

/**
 * Validates brain state for checkpointing
 */
bool validateBrainState(const std::shared_ptr<Brain>& brain) {
    std::cout << "Validating brain state..." << std::endl;
    
    bool isValid = true;
    
    // Check basic brain properties
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    size_t spikeCount = brain->getTotalSpikeCount();
    
    if (neuronCount == 0) {
        std::cout << "  [ERROR] Brain has no neurons" << std::endl;
        isValid = false;
    } else {
        std::cout << "  [OK] " << neuronCount << " neurons" << std::endl;
    }
    
    if (synapseCount == 0) {
        std::cout << "  [WARNING] Brain has no synapses" << std::endl;
    } else {
        std::cout << "  [OK] " << synapseCount << " synapses" << std::endl;
    }
    
    // Check for NaN or invalid values
    float firingRate = brain->getAverageFiringRate();
    if (std::isnan(firingRate) || firingRate < 0.0f) {
        std::cout << "  [ERROR] Invalid firing rate: " << firingRate << std::endl;
        isValid = false;
    } else {
        std::cout << "  [OK] Firing rate: " << firingRate << " Hz" << std::endl;
    }
    
    return isValid;
}

/**
 * Validates JSON configuration format
 */
bool validateJSONFormat(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        
        // Check if it's a valid JSON object
        if (!json.is_object()) {
            std::cout << "  [ERROR] JSON is not an object" << std::endl;
            return false;
        }
        
        // Validate required fields
        std::vector<std::string> requiredFields = {
            "neuron_count",
            "region_count",
            "connection_probability",
            "simulation_timestep",
            "random_seed"
        };
        
        for (const auto& field : requiredFields) {
            if (!json.contains(field)) {
                std::cout << "  [ERROR] Missing required field: " << field << std::endl;
                return false;
            }
        }
        
        // Check value types
        for (auto& [key, value] : json.items()) {
            if (key == "neuron_count" && !value.is_number_integer()) {
                std::cout << "  [ERROR] neuron_count must be an integer" << std::endl;
                return false;
            } else if (key == "region_count" && !value.is_number_integer()) {
                std::cout << "  [ERROR] region_count must be an integer" << std::endl;
                return false;
            } else if (key == "connection_probability" && !value.is_number()) {
                std::cout << "  [ERROR] connection_probability must be a number" << std::endl;
                return false;
            } else if (key == "simulation_timestep" && !value.is_number()) {
                std::cout << "  [ERROR] simulation_timestep must be a number" << std::endl;
                return false;
            } else if (key == "random_seed" && !value.is_number_integer()) {
                std::cout << "  [ERROR] random_seed must be an integer" << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "  [ERROR] Invalid JSON format: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Compares two configurations and reports differences
 */
void diffConfigurations(const std::shared_ptr<Config>& config1, 
                       const std::shared_ptr<Config>& config2) {
    std::cout << "Comparing configurations..." << std::endl;
    
    auto keys1 = config1->getKeys();
    auto keys2 = config2->getKeys();
    
    // Find common keys
    std::vector<std::string> commonKeys;
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
            commonKeys.push_back(key);
        }
    }
    
    // Find keys only in config1
    std::vector<std::string> onlyInConfig1;
    for (const auto& key : keys1) {
        if (std::find(commonKeys.begin(), commonKeys.end(), key) == commonKeys.end()) {
            onlyInConfig1.push_back(key);
        }
    }
    
    // Find keys only in config2
    std::vector<std::string> onlyInConfig2;
    for (const auto& key : keys2) {
        if (std::find(commonKeys.begin(), commonKeys.end(), key) == commonKeys.end()) {
            onlyInConfig2.push_back(key);
        }
    }
    
    if (!onlyInConfig1.empty()) {
        std::cout << "Keys only in first config:" << std::endl;
        for (const auto& key : onlyInConfig1) {
            std::cout << "  " << key << " = " << config1->getOr<std::string>(key, "") << std::endl;
        }
    }
    
    if (!onlyInConfig2.empty()) {
        std::cout << "Keys only in second config:" << std::endl;
        for (const auto& key : onlyInConfig2) {
            std::cout << "  " << key << " = " << config2->getOr<std::string>(key, "") << std::endl;
        }
    }
    
    if (commonKeys.empty()) {
        std::cout << "No common keys found" << std::endl;
        return;
    }
    
    std::cout << "Comparing common keys:" << std::endl;
    bool differencesFound = false;
    
    for (const auto& key : commonKeys) {
        auto value1 = config1->getOr<std::string>(key, "");
        auto value2 = config2->getOr<std::string>(key, "");
        
        if (value1 != value2) {
            std::cout << "  [DIFF] " << key << ": " << value1 << " != " << value2 << std::endl;
            differencesFound = true;
        }
    }
    
    if (!differencesFound) {
        std::cout << "No differences found in common keys" << std::endl;
    }
}

/**
 * Safe configuration merge that handles conflicts
 */
void safeMergeConfiguration(std::shared_ptr<Config>& config, const std::string& filepath) {
    std::cout << "Safely merging configuration from " << filepath << "..." << std::endl;
    
    // Create backup
    std::string backupFile = filepath + ".backup";
    if (config->saveToFile(backupFile)) {
        std::cout << "Backup created: " << backupFile << std::endl;
    } else {
        std::cout << "Failed to create backup, continuing anyway" << std::endl;
    }
    
    // Try to load new configuration
    if (config->loadFromFile(filepath)) {
        std::cout << "Configuration merged successfully" << std::endl;
        
        // Validate merged configuration
        if (!validateConfiguration(config)) {
            std::cout << "WARNING: Merged configuration failed validation!" << std::endl;
            
            // Restore from backup if validation fails
            if (config->loadFromFile(backupFile)) {
                std::cout << "Restored original configuration from backup" << std::endl;
            } else {
                std::cout << "Failed to restore from backup!" << std::endl;
            }
        }
    } else {
        std::cout << "Failed to merge configuration" << std::endl;
    }
}

} // namespace nlm
