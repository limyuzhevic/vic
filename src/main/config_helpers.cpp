// Configuration Helper Functions
// Improved configuration loading and validation

#include "main/config_helpers.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

using namespace nlm;

// Improved Configuration loading function
bool loadConfiguration(std::shared_ptr<Config> config, const std::string& configFile) {
    // Clear any existing configuration
    if (!config) {
        NLM_LOG_ERROR("Invalid config pointer");
        return false;
    }
    
    config->clear();
    
    // Try to load from file if provided
    if (!configFile.empty()) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
            return true;
        } else {
            NLM_LOG_WARNING("Could not load configuration from: " + configFile + ", using defaults");
        }
    }
    
    return false;
}

// Helper function to validate brain initialization
bool validateBrainConfiguration(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Invalid config pointer for validation");
        return false;
    }
    
    const auto& neuronCount = config->getOr<int64_t>("neuron_count", 500);
    const auto& regionCount = config->getOr<int64_t>("region_count", 1);
    const auto& connectionProb = config->getOr<float>("connection_probability", 0.15f);
    
    NLM_LOG_INFO("Validating configuration...");
    NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  Regions: " + std::to_string(regionCount));
    NLM_LOG_INFO("  Connection probability: " + std::to_string(connectionProb));
    
    // Validate reasonable bounds
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
        return false;
    }
    
    if (regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount));
        return false;
    }
    
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProb));
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

// Enhanced argument parsing with better error handling
void parseCommandLineArguments(int argc, char** argv, std::string& configFile) {
    configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--config") == 0) {
            // Handle --config=file or --config file
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
                NLM_LOG_INFO("Using config file from command line: " + configFile);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
                NLM_LOG_INFO("Using config file from command line: " + configFile);
            }
        }
        // Additional command line options could be added here
    }
}

// Default configuration setup
void setupDefaultConfiguration(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Invalid config pointer for default configuration setup");
        return;
    }
    
    // Clear any existing configuration first
    config->clear();
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    NLM_LOG_INFO("Default configuration applied");
}
