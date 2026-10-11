// Configuration Management Utilities
// Provides command-line tools for configuration management

#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

namespace ConfigUtils {

// Show help for configuration commands
void showConfigHelp() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 NLM Configuration Management Tool             ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                              ║" << std::endl;
    std::cout << "║  Available Commands:                                        ║" << std::endl;
    std::cout << "║                                                              ║" << std::endl;
    std::cout << "║  --help                    Show this help message            ║" << std::endl;
    std::cout << "║  --version                 Show version information         ║" << std::endl;
    std::cout << "║  --validate-only           Validate configuration only      ║" << std::endl;
    std::cout << "║  --list-params             List all configuration parameters ║" << std::endl;
    std::cout << "║  --show-config             Display current configuration    ║" << std::endl;
    std::cout << "║  --create-default-config    Create default configuration file ║" << std::endl;
    std::cout << "║  --export-config=<file>     Export configuration to file    ║" << std::endl;
    std::cout << "║  --import-config=<file>     Import configuration from file  ║" << std::endl;
    std::cout << "║  --set-param=<key>=<value>  Set a configuration parameter   ║" << std::endl;
    std::cout << "║  --get-param=<key>          Get a configuration parameter value║" << std::endl;
    std::cout << "║  --remove-param=<key>       Remove a configuration parameter║" << std::endl;
    std::cout << "║  --validate-file=<file>     Validate configuration file   ║" << std::endl;
    std::cout << "║                                                              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
}

// Show version information
void showVersion() {
    std::cout << "NLM Configuration Management Tool v2.0.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with C++20 and extensive validation" << std::endl;
}

// List all configuration parameters with descriptions
void listConfigParameters() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                   Available Configuration Parameters           ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Core parameters
    std::cout << "║  Core Parameters:                                            " << std::endl;
    std::cout << "║    neuron_count              Number of neurons (int64)       " << std::endl;
    std.util << "    region_count              Number of brain regions (int64)" << std::endl;
    std::cout << "║    connection_probability    Connection probability (float)   " << std::endl;
    std::cout << "║    simulation_timestep       Time step size in seconds (double)" << std::endl;
    std::cout << "║    random_seed               Random seed (int64)             " << std::endl;
    std::cout << "║                                                              " << std::endl;
    
    // Plasticity parameters
    std::cout << "║  Plasticity Parameters:                                     " << std::endl;
    std::cout << "║    plasticity_learning_rate  Base learning rate (float)      " << std::endl;
    std::cout << "║    stdp_ltp_weight          STDP LTP weight (float)         " << std::endl;
    std::cout << "║    stdp_ltd_weight          STDP LTD weight (float)         " << std::endl;
    std::cout << "║    stdp_time_constant       STDP time constant (float)      " << std::endl;
    std::cout << "║    synaptogenesis_rate      New synapse formation (float)   " << std::endl;
    std::cout << "║    pruning_rate             Synapse elimination (float)     " << std::endl;
    std::cout << "║                                                              " << std::endl;
    
    // Neuromodulation parameters
    std::cout << "║  Neuromodulation Parameters:                                " << std::endl;
    std::cout << "║    dopamine_baseline         Baseline dopamine level (float) " << std::endl;
    std::cout << "║    novelty_threshold         Novelty detection threshold (float)" << std::endl;
    std::cout << "║    curiosity_strength        Curiosity motivation (float)     " << std::endl;
    std::cout << "║                                                              " << std::endl;
    
    // Logging parameters
    std::cout << "║  Logging Parameters:                                        " << std::endl;
    std::cout << "║    log_level                Logging level (string)          " << std::endl;
    std::cout << "║    log_to_file              Enable file logging (bool)      " << std::endl;
    std::cout << "║    log_filename             Log file name (string)          " << std::endl;
    std::cout << "║                                                              " << std::endl;
    
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
}

// Display current configuration
void showCurrentConfig(const Config& config) {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    Current Configuration Summary                ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    
    auto keys = config.getKeys();
    std::cout << "║  Parameters: " << keys.size() << " total" << std::endl;
    std::cout << "║                                                              " << std::endl;
    
    for (const auto& key : keys) {
        std::string valueStr;
        if (auto val = config.get<std::string>(key)) {
            valueStr = "\"" + *val + "\"";
        } else if (auto val = config.get<int64_t>(key)) {
            valueStr = std::to_string(*val);
        } else if (auto val = config.get<double>(key)) {
            valueStr = std::to_string(*val);
        } else if (auto val = config.get<int>(key)) {
            valueStr = std::to_string(*val);
        } else if (auto val = config.get<bool>(key)) {
            valueStr = (*val ? "true" : "false");
        } else {
            valueStr = "[unknown type]";
        }
        
        std::cout << "║  " << std::left << std::setw(25) << key << " = " << valueStr << std::endl;
    }
    
    std::cout << "║                                                              " << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
}

// Create default configuration file
bool createDefaultConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("novelty_threshold", 0.5f, ConfigSource::Default);
    config->set("curiosity_strength", 0.5f, ConfigSource::Default);
    config->set("log_level", std::string("INFO"), ConfigSource::Default);
    config->set("log_to_file", false, ConfigSource::Default);
    config->set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    
    if (config->saveToFile(filepath)) {
        std::cout << "Default configuration saved to: " << filepath << std::endl;
        return true;
    } else {
        std::cout << "Failed to save configuration to: " << filepath << std::endl;
        return false;
    }
}

// Validate configuration file
bool validateConfigFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cout << "Error: Failed to load configuration file: " << filepath << std::endl;
        return false;
    }
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    bool allValid = true;
    for (const auto& param : requiredParams) {
        if (!config->has(param)) {
            std::cout << "Warning: Missing required parameter: " << param << std::endl;
            allValid = false;
        }
    }
    
    if (allValid) {
        std::cout << "Configuration file is valid!" << std::endl;
        showCurrentConfig(*config);
    }
    
    return allValid;
}

// Main configuration management function
void processConfigCommands(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            showConfigHelp();
            return;
        } else if (arg == "--version") {
            showVersion();
            return;
        } else if (arg == "--list-params") {
            listConfigParameters();
            return;
        } else if (arg.find("--export-config=") == 0) {
            std::string filepath = arg.substr(15);
            auto config = std::make_shared<Config>();
            // In a real implementation, you would load the current config from somewhere
            if (config->saveToFile(filepath)) {
                std::cout << "Configuration exported to: " << filepath << std::endl;
            } else {
                std::cout << "Failed to export configuration to: " << filepath << std::endl;
            }
            return;
        } else if (arg.find("--create-default-config") == 0) {
            std::string filepath = (i + 1 < argc) ? argv[++i] : "default.cfg";
            createDefaultConfig(filepath);
            return;
        } else if (arg.find("--validate-file=") == 0) {
            std::string filepath = arg.substr(17);
            validateConfigFile(filepath);
            return;
        } else if (arg.find("--set-param=") == 0) {
            std::string param = arg.substr(12);
            size_t pos = param.find('=');
            if (pos != std::string::npos) {
                std::string key = param.substr(0, pos);
                std::string value = param.substr(pos + 1);
                auto config = std::make_shared<Config>();
                if (config->validateAndSet(key, value, ConfigSource::CommandLine)) {
                    std::cout << "Parameter set: " << key << " = " << value << std::endl;
                } else {
                    std::cout << "Failed to set parameter: " << key << " = " << value << std::endl;
                }
            } else {
                std::cout << "Error: Invalid parameter format for --set-param" << std::endl;
            }
            return;
        } else if (arg.find("--get-param=") == 0) {
            std::string key = arg.substr(12);
            auto config = std::make_shared<Config>();
            // In a real implementation, you would load the current config
            if (config->has(key)) {
                std::string valueStr;
                if (auto val = config->get<std::string>(key)) {
                    valueStr = *val;
                } else if (auto val = config->get<int64_t>(key)) {
                    valueStr = std::to_string(*val);
                } else if (auto val = config->get<double>(key)) {
                    valueStr = std::to_string(*val);
                } else if (auto val = config->get<int>(key)) {
                    valueStr = std::to_string(*val);
                } else if (auto val = config->get<bool>(key)) {
                    valueStr = (*val ? "true" : "false");
                }
                std::cout << key << " = " << valueStr << std::endl;
            } else {
                std::cout << "Parameter not found: " << key << std::endl;
            }
            return;
        }
    }
    
    // If no config-specific arguments found, show help
    if (argc <= 1) {
        showConfigHelp();
    }
}

} // namespace ConfigUtils

} // namespace nlm
