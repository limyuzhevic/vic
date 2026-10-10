// NLM Advanced CLI - Configuration Command Implementation
#pragma once

#include "cli/main_cli.hpp"
#include "cli/command_factory.hpp"
#include "core/Config/Config.hpp"
#include "performance/CheckpointSystem.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

class ConfigCommand : public BaseCommand {
public:
    ConfigCommand() : BaseCommand("config", "Load and validate configuration files") {}
    
protected:
    CLIResult executeImpl(const std::map<std::string, std::string>& options) override {
        if (options.empty()) {
            displayHelpImpl();
            return CLIResult::HelpDisplayed;
        }
        
        std::string action = getOption(options, "action", "load");
        std::string file = getOption(options, "file", "");
        std::string format = getOption(options, "format", "auto");
        std::string output = getOption(options, "output", "");
        
        if (action == "load") {
            return handleLoadConfig(file, format, output);
        } else if (action == "save") {
            return handleSaveConfig(file, format);
        } else if (action == "validate") {
            return handleValidateConfig(file);
        } else if (action == "info") {
            return handleConfigInfo();
        } else if (action == "merge") {
            std::string source = getOption(options, "source", "");
            return handleMergeConfig(file, source);
        } else if (action == "defaults") {
            return handleConfigDefaults();
        } else {
            std::cout << "Unknown config action: " << action << std::endl;
            displayHelpImpl();
            return CLIResult::Error;
        }
    }
    
    void displayHelpImpl() const override {
        std::cout << "\n=== Config Command Help ===\n" << std::endl;
        std::cout << "Usage: nlm config [options]\n" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  --action=load --file=<path> [--format=json|yaml|auto] [--output=file]\n" << std::endl;
        std::cout << "  --action=save --file=<path> [--format=json|yaml]\n" << std::endl;
        std::cout << "  --action=validate --file=<path>\n" << std::endl;
        std::cout << "  --action=info\n" << std::endl;
        std::cout << "  --action=merge --file=<path> --source=<path>\n" << std::endl;
        std::cout << "  --action=defaults\n" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --format=<format>    Configuration file format (auto, json, yaml)\n" << std::endl;
        std::cout << "  --output=<file>      Output configuration file\n" << std::endl;
        std::cout << "  --source=<file>      Source configuration file for merging\n" << std::endl;
    }
    
private:
    CLIResult handleLoadConfig(const std::string& file, const std::string& format, const std::string& output) {
        std::string configFile = file;
        if (configFile.empty()) {
            configFile = CommandFactory::getState().config ? "" : "configs/config.json";
            if (configFile.empty()) {
                std::cout << "Error: No configuration file specified and no default config available.\n" << std::endl;
                return CLIResult::Error;
            }
        }
        
        std::cout << "Loading configuration from: " << configFile << std::endl;
        
        bool success = false;
        if (format == "json" || (format == "auto" && configFile.substr(configFile.find_last_of(".") + 1) == "json")) {
            success = globalState.config->loadFromFile(configFile);
        } else if (format == "yaml" || (format == "auto" && configFile.substr(configFile.find_last_of(".") + 1) == "yaml")) {
            success = globalState.config->loadFromFile(configFile);
        } else {
            success = globalState.config->loadFromFileAuto(configFile);
        }
        
        if (!success) {
            std::cout << "Error: Failed to load configuration from " << configFile << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Configuration loaded successfully.\n" << std::endl;
        std::cout << "Keys loaded: " << globalState.config->getKeys().size() << std::endl;
        
        if (!output.empty()) {
            if (globalState.config->saveToFile(output)) {
                std::cout << "Configuration saved to: " << output << std::endl;
            } else {
                std::cout << "Warning: Failed to save configuration to " << output << std::endl;
            }
        }
        
        CommandFactory::getState().config = globalState.config;
        return CLIResult::Success;
    }
    
    CLIResult handleSaveConfig(const std::string& file, const std::string& format) {
        std::string configFile = file;
        if (configFile.empty()) {
            configFile = "config.json";
        }
        
        bool isJSON = (format == "json" || format == "auto" || format.empty());
        if (globalState.config->saveToFile(configFile, isJSON)) {
            std::cout << "Configuration saved to: " << configFile << std::endl;
            return CLIResult::Success;
        } else {
            std::cout << "Error: Failed to save configuration to " << configFile << std::endl;
            return CLIResult::Error;
        }
    }
    
    CLIResult handleValidateConfig(const std::string& file) {
        if (!globalState.config) {
            std::cout << "Error: No configuration loaded for validation.\n" << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Validating configuration...\n" << std::endl;
        
        // Validate schema if available
        auto& schema = globalState.config->getSchema();
        auto allEntries = schema.getAllEntries();
        
        std::cout << "Schema validation:
" << std::endl;
        std::cout << "  Total entries in schema: " << allEntries.size() << std::endl;
        
        // Check for required entries
        size_t requiredCount = 0;
        for (const auto& entry : allEntries) {
            if (entry.required) {
                requiredCount++;
                if (!globalState.config->has(entry.name)) {
                    std::cout << "    Missing required entry: " << entry.name << std::endl;
                }
            }
        }
        
        if (requiredCount > 0) {
            std::cout << "  Required entries: " << requiredCount << std::endl;
        }
        
        std::cout << "  Loaded entries: " << globalState.config->getKeys().size() << std::endl;
        std::cout << "Validation completed.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleConfigInfo() {
        if (!globalState.config) {
            std::cout << "Error: No configuration loaded.\n" << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "=== Configuration Information ===\n" << std::endl;
        
        auto& schema = globalState.config->getSchema();
        std::cout << "Schema:
" << std::endl;
        std::cout << "  Categories: " << schema.getAllGroups().size() << std::endl;
        
        auto allEntries = schema.getAllEntries();
        std::cout << "  Total schema entries: " << allEntries.size() << std::endl;
        
        // Show loaded configuration summary
        std::cout << "\nLoaded Configuration Summary:
" << std::endl;
        std::cout << globalState.config->summary() << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleMergeConfig(const std::string& file, const std::string& source) {
        if (!globalState.config) {
            std::cout << "Error: No configuration loaded for merging.\n" << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Merging configuration from: " << source << "\n" << std::endl;
        
        auto sourceConfig = std::make_shared<Config>();
        if (!sourceConfig->loadFromFileAuto(source)) {
            std::cout << "Error: Failed to load source configuration from " << source << std::endl;
            return CLIResult::Error;
        }
        
        globalState.config->merge(*sourceConfig, ConfigSource::Inherited);
        std::cout << "Configuration merged successfully.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleConfigDefaults() {
        if (!globalState.config) {
            std::cout << "Error: No configuration loaded.\n" << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Applying default configuration values...\n" << std::endl;
        globalState.config->applyDefaults();
        std::cout << "Default configuration applied.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    // Helper methods
    std::string getOption(const std::map<std::string, std::string>& options, const std::string& key, const std::string& defaultValue) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
};

} // namespace nlm

