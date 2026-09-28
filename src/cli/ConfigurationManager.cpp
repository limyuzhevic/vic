// src/cli/ConfigurationManager.cpp
#include "ConfigurationManager.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <set>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct ConfigurationManager::Impl {
    std::set<std::string> supportedExtensions;
    std::map<std::string, std::string> templates;
    
    Impl() {
        // Supported file extensions
        supportedExtensions = {".json", ".yaml", ".yml"};
        
        // Basic configuration templates
        templates["basic"] = R"({
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1f,
    "random_seed": 42,
    "simulation_timestep": 0.001f,
    "stdp_ltp_weight": 0.01f,
    "stdp_ltd_weight": 0.012f,
    "stdp_tau": 20.0f,
    "synaptogenesis_rate": 0.0001f,
    "pruning_rate": 0.00001f
})";
        
        templates["phase3"] = R"({
    "environment_width": 20,
    "environment_height": 20,
    "vision_width": 16,
    "vision_height": 16,
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1f,
    "random_seed": 42,
    "simulation_timestep": 0.001f,
    "stdp_ltp_weight": 0.01f,
    "stdp_ltd_weight": 0.012f,
    "stdp_tau": 20.0f,
    "synaptogenesis_rate": 0.0001f,
    "pruning_rate": 0.00001f,
    "enable_reward_modulation": true,
    "enable_structural_plasticity": true,
    "enable_development": true,
    "enable_curiosity": true
})";
        
        templates["phase6"] = R"({
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1f,
    "random_seed": 42,
    "simulation_timestep": 0.001f,
    "stdp_ltp_weight": 0.01f,
    "stdp_ltd_weight": 0.012f,
    "stdp_tau": 20.0f,
    "synaptogenesis_rate": 0.0001f,
    "pruning_rate": 0.00001f,
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "checkpoint_dir": "./checkpoints",
    "checkpoint_compression": "balanced"
})";
    }
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {
}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadConfiguration(const std::string& filePath, std::shared_ptr<Config> config) {
    std::filesystem::path path(filePath);
    
    if (!std::filesystem::exists(path)) {
        NLM_LOG_ERROR("Configuration file not found: " + filePath);
        return false;
    }
    
    if (!isConfigurationFile(filePath)) {
        NLM_LOG_ERROR("File is not a valid configuration file: " + filePath);
        return false;
    }
    
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == ".json") {
            return loadJSONConfiguration(filePath, config);
        } else if (extension == ".yaml" || extension == ".yml") {
            return loadYAMLConfiguration(filePath, config);
        } else {
            NLM_LOG_ERROR("Unsupported file format: " + extension);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load configuration: ") + e.what());
        return false;
    }
}

bool ConfigurationManager::saveConfiguration(std::shared_ptr<Config> config, const std::string& filePath, 
                                            const std::string& format) {
    std::filesystem::path path(filePath);
    std::filesystem::path parentDir = path.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        if (!std::filesystem::create_directories(parentDir)) {
            NLM_LOG_ERROR("Failed to create directory: " + parentDir.string());
            return false;
        }
    }
    
    std::string extension = format;
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == "json") {
            return saveJSONConfiguration(config, filePath);
        } else if (extension == "yaml" || extension == "yml") {
            return saveYAMLConfiguration(config, filePath);
        } else {
            NLM_LOG_ERROR("Unsupported format: " + format);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save configuration: ") + e.what());
        return false;
    }
}

bool ConfigurationManager::validateConfiguration(const std::string& filePath) {
    std::filesystem::path path(filePath);
    
    if (!std::filesystem::exists(path)) {
        NLM_LOG_ERROR("Configuration file not found: " + filePath);
        return false;
    }
    
    try {
        std::shared_ptr<Config> config = std::make_shared<Config>();
        
        if (!loadConfiguration(filePath, config)) {
            return false;
        }
        
        return validateConfigObject(config);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Configuration validation failed: ") + e.what());
        return false;
    }
}

std::vector<std::string> ConfigurationManager::getSupportedExtensions() const {
    return std::vector<std::string>(pImpl->supportedExtensions.begin(), 
                                   pImpl->supportedExtensions.end());
}

std::map<std::string, std::string> ConfigurationManager::getConfigurationTemplates() const {
    return pImpl->templates;
}

std::vector<std::string> ConfigurationManager::compareConfigurations(const std::string& filePath1, 
                                                                    const std::string& filePath2) {
    std::vector<std::string> differences;
    
    try {
        std::shared_ptr<Config> config1 = std::make_shared<Config>();
        std::shared_ptr<Config> config2 = std::make_shared<Config>();
        
        if (!loadConfiguration(filePath1, config1) || 
            !loadConfiguration(filePath2, config2)) {
            differences.push_back("Failed to load one or both configuration files");
            return differences;
        }
        
        // Get all keys from both configurations
        std::set<std::string> allKeys;
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        allKeys.insert(keys1.begin(), keys1.end());
        allKeys.insert(keys2.begin(), keys2.end());
        
        for (const auto& key : allKeys) {
            bool hasKey1 = config1->has(key);
            bool hasKey2 = config2->has(key);
            
            if (hasKey1 != hasKey2) {
                if (hasKey1) {
                    differences.push_back("Key present in config1 only: " + key);
                } else {
                    differences.push_back("Key present in config2 only: " + key);
                }
            } else if (hasKey1 && hasKey2) {
                // Get values as strings for comparison
                std::string value1 = config1->get<std::string>(key);
                std::string value2 = config2->get<std::string>(key);
                
                if (value1 != value2) {
                    differences.push_back("Value mismatch for key " + key + ": " +
                                        value1 + " vs " + value2);
                }
            }
        }
        
    } catch (const std::exception& e) {
        differences.push_back(std::string("Comparison failed: ") + e.what());
    }
    
    return differences;
}

