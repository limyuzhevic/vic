// Configuration Validation Tool
// Command-line utility for validating NLM configuration files

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>

// Usage information
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --file <path>    Configuration file to validate" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " --file configs/default.cfg" << std::endl;
}

// Validate configuration file
bool validateConfigFile(const std::string& filepath) {
    std::cout << "Validating configuration file: " << filepath << std::endl;
    
    // Initialize logger for validation tool
    auto logger = std::make_shared<Logger>();
    logger->setLevel(LogLevel::Info);
    
    nlm::Config config;
    
    // Load configuration from file
    if (!config.loadFromFile(filepath)) {
        std::cerr << "ERROR: Failed to load configuration file: " << filepath << std::endl;
        return false;
    }
    
    // Validate configuration
    config.validateConfiguration();
    
    // Display validation results
    std::cout << "\nValidation Results:" << std::endl;
    std::cout << "  File: " << filepath << std::endl;
    std::cout << "  Entries loaded: " << config.getKeys().size() << std::endl;
    
    // Show loaded configuration values
    std::cout << "\nLoaded Configuration:" << std::endl;
    for (const auto& key : config.getKeys()) {
        auto value = config.get<std::string>(key);
        if (value.has_value()) {
            std::cout << "  " << key << " = " << value.value() << std::endl;
        } else {
            std::cout << "  " << key << " = [unable to retrieve]" << std::endl;
        }
    }
    
    return true;
}

// Check consistency between configuration values
bool validateConfigConsistency(nlm::Config& config) {
    bool isValid = true;
    
    // Check neuron_count and region_count consistency
    auto neuronCount = config.get<int64_t>("neuron_count");
    auto regionCount = config.get<int64_t>("region_count");
    
    if (neuronCount.has_value() && regionCount.has_value()) {
        if (neuronCount.value() < regionCount.value() * 10) {
            std::cout << "WARNING: neuron_count (" << neuronCount.value() 
                     << ") is quite small for region_count (" << regionCount.value() << ")" << std::endl;
        }
    }
    
    // Check connection_probability is reasonable
    auto connProb = config.get<float>("connection_probability");
    if (connProb.has_value()) {
        if (connProb.value() < 0.01f || connProb.value() > 0.5f) {
            std::cout << "INFO: connection_probability (" << connProb.value() 
                     << ") is at extreme value - consider adjusting for better network connectivity" << std::endl;
        }
    }
    
    // Check timestep is reasonable
    auto timestep = config.get<double>("simulation_timestep");
    if (timestep.has_value()) {
        if (timestep.value() < 0.0001 || timestep.value() > 0.1) {
            std::cout << "INFO: simulation_timestep (" << timestep.value() 
                     << ") may affect simulation accuracy - consider 0.001 for typical simulations" << std::endl;
        }
    }
    
    // Check for required configuration parameters
    std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& param : requiredParams) {
        if (!config.has(param)) {
            std::cout << "WARNING: Required configuration parameter not found: " << param << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

// Show configuration summary
void showConfigSummary(nlm::Config& config) {
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << "====================" << std::endl;
    
    // Show key configuration values
    std::cout << "\nKey Configuration Values:" << std::endl;
    std::vector<std::string> keyParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "plasticity_learning_rate"
    };
    
    for (const auto& param : keyParams) {
        if (config.has(param)) {
            auto value = config.get<std::string>(param);
            if (value.has_value()) {
                std::cout << "  " << param << " = " << value.value() << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Set program name for usage message
    const char* programName = "nlm_config_validator";
    
    // Check for help flag
    if (argc == 2 && std::string(argv[1]) == "--help") {
        printUsage(programName);
        return 0;
    }
    
    // Parse command line arguments
    std::string configFile;
    bool hasConfigFile = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--file" && i + 1 < argc) {
            configFile = argv[++i];
            hasConfigFile = true;
        } else if (arg == "--help") {
            printUsage(programName);
            return 0;
        } else {
            std::cerr << "ERROR: Unknown argument: " << arg << std::endl;
            printUsage(programName);
            return 1;
        }
    }
    
    // Validate configuration file
    if (hasConfigFile) {
        if (!validateConfigFile(configFile)) {
            return 1;
        }
    } else {
        std::cerr << "ERROR: No configuration file specified" << std::endl;
        printUsage(programName);
        return 1;
    }
    
    std::cout << "\nConfiguration validation completed successfully!" << std::endl;
    return 0;
}