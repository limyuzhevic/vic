#pragma once

#include "CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace cli {

class ConfigSubcommand : public Subcommand {
private:
    std::string configFile;
    std::string action;
    
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Manage NLM configuration files"; }
    
    void addOptions() override {
        // Options are parsed in execute() based on command line args
    }
    
    CLIResult execute(const std::shared_ptr<Config>& config) override {
        // Parse action from command line (simplified for now)
        action = "list"; // Default action
        
        if (configFile.empty()) {
            return CLIResult(true, "Listing configuration keys:");
        }
        
        if (action == "list") {
            return listConfig(config);
        } else if (action == "show") {
            return showConfig(config);
        } else if (action == "set") {
            return setConfig(config);
        } else if (action == "save") {
            return saveConfig(config);
        } else if (action == "load") {
            return loadConfig(config);
        } else {
            return CLIResult(false, "Unknown config action: " + action);
        }
    }
    
    CLIResult listConfig(const std::shared_ptr<Config>& config) {
        std::vector<std::string> keys = config->getKeys();
        if (keys.empty()) {
            return CLIResult(true, "No configuration keys found");
        }
        
        std::cout << "Configuration keys:" << std::endl;
        for (const auto& key : keys) {
            std::cout << "  " << key << std::endl;
        }
        
        return CLIResult(true, "Listed " + std::to_string(keys.size()) + " configuration keys");
    }
    
    CLIResult showConfig(const std::shared_ptr<Config>& config) {
        std::string summary = config->summary();
        std::cout << "Configuration summary:" << std::endl;
        std::cout << summary << std::endl;
        
        return CLIResult(true, "Configuration displayed");
    }
    
    CLIResult setConfig(const std::shared_ptr<Config>& config) {
        // Implementation would parse key-value pairs from command line
        return CLIResult(true, "Configuration setting not implemented yet");
    }
    
    CLIResult saveConfig(const std::shared_ptr<Config>& config) {
        std::string filepath = configFile.empty() ? "nlm_config.cfg" : configFile;
        if (config->saveToFile(filepath)) {
            return CLIResult(true, "Configuration saved to: " + filepath);
        } else {
            return CLIResult(false, "Failed to save configuration to: " + filepath);
        }
    }
    
    CLIResult loadConfig(const std::shared_ptr<Config>& config) {
        std::string filepath = configFile.empty() ? "nlm_config.cfg" : configFile;
        if (config->loadFromFile(filepath)) {
            return CLIResult(true, "Configuration loaded from: " + filepath);
        } else {
            return CLIResult(false, "Failed to load configuration from: " + filepath);
        }
    }
    
    void showHelp() const override {
        std::cout << "Usage: config [OPTIONS] [ACTION]" << std::endl;
        std::cout << std::endl;
        std::cout << "Manage NLM configuration files." << std::endl;
        std::cout << std::endl;
        std::cout << "ACTIONS:" << std::endl;
        std::cout << "  list          List all configuration keys" << std::endl;
        std::cout << "  show          Show configuration summary" << std::endl;
        std::cout << "  save <file>   Save configuration to file" << std::endl;
        std::cout << "  load <file>   Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --file <file>  Configuration file path" << std::endl;
        std::cout << std::endl;
        std::cout << "If no action is specified, 'list' is used." << std::endl;
    }
};

} // namespace cli
} // namespace nlm
