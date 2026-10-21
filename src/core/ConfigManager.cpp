#include "core/ConfigManager.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

ConfigManager::ConfigManager() : config(std::make_shared<Config>()), configFile("configs/default.cfg") {}

bool ConfigManager::loadConfig(const std::string& configFile, int argc, char** argv) {
    this->configFile = configFile;
    
    // Load configuration from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    setDefaultConfig();
    
    // Parse additional command line arguments
    parseCommandLineArgs(argc, argv);
    
    return true;
}

void ConfigManager::logConfigSummary() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
}

void ConfigManager::setDefaultConfig() {
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void ConfigManager::parseCommandLineArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config file format
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                std::string key = "config_file";
                std::string value = arg.substr(arg.find('=') + 1);
                config->set(key, value, ConfigSource::CommandLine);
            } else if (i + 1 < argc) {
                std::string key = "config_file";
                std::string value = argv[++i];
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        
        // Handle --neuron_count value format
        if (arg.substr(0, 13) == "--neuron_count") {
            if (arg.find('=') != std::string::npos) {
                std::string key = "neuron_count";
                std::string value = arg.substr(arg.find('=') + 1);
                try {
                    int64_t intValue = std::stoll(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid neuron_count value: " + value + " - " + e.what());
                }
            } else if (i + 1 < argc) {
                std::string key = "neuron_count";
                std::string value = argv[++i];
                try {
                    int64_t intValue = std::stoll(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid neuron_count value: " + value + " - " + e.what());
                }
            }
        }
        
        // Handle --region_count value format
        if (arg.substr(0, 12) == "--region_count") {
            if (arg.find('=') != std::string::npos) {
                std::string key = "region_count";
                std::string value = arg.substr(arg.find('=') + 1);
                try {
                    int64_t intValue = std::stoll(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid region_count value: " + value + " - " + e.what());
                }
            } else if (i + 1 < argc) {
                std::string key = "region_count";
                std::string value = argv[++i];
                try {
                    int64_t intValue = std::stoll(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid region_count value: " + value + " - " + e.what());
                }
            }
        }
    }
}

} // namespace nlm