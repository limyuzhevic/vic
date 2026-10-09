#include "ConfigManager.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <numeric>
#include <limits>

namespace nlm {

ConfigManager::ConfigManager() 
    : monitoringEnabled_(false), lastValidationTime_(0.0), validationCount_(0), keyCount_(0) {
    // Register basic configuration schema
    registerSchema("random_seed", "integer", "Random seed for reproducible simulations", 
                  true, "42", "", 0, Constants::MAX_RANDOM_SEED);
    
    registerSchema("simulation_timestep", "double", "Time step for simulation (seconds)",
                  true, std::to_string(Constants::DEFAULT_SIMULATION_TIMESTEP), "",
                  0.0, 1.0);
    
    registerSchema("neuron_count", "integer", "Number of neurons in brain",
                  true, std::to_string(Constants::DEFAULT_NEURON_COUNT), "",
                  1, 100000);
    
    registerSchema("region_count", "integer", "Number of brain regions",
                  true, std::to_string(Constants::DEFAULT_REGION_COUNT), "",
                  1, 1000);
    
    registerSchema("connection_probability", "float", "Probability of synapse formation",
                  true, std::to_string(Constants::DEFAULT_CONNECTION_PROBABILITY), "",
                  0.0, 1.0);
    
    // STDP parameters
    registerSchema("stdp_ltp_weight", "float", "STDP long-term potentiation weight",
                  true, std::to_string(Constants::DEFAULT_STDP_LTP_WEIGHT), "",
                  -1.0, 1.0);
    
    registerSchema("stdp_ltd_weight", "float", "STDP long-term depression weight",
                  true, std::to_string(Constants::DEFAULT_STDP_LTD_WEIGHT), "",
                  -1.0, 1.0);
    
    registerSchema("stdp_tau", "float", "STDP time constant (ms)",
                  true, std::to_string(Constants::DEFAULT_STDP_TIME_CONST), "",
                  1.0, 1000.0);
    
    // Structural plasticity
    registerSchema("synaptogenesis_rate", "float", "Rate of new synapse formation",
                  true, std::to_string(Constants::DEFAULT_SYNAPTONGENESIS_RATE), "",
                  0.0, 1.0);
    
    registerSchema("pruning_rate", "float", "Rate of synapse removal",
                  true, std::to_string(Constants::DEFAULT_PRUNING_RATE), "",
                  0.0, 1.0);
    
    // Memory system parameters
    registerSchema("working_memory_size", "integer", "Size of working memory",
                  false, std::to_string(Constants::DEFAULT_WORKING_MEMORY_SIZE), "",
                  0, 10000);
    
    registerSchema("episodic_memory_capacity", "integer", "Capacity of episodic memory",
                  false, std::to_string(Constants::DEFAULT_EPISODIC_MEMORY_CAPACITY), "",
                  0, 100000);
    
    // Development parameters
    registerSchema("development_enabled", "bool", "Enable developmental processes",
                  false, "true", "", 0, 1);
    
    registerSchema("developmental_stage_duration", "double", "Duration of each developmental stage",
                  false, "10.0", "", 1.0, 1000.0);
    
    NLM_LOG_INFO("ConfigManager initialized with schema for " + std::to_string(schema_.size()) + " configuration keys");
}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadFromFile(const std::string& filename) {
    NLM_LOG_INFO("Loading configuration from file: " + filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw ConfigManagerError("Cannot open configuration file: " + filename);
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value format
        size_t separatorPos = line.find('=');
        if (separatorPos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Store configuration
        config_[key] = value;
        
        // Validate against schema if registered
        if (schema_.find(key) != schema_.end()) {
            validateValue(key, value);
        }
    }
    
    keyCount_ = config_.size();
    NLM_LOG_INFO("Loaded " + std::to_string(keyCount_) + " configuration keys from file");
}

void ConfigManager::loadFromArgs(int argc, char* argv[]) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 7) == "--config") {
            std::string configFile = "";
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            if (!configFile.empty()) {
                loadFromFile(configFile);
            }
        } else if (arg.substr(0, 6) == "--set=") {
            std::string configPair = arg.substr(6);
            size_t separatorPos = configPair.find('=');
            if (separatorPos != std::string::npos) {
                std::string key = configPair.substr(0, separatorPos);
                std::string value = configPair.substr(separatorPos + 1);
                config_[key] = value;
                NLM_LOG_INFO("Set configuration from command line: " + key + " = " + value);
            }
        }
    }
}

void ConfigManager::saveToFile(const std::string& filename) const {
    NLM_LOG_INFO("Saving configuration to file: " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw ConfigManagerError("Cannot write configuration file: " + filename);
    }
    
    file << Constants::CONFIG_FILE_HEADER;
    
    for (const auto& pair : config_) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
}

bool ConfigManager::validateConfig() const {
    auto errors = getValidationErrors();
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errors.size()) + " errors:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  - " + error);
        }
        return false;
    }
    
    lastValidationTime_ = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    validationCount_++;
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for required keys
    for (const auto& schemaEntry : schema_) {
        if (schemaEntry.second.required) {
            std::string key = schemaEntry.first;
            if (config_.find(key) == config_.end()) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
    }
    
    // Validate existing keys
    for (const auto& schemaEntry : schema_) {
        std::string key = schemaEntry.first;
        if (config_.find(key) != config_.end()) {
            try {
                validateValue(key, config_.at(key));
            } catch (const std::exception& e) {
                errors.push_back("Invalid value for key '" + key + "': " + std::string(e.what()));
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(config_.size());
    for (const auto& pair : config_) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool ConfigManager::hasKey(const std::string& key) const {
    return config_.find(key) != config_.end();
}

void ConfigManager::registerSchema(const std::string& key, const std::string& type,
                                   const std::string& description, bool required,
                                   const std::string& defaultValue,
                                   const std::string& allowedValuesStr,
                                   double minValue, double maxValue) {
    ConfigEntry entry;
    entry.key = key;
    entry.type = type;
    entry.description = description;
    entry.required = required;
    entry.defaultValue = defaultValue;
    entry.minValue = minValue;
    entry.maxValue = maxValue;
    entry.isValidated = false;
    
    // Parse allowed values
    if (!allowedValuesStr.empty()) {
        std::stringstream ss(allowedValuesStr);
        std::string value;
        while (std::getline(ss, value, ',')) {
            entry.allowedValues.push_back(value);
        }
    }
    
    schema_[key] = entry;
}

bool ConfigManager::validateKey(const std::string& key) const {
    return schema_.find(key) != schema_.end();
}

bool ConfigManager::validateValue(const std::string& key, const std::string& value) const {
    if (!validateKey(key)) {
        throw ConfigManagerError("Unknown configuration key: " + key);
    }
    
    const ConfigEntry& entry = schema_.at(key);
    
    // Convert string to appropriate type for validation
    double numericValue = 0.0;
    bool boolValue = false;
    
    try {
        if (entry.type == "integer") {
            size_t pos;
            long intValue = std::stol(value, &pos);
            if (pos != value.length()) {
                throw ConfigManagerError("Invalid integer value: " + value);
            }
            numericValue = static_cast<double>(intValue);
        } else if (entry.type == "float" || entry.type == "double") {
            size_t pos;
            double floatValue = std::stod(value, &pos);
            if (pos != value.length()) {
                throw ConfigManagerError("Invalid floating-point value: " + value);
            }
            numericValue = floatValue;
        } else if (entry.type == "bool") {
            if (value == "true" || value == "1" || value == "yes") {
                boolValue = true;
            } else if (value == "false" || value == "0" || value == "no") {
                boolValue = false;
            } else {
                throw ConfigManagerError("Invalid boolean value: " + value);
            }
        } else {
            // String type - no validation needed
        }
        
        // Range validation
        if (numericValue < entry.minValue || numericValue > entry.maxValue) {
            throw ConfigManagerError("Value " + value + " for key '" + key + 
                                   "' is out of range [" + std::to_string(entry.minValue) + 
                                   ", " + std::to_string(entry.maxValue) + "]");
        }
        
        // Allowed values validation
        if (!entry.allowedValues.empty()) {
            if (std::find(entry.allowedValues.begin(), entry.allowedValues.end(), value) == entry.allowedValues.end()) {
                throw ConfigManagerError("Value '" + value + "' for key '" + key + 
                                       "' is not in allowed values: " + 
                                       entry.allowedValues[0]);
                for (size_t i = 1; i < entry.allowedValues.size(); ++i) {
                    throw ConfigManagerError("Value '" + value + "' for key '" + key + 
                                           "' is not in allowed values: " + 
                                           entry.allowedValues[i]);
                }
            }
        }
        
    } catch (const ConfigManagerError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigManagerError(std::string(e.what()) + " for key '" + key + "'");
    }
    
    entry.isValidated = true;
    return true;
}

void ConfigManager::loadFromEnvironment(const std::string& prefix) {
    NLM_LOG_INFO("Loading configuration from environment variables with prefix: " + prefix);
    
    // This would typically use getenv() but for security we'll implement a placeholder
    // Environment variables would be loaded here
    NLM_LOG_INFO("Environment variable loading is not implemented yet");
}

void ConfigManager::setupDefaultBrainConfiguration() {
    setupPhase2Configuration();
    NLM_LOG_INFO("Default brain configuration set up (Phase 2)");
}

void ConfigManager::setupPhase2Configuration() {
    set("random_seed", "42");
    set("simulation_timestep", std::to_string(Constants::DEFAULT_SIMULATION_TIMESTEP));
    set("neuron_count", std::to_string(Constants::DEFAULT_NEURON_COUNT));
    set("region_count", std::to_string(Constants::DEFAULT_REGION_COUNT));
    set("connection_probability", std::to_string(Constants::DEFAULT_CONNECTION_PROBABILITY));
    set("stdp_ltp_weight", std::to_string(Constants::DEFAULT_STDP_LTP_WEIGHT));
    set("stdp_ltd_weight", std::to_string(Constants::DEFAULT_STDP_LTD_WEIGHT));
    set("stdp_tau", std::to_string(Constants::DEFAULT_STDP_TIME_CONST));
    set("synaptogenesis_rate", std::to_string(Constants::DEFAULT_SYNAPTONGENESIS_RATE));
    set("pruning_rate", std::to_string(Constants::DEFAULT_PRUNING_RATE));
    NLM_LOG_INFO("Phase 2 configuration set up");
}

void ConfigManager::setupPhase3Configuration() {
    setupPhase2Configuration();
    
    // Phase 3 specific settings
    set("world_width", "16");
    set("world_height", "16");
    set("enable_reward", "true");
    set("reward_scale", "1.0");
    set("enable_curiosity", "true");
    set("curiosity_level", std::to_string(Constants::DEFAULT_CURIOUSITY_LEVEL));
    
    NLM_LOG_INFO("Phase 3 configuration set up");
}

void ConfigManager::setupPhase4Configuration() {
    setupPhase3Configuration();
    
    // Phase 4 specific settings
    set("enable_planning", "true");
    set("planner_horizon", std::to_string(Constants::DEFAULT_PLANNER_HORIZON));
    set("planning_cost", std::to_string(Constants::DEFAULT_PLANNING_COST));
    set("enable_concept_formation", "true");
    set("concept_formation_threshold", std::to_string(Constants::DEFAULT_CONCEPT_FORMATION_THRESHOLD));
    
    NLM_LOG_INFO("Phase 4 configuration set up");
}

void ConfigManager::setupPhase5Configuration() {
    setupPhase4Configuration();
    
    // Phase 5 specific settings
    set("enable_performance_optimizations", "true");
    set("enable_checkpointing", "true");
    set("checkpoint_interval", std::to_string(Constants::CHECKPOINT_COMPRESSION_LEVEL));
    set("enable_memory_pools", "true");
    set("enable_simd_optimization", "true");
    
    NLM_LOG_INFO("Phase 5 configuration set up");
}

void ConfigManager::setupPhase6Configuration() {
    setupPhase5Configuration();
    
    // Phase 6 specific settings (Final Integration)
    set("enable_memory_integration", "true");
    set("enable_neuromodulation_integration", "true");
    set("enable_prediction_integration", "true");
    set("enable_development_integration", "true");
    set("enable_replay_consolidation", "true");
    set("enable_structural_plasticity", "true");
    
    NLM_LOG_INFO("Phase 6 configuration set up");
}

void ConfigManager::enableMonitoring(bool enable) {
    monitoringEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Configuration monitoring enabled");
    } else {
        NLM_LOG_INFO("Configuration monitoring disabled");
    }
}

void ConfigManager::fromJSON(const std::string& json) {
    NLM_LOG_INFO("Loading configuration from JSON format");
    
    // Placeholder for JSON parsing - would use a proper JSON library
    // For now, just log that it's not implemented
    NLM_LOG_WARNING("JSON configuration parsing is not implemented yet");
}

std::string ConfigManager::toJSON() const {
    // Placeholder for JSON serialization
    // For now, return a simple string representation
    std::stringstream json;
    json << "{\n";
    for (auto it = config_.begin(); it != config_.end(); ++it) {
        json << "  \"" << it->first << "\": \"" << it->second << "\"";
        if (std::next(it) != config_.end()) {
            json << ",";
        }
        json << "\n";
    }
    json << "}";
    return json.str();
}

double ConfigManager::getLastValidationTime() const {
    return lastValidationTime_;
}

size_t ConfigManager::getValidationCount() const {
    return validationCount_;
}

size_t ConfigManager::getKeyCount() const {
    return keyCount_;
}

double ConfigManager::getMemoryUsage() const {
    double memoryUsage = sizeof(ConfigManager) + 
                        sizeof(ConfigEntry) * schema_.size() + 
                        sizeof(std::pair<std::string, std::string>) * config_.size();
    return memoryUsage;
}

} // namespace nlm
