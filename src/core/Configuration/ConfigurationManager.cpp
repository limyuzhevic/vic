#include "ConfigurationManager.hpp"
#include "Validation.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace nlm;

bool ConfigurationManager::loadConfiguration(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration...");
    
    config_ = std::make_shared<Config>();
    
    // Load from file if exists
    if (!configFilePath_.empty() && config_->loadFromFile(configFilePath_)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFilePath_);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Load from command line arguments (overrides file)
    config_->loadFromArgs(argc, argv);
    
    // Apply default values for Phase 2 (these are currently in main.cpp)
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration loaded successfully.");
    return true;
}

std::vector<std::string> ConfigurationManager::validateConfiguration() const {
    std::vector<std::string> errors;
    
    if (!config_) {
        errors.push_back("Configuration not loaded");
        return errors;
    }
    
    // Use the ConfigurationValidator to validate all configuration entries
    std::vector<ConfigEntry> entries;
    entries.reserve(config_->getKeys().size() + 10); // Reserve space for defaults
    
    // Collect existing entries
    for (const auto& key : config_->getKeys()) {
        auto value = config_->get<std::string>(key);
        if (value) {
            // Find the entry in the config
            for (const auto& entry : config_->getKeys()) {
                ConfigEntry e;
                e.key = entry;
                errors.push_back("TODO: Implement proper validation");
            }
        }
    }
    
    // Validate specific key values directly for now
    int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 100);
    auto neuronErrors = validator_.validate(ConfigEntry("neuron_count", neuronCount, ConfigSource::Runtime));
    errors.insert(errors.end(), neuronErrors.begin(), neuronErrors.end());
    
    int64_t regionCount = config_->getOr<int64_t>("region_count", 1);
    auto regionErrors = validator_.validate(ConfigEntry("region_count", regionCount, ConfigSource::Runtime));
    errors.insert(errors.end(), regionErrors.begin(), regionErrors.end());
    
    float connProb = config_->getOr<float>("connection_probability", 0.15f);
    auto connProbErrors = validator_.validate(ConfigEntry("connection_probability", connProb, ConfigSource::Runtime));
    errors.insert(errors.end(), connProbErrors.begin(), connProbErrors.end());
    
    double timestep = config_->getOr<double>("simulation_timestep", 0.001);
    auto timestepErrors = validator_.validate(ConfigEntry("simulation_timestep", timestep, ConfigSource::Runtime));
    errors.insert(errors.end(), timestepErrors.begin(), timestepErrors.end());
    
    return errors;
}

template<typename T>
T ConfigurationManager::getValue(const std::string& key, const T& defaultValue) const {
    if (!config_) {
        return defaultValue;
    }
    return config_->getOr<T>(key, defaultValue);
}

void ConfigurationManager::setValue(const std::string& key, const std::string& value) {
    if (config_) {
        config_->set(key, value, ConfigSource::Runtime);
    }
}

std::string ConfigurationManager::getConfigurationSummary() const {
    if (!config_) {
        return "Configuration not loaded";
    }
    return config_->summary();
}

bool ConfigurationManager::isComplete() const {
    if (!config_) return false;
    
    // Check essential configuration parameters
    std::vector<std::string> essentialKeys = {
        "neuron_count", "region_count", "simulation_timestep"
    };
    
    for (const auto& key : essentialKeys) {
        if (!config_->has(key)) return false;
    }
    
    return true;
}

std::string ConfigurationManager::getConfigFilePath() const {
    return configFilePath_;
}

void ConfigurationManager::loadDefaultConfiguration() {
    // Apply Phase 2 defaults
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
}

// Validation methods moved to Validation.cpp