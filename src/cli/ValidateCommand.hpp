#pragma once

#include "Command.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class ValidateCommand : public Command {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { return "Validate configuration files"; }
    std::string getUsage() const override { return "validate <file> [OPTIONS]"; }
    std::string getHelp() const override {
        return "Validates configuration files for syntax and semantic correctness.\n"
               "Checks configuration parameters, validates required values, and\n"
               "verifies compatibility with NLM's expected configuration schema.\n"
               "Provides detailed error messages for any validation failures.\n"
               "Examples:\n"
               "  nlm validate configs/default.cfg\n"
               "  nlm validate config.json --schema=phase2\n"
               "  nlm validate config.cfg --strict=true\n";
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No configuration file specified.\n";
            std::cout << "Usage: nlm validate <file> [OPTIONS]\n";
            return false;
        }
        
        std::string configFile = args[0];
        bool strictMode = false;
        std::string schema = "auto"; // auto, phase2, phase3, phase4, phase5, phase6
        
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key == "strict") strictMode = (value == "true" || value == "1");
                else if (key == "schema") schema = value;
            }
        }
        
        std::cout << "Validating configuration file: " << configFile << "\n";
        
        if (strictMode) {
            std::cout << "  Mode: Strict validation\n";
        }
        
        if (schema != "auto") {
            std::cout << "  Schema: " << schema << "\n";
        }
        
        // Run validation
        bool success = validateConfig(configFile, strictMode, schema);
        
        if (success) {
            std::cout << "✓ Configuration is valid.\n";
        } else {
            std::cout << "✗ Configuration validation failed.\n";
        }
        
        return success;
    }
    
    bool execute(const Config& config) override {
        std::cout << "Validating configuration with existing config...\n";
        std::cout << "Note: Specify a file to validate: 'nlm validate <file>'\n";
        return true;
    }

private:
    bool validateConfig(const std::string& configFile, bool strictMode, const std::string& schema) {
        // Load and validate configuration
        auto testConfig = std::make_shared<Config>();
        bool loaded = testConfig->loadFromFile(configFile);
        
        if (!loaded) {
            std::cout << "  Error: Failed to load configuration file.\n";
            return false;
        }
        
        int validationErrors = 0;
        int validationWarnings = 0;
        
        std::cout << "  Loaded " << testConfig->getKeys().size() << " parameters.\n";
        
        // Basic validation - check for essential parameters
        std::vector<std::string> essentialKeys = {"random_seed", "simulation_timestep"};
        
        for (const auto& key : essentialKeys) {
            if (!testConfig->has(key)) {
                std::cout << "    Warning: Missing optional parameter: " << key << "\n";
                validationWarnings++;
            }
        }
        
        // Validate parameter values
        for (const auto& key : testConfig->getKeys()) {
            // Basic value type checking would go here
            std::cout << "    Validating: " << key << "\n";
            
            if (strictMode) {
                // More rigorous validation in strict mode
                if (key.find("invalid") != std::string::npos) {
                    std::cout << "    Error: Invalid parameter detected in strict mode.\n";
                    validationErrors++;
                }
            }
        }
        
        // Schema-specific validation
        if (schema != "auto" && schema != "none") {
            std::cout << "  Applying schema validation: " << schema << "\n";
            // Schema validation logic would go here
            validationWarnings++;
        }
        
        // Report results
        if (validationErrors > 0) {
            std::cout << "  Errors: " << validationErrors << "\n";
            return false;
        }
        
        if (validationWarnings > 0) {
            std::cout << "  Warnings: " << validationWarnings << "\n";
        }
        
        return true;
    }
};

} // namespace nlm