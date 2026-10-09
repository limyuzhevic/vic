// Config Command - Manage NLM configuration files

#pragma once

#include "CommandBase.hpp"
#include "CLIParser.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

class ConfigCommand : public CommandBase {
public:
    ConfigCommand() : CommandBase("config", "Manage configuration files and settings") {}
    
    int execute(std::shared_ptr<Config> config, const CLIParser& parser) override {
        std::cout << "NLM Configuration Manager" << std::endl;
        std::cout << "==========================" << std::endl;
        
        // Get positional arguments
        std::vector<std::string> args = parser.getPositionalArgs();
        std::string action = args.empty() ? "show" : args[0];
        
        if (action == "show" || action == "list" || args.empty()) {
            return showConfig(config, parser);
        } else if (action == "edit" || action == "set") {
            return editConfig(config, parser);
        } else if (action == "save") {
            return saveConfig(config, parser);
        } else if (action == "load") {
            return loadConfig(config, parser);
        } else if (action == "validate") {
            return validateConfig(config, parser);
        } else {
            printError("Unknown config action: " + action);
            printUsage("config", "[show|edit|save|load|validate] [options]");
            return 1;
        }
    }
    
private:
    int showConfig(std::shared_ptr<Config> config, const CLIParser& parser) {
        std::cout << "Current Configuration:" << std::endl;
        std::cout << "======================" << std::endl;
        
        // Print config summary
        std::string summary = config->summary();
        std::cout << summary;
        
        // Additional info
        bool verbose = parser.getBool("--verbose", "-v", false);
        if (verbose) {
            std::cout << std::endl;
            std::cout << "Source counts:" << std::endl;
            // Count by source type
            auto keys = config->getKeys();
            int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
            for (const auto& key : keys) {
                // This would require accessing ConfigEntry directly, simplified for now
                defaultCount++;
            }
            std::cout << "  Default:    " << defaultCount << std::endl;
            std::cout << "  File:       " << fileCount << std::endl;
            std::cout << "  Command Line: " << commandLineCount << std::endl;
            std::cout << "  Runtime:    " << runtimeCount << std::endl;
        }
        
        return 0;
    }
    
    int editConfig(std::shared_ptr<Config> config, const CLIParser& parser) {
        std::cout << "Configuration Editor (placeholder)" << std::endl;
        std::cout << "This would open a configuration editor in a real implementation." << std::endl;
        
        // Get key and value from command line
        std::string key = parser.getString("--key", "-k", "");
        std::string value = parser.getString("--value", "-v", "");
        
        if (key.empty()) {
            printError("Key is required for configuration editing");
            printUsage("config edit", "--key <name> --value <value>");
            return 1;
        }
        
        // Set configuration value
        config->set(key, value);
        std::cout << "Set " << key << " = " << value << std::endl;
        
        return 0;
    }
    
    int saveConfig(std::shared_ptr<Config> config, const CLIParser& parser) {
        std::string filepath = parser.getString("--output", "-o", "nlm_config.cfg");
        
        std::cout << "Saving configuration to: " << filepath << std::endl;
        
        // In a real implementation, this would save to JSON/YAML
        bool success = config->saveToFile(filepath);
        if (success) {
            std::cout << "Configuration saved successfully." << std::endl;
            return 0;
        } else {
            printError("Failed to save configuration to " + filepath);
            return 1;
        }
    }
    
    int loadConfig(std::shared_ptr<Config> config, const CLIParser& parser) {
        std::string filepath = parser.getString("--input", "-i", "");
        
        if (filepath.empty()) {
            printError("Input file is required for loading configuration");
            printUsage("config load", "--input <file>");
            return 1;
        }
        
        std::cout << "Loading configuration from: " << filepath << std::endl;
        
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            printError("Configuration file does not exist: " + filepath);
            return 1;
        }
        
        // In a real implementation, this would load from JSON/YAML
        bool success = config->loadFromFile(filepath);
        if (success) {
            std::cout << "Configuration loaded successfully." << std::endl;
            return 0;
        } else {
            printError("Failed to load configuration from " + filepath);
            return 1;
        }
    }
    
    int validateConfig(std::shared_ptr<Config> config, const CLIParser& parser) {
        std::cout << "Validating Configuration" << std::endl;
        std::cout << "==========================" << std::endl;
        
        // Get config summary for validation
        std::string summary = config->summary();
        
        // Basic validation
        bool valid = true;
        std::string errors;
        
        // Check for required keys
        std::vector<std::string> requiredKeys = {
            "neuron_count",
            "region_count",
            "simulation_timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                valid = false;
                errors += "Missing required key: " + key + "\n";
            }
        }
        
        // Check value ranges
        auto neuronCount = config->get<int64_t>("neuron_count");
        if (neuronCount && (*neuronCount < 10 || *neuronCount > 1000000)) {
            valid = false;
            errors += "neuron_count must be between 10 and 1,000,000\n";
        }
        
        auto timestep = config->get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            valid = false;
            errors += "simulation_timestep must be between 0 and 1.0\n";
        }
        
        if (valid) {
            std::cout << "✓ Configuration is valid." << std::endl;
            return 0;
        } else {
            std::cout << "✗ Configuration validation failed:" << std::endl;
            std::cout << errors;
            return 1;
        }
    }
};