std::string ConfigurationManager::getConfigurationSummary(std::shared_ptr<Config> config) const {
    std::stringstream summary;
    summary << "NLM Configuration Summary" << std::endl;
    summary << "=======================" << std::endl;
    
    if (!config) {
        summary << "No configuration loaded" << std::endl;
        return summary.str();
    }
    
    auto keys = config->getKeys();
    summary << "Keys: " << keys.size() << std::endl;
    
    for (const auto& key : keys) {
        summary << "  " << key << std::endl;
    }
    
    return summary.str();
}

bool ConfigurationManager::isConfigurationFile(const std::string& filePath) const {
    std::filesystem::path path(filePath);
    std::string extension = path.extension().string();
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return pImpl->supportedExtensions.find(extension) != pImpl->supportedExtensions.end();
}

// Helper functions for JSON/YAML operations

bool ConfigurationManager::loadJSONConfiguration(const std::string& filePath, std::shared_ptr<Config> config) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filePath);
        return false;
    }
    
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR(std::string("JSON parse error: ") + e.what());
        return false;
    }
    
    // Load configuration from JSON
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        std::string value = it.value().dump();
        
        // Parse different types (basic implementation)
        if (it.value().is_number_integer()) {
            int64_t intValue = it.value();
            config->set(key, intValue, ConfigSource::File);
        } else if (it.value().is_number_float()) {
            float floatValue = it.value();
            config->set(key, floatValue, ConfigSource::File);
        } else if (it.value().is_boolean()) {
            bool boolValue = it.value();
            config->set(key, boolValue, ConfigSource::File);
        } else if (it.value().is_string()) {
            config->set(key, it.value(), ConfigSource::File);
        } else {
            NLM_LOG_WARNING(std::string("Skipping unsupported type for key: ") + key);
        }
    }
    
    return true;
}

bool ConfigurationManager::saveJSONConfiguration(std::shared_ptr<Config> config, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open output file: " + filePath);
        return false;
    }
    
    nlohmann::json jsonData;
    
    // Convert configuration to JSON
    if (config) {
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (config->has(key)) {
                // Note: This is a simplified implementation
                // A full implementation would need to get the actual type
                std::string value = config->get<std::string>(key);
                jsonData[key] = value;
            }
        }
    }
    
    try {
        file << jsonData.dump(4); // Pretty print with 4-space indentation
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to write JSON: ") + e.what());
        return false;
    }
    
    return true;
}

bool ConfigurationManager::loadYAMLConfiguration(const std::string& filePath, std::shared_ptr<Config> config) {
    YAML::Node yamlData;
    try {
        yamlData = YAML::LoadFile(filePath);
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR(std::string("YAML parse error: ") + e.what());
        return false;
    }
    
    // Load configuration from YAML
    for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
        std::string key = it->first.as<std::string>();
        
        try {
            if (it->IsScalar()) {
                std::string value = it->as<std::string>();
                config->set(key, value, ConfigSource::File);
            } else if (it->IsSequence()) {
                // Handle sequence types
                NLM_LOG_WARNING("Sequence type for key " + key + " not yet implemented");
            } else if (it->IsMap()) {
                // Handle map types
                NLM_LOG_WARNING("Map type for key " + key + " not yet implemented");
            } else {
                NLM_LOG_WARNING(std::string("Unsupported YAML type for key: ") + key);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error processing key " + key + ": ") + e.what());
        }
    }
    
    return true;
}

bool ConfigurationManager::saveYAMLConfiguration(std::shared_ptr<Config> config, const std::string& filePath) {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    if (config) {
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (config->has(key)) {
                try {
                    std::string value = config->get<std::string>(key);
                    emitter << YAML::Key << key << YAML::Value << value;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Skipping key " + key + ": ") + e.what());
                }
            }
        }
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open output file: " + filePath);
        return false;
    }
    
    file << emitter.c_str();
    return true;
}

bool ConfigurationManager::validateConfigObject(std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    // Basic validation
    auto keys = config->getKeys();n    
    // Check for required keys
    std::set<std::string> requiredKeys = {"neuron_count", "region_count"};
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + key);
            return false;
        }
    }
    
    // Validate value ranges
    try {
        if (config->has("neuron_count")) {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                NLM_LOG_ERROR("neuron_count must be positive, got: " + std::to_string(neuronCount));
                return false;
            }
        }
        
        if (config->has("region_count")) {
            int64_t regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                NLM_LOG_ERROR("region_count must be positive, got: " + std::to_string(regionCount));
                return false;
            }
        }
        
        if (config->has("connection_probability")) {
            float connectionProb = config->getOr<float>("connection_probability", 0.0f);
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                NLM_LOG_ERROR("connection_probability must be between 0 and 1, got: " + 
                            std::to_string(connectionProb));
                return false;
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Configuration validation error: ") + e.what());
        return false;
    }
    
    return true;
}

} // namespace nlm