// NLM (熙然) - Neural Learning Machine
// Configuration CLI - Advanced configuration management

#include "core/CLI/NLMCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace nlm;

class ConfigurationCLI {
public:
    ConfigurationCLI(const CLIConfigManager& config) : configManager(config) {}
    
    int run(CLIProgressReporter& reporter) {
        reporter.log("=== NLM Configuration CLI ===");
        reporter.log("Advanced configuration management...");
        
        std::string action = configManager.getConfig("action");
        std::string key = configManager.getConfig("key");
        std::string value = configManager.getConfig("value");
        std::string filename = configManager.getConfig("filename");
        
        // Load configuration manager
        std::string configFile = "configs/default.cfg";
        if (configManager.hasOption("--config")) {
            size_t pos = 0;
            if ((pos = configManager.getConfig("--config").find("--config")) != std::string::npos) {
                configFile = configManager.getConfig("--config").substr(pos + 8);
            }
        }
        
        // Load configuration
        auto config = std::make_shared<Config>();
        bool configLoaded = false;
        if (config->loadFromFile(configFile)) {
            reporter.log("Loaded configuration from: " + configFile);
            configLoaded = true;
        } else {
            reporter.log("Using default configuration (could not load: " + configFile + ")");
        }
        
        // Perform action
        if (action == "show") {
            return handleShowAction(config, reporter);
        } else if (action == "load") {
            return handleLoadAction(config, filename, reporter);
        } else if (action == "save") {
            return handleSaveAction(config, filename, reporter);
        } else if (action == "set") {
            return handleSetAction(config, key, value, reporter);
        } else if (action == "unset") {
            return handleUnsetAction(config, key, reporter);
        } else if (action == "list") {
            return handleListAction(config, reporter);
        } else if (action == "validate") {
            return handleValidateAction(config, reporter);
        } else if (action == "export") {
            return handleExportAction(config, filename, reporter);
        } else if (action == "import") {
            return handleImportAction(config, filename, reporter);
        } else if (action == "merge") {
            return handleMergeAction(config, filename, reporter);
        } else {
            reporter.error("Unknown action: " + action);
            return 1;
        }
    }
    
private:
    int handleShowAction(std::shared_ptr<Config> config, CLIProgressReporter& reporter) {
        reporter.log("");
        reporter.log("=== Current Configuration ===");
        
        // Show all configuration values
        config->logAll();
        
        // Show command-line overrides
        if (!configManager.getPositionalArgs().empty()) {
            reporter.log("");
            reporter.log("Command-line arguments:");
            for (const auto& arg : configManager.getPositionalArgs()) {
                if (arg.substr(0, 2) == "--") {
                    reporter.log("  " + arg);
                }
            }
        }
        
        return 0;
    }
    
    int handleLoadAction(std::shared_ptr<Config> config, const std::string& filename, CLIProgressReporter& reporter) {
        std::string fileToLoad = filename;
        if (fileToLoad.empty()) {
            // Try to get from config file option
            size_t pos = 0;
            if ((pos = configManager.getConfig("--config").find("--config")) != std::string::npos) {
                fileToLoad = configManager.getConfig("--config").substr(pos + 8);
            }
        }
        
        if (fileToLoad.empty()) {
            reporter.error("No configuration file specified for load action");
            return 1;
        }
        
        reporter.log("Loading configuration from: " + fileToLoad);
        
        if (config->loadFromFile(fileToLoad)) {
            reporter.log("Configuration loaded successfully");
            return 0;
        } else {
            reporter.error("Failed to load configuration from: " + fileToLoad);
            return 1;
        }
    }
    
    int handleSaveAction(std::shared_ptr<Config> config, const std::string& filename, CLIProgressReporter& reporter) {
        std::string fileToSave = filename;
        if (fileToSave.empty()) {
            // Use default config file name
            fileToSave = "configs/default.cfg";
        }
        
        reporter.log("Saving configuration to: " + fileToSave);
        
        if (config->saveToFile(fileToSave)) {
            reporter.log("Configuration saved successfully");
            return 0;
        } else {
            reporter.error("Failed to save configuration to: " + fileToSave);
            return 1;
        }
    }
    
    int handleSetAction(std::shared_ptr<Config> config, const std::string& key, const std::string& value, CLIProgressReporter& reporter) {
        if (key.empty()) {
            reporter.error("No key specified for set action");
            return 1;
        }
        
        if (value.empty()) {
            reporter.error("No value specified for set action");
            return 1;
        }
        
        reporter.log("Setting configuration: " + key + " = " + value);
        
        try {
            // Try to set as integer
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::CommandLine);
                reporter.log("Set as integer: " + std::to_string(intValue));
                return 0;
            } catch (...) {}
            
            // Try to set as float
            try {
                float floatValue = std::stof(value);
                config->set(key, floatValue, ConfigSource::CommandLine);
                reporter.log("Set as float: " + std::to_string(floatValue));
                return 0;
            } catch (...) {}
            
            // Try to set as string
            config->set(key, value, ConfigSource::CommandLine);
            reporter.log("Set as string: \"\" + value + "\"");
            return 0;
            
        } catch (const std::exception& e) {
            reporter.error("Failed to set configuration value: " + std::string(e.what()));
            return 1;
        }
    }
    
    int handleUnsetAction(std::shared_ptr<Config> config, const std::string& key, CLIProgressReporter& reporter) {
        if (key.empty()) {
            reporter.error("No key specified for unset action");
            return 1;
        }
        
        reporter.log("Removing configuration: " + key);
        
        // Reset to default
        config->resetToDefault(key);
        reporter.log("Configuration reset to default");
        
        return 0;
    }
    
    int handleListAction(std::shared_ptr<Config> config, CLIProgressReporter& reporter) {
        reporter.log("");
        reporter.log("=== Configuration Keys ===");
        
        // Get all configuration keys
        auto keys = config->getAllKeys();
        
        // Sort keys alphabetically
        std::vector<std::string> sortedKeys(keys.begin(), keys.end());
        std::sort(sortedKeys.begin(), sortedKeys.end());
        
        // Display keys with values
        for (const auto& key : sortedKeys) {
            std::string value;
            
            try {
                // Try to get as integer
                int64_t intValue = config->getOr<int64_t>(key, 0);
                value = std::to_string(intValue);
            } catch (...) {
                try {
                    // Try to get as float
                    float floatValue = config->getOr<float>(key, 0.0f);
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(6) << floatValue;
                    value = ss.str();
                } catch (...) {
                    // Try to get as string
                    try {
                        value = config->getOr<std::string>(key, "");
                    } catch (...) {
                        value = "<error>";
                    }
                }
            }
            
            reporter.log("  " + key + " = " + value);
        }
        
        reporter.log("Total keys: " + std::to_string(sortedKeys.size()));
        
        return 0;
    }
    
    int handleValidateAction(std::shared_ptr<Config> config, CLIProgressReporter& reporter) {
        reporter.log("");
        reporter.log("=== Configuration Validation ===");
        
        // Perform validation checks
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Check for required parameters
        if (!config->hasKey("random_seed")) {
            warnings.push_back("Missing random_seed - using default");
        }
        
        if (!config->hasKey("simulation_timestep")) {
            warnings.push_back("Missing simulation_timestep - using default");
        }
        
        if (!config->hasKey("neuron_count")) {
            warnings.push_back("Missing neuron_count - using default");
        }
        
        // Validate ranges
        if (config->hasKey("simulation_timestep")) {
            double timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            } else if (timestep > 1.0) {
                warnings.push_back("simulation_timestep is large (> 1s) - may cause instability");
            }
        }
        
        if (config->hasKey("neuron_count")) {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
            if (neuronCount <= 0) {
                errors.push_back("neuron_count must be positive");
            } else if (neuronCount > 100000) {
                warnings.push_back("neuron_count is very large - may cause performance issues");
            }
        }
        
        if (config->hasKey("region_count")) {
            int64_t regionCount = config->getOr<int64_t>("region_count", 1);
            if (regionCount <= 0) {
                errors.push_back("region_count must be positive");
            }
        }
        
        if (!errors.empty()) {
            reporter.log("ERRORS:");
            for (const auto& error : errors) {
                reporter.error("  " + error);
            }
        }
        
        if (!warnings.empty()) {
            reporter.log("");
            reporter.log("WARNINGS:");
            for (const auto& warning : warnings) {
                reporter.warning("  " + warning);
            }
        }
        
        if (errors.empty() && warnings.empty()) {
            reporter.log("✓ Configuration is valid");
            return 0;
        } else if (warnings.empty()) {
            reporter.log("✗ Configuration has errors and cannot be used");
            return 1;
        } else {
            reporter.log("⚠ Configuration has warnings but can be used");
            return 0;
        }
    }
    
    int handleExportAction(std::shared_ptr<Config> config, const std::string& filename, CLIProgressReporter& reporter) {
        std::string fileToExport = filename;
        if (fileToExport.empty()) {
            fileToExport = "configs/exported_config.cfg";
        }
        
        reporter.log("Exporting configuration to: " + fileToExport);
        
        std::ofstream file(fileToExport);
        if (!file.is_open()) {
            reporter.error("Failed to create export file: " + fileToExport);
            return 1;
        }
        
        file << "# NLM Configuration exported by CLI" << std::endl;
        file << "# Generated: " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        auto keys = config->getAllKeys();
        for (const auto& key : keys) {
            try {
                int64_t intValue = config->getOr<int64_t>(key, 0);
                file << key << " = " << intValue << std::endl;
            } catch (...) {
                try {
                    float floatValue = config->getOr<float>(key, 0.0f);
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(6) << floatValue;
                    file << key << " = " << ss.str() << std::endl;
                } catch (...) {
                    try {
                        std::string strValue = config->getOr<std::string>(key, "");
                        file << key << " = \"" << strValue << "\"" << std::endl;
                    } catch (...) {
                        file << key << " = <unknown>" << std::endl;
                    }
                }
            }
        }
        
        file.close();
        
        reporter.log("Configuration exported successfully");
        return 0;
    }
    
    int handleImportAction(std::shared_ptr<Config> config, const std::string& filename, CLIProgressReporter& reporter) {
        std::string fileToImport = filename;
        if (fileToImport.empty()) {
            reporter.error("No configuration file specified for import action");
            return 1;
        }
        
        reporter.log("Importing configuration from: " + fileToImport);
        
        std::ifstream file(fileToImport);
        if (!file.is_open()) {
            reporter.error("Failed to open import file: " + fileToImport);
            return 1;
        }
        
        std::string line;
        int lineNumber = 0;
        int importedCount = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            size_t end = line.find_last_not_of(" \t\r\n");
            if (start == std::string::npos || end == std::string::npos) continue;
            
            line = line.substr(start, end - start + 1);
            
            // Parse key=value
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                reporter.warning("Skipping invalid line " + std::to_string(lineNumber) + ": no '='");
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim key and value
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            
            // Remove quotes from string values
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set configuration value
            try {
                config->set(key, value, ConfigSource::File);
                importedCount++;
            } catch (...) {
                reporter.warning("Failed to set configuration key: " + key);
            }
        }
        
        file.close();
        
        reporter.log("Imported " + std::to_string(importedCount) + " configuration values");
        return 0;
    }
    
    int handleMergeAction(std::shared_ptr<Config> config, const std::string& filename, CLIProgressReporter& reporter) {
        std::string fileToMerge = filename;
        if (fileToMerge.empty()) {
            reporter.error("No configuration file specified for merge action");
            return 1;
        }
        
        reporter.log("Merging configuration from: " + fileToMerge);
        
        // Create temporary config to merge
        auto mergeConfig = std::make_shared<Config>();
        
        if (!mergeConfig->loadFromFile(fileToMerge)) {
            reporter.error("Failed to load merge file: " + fileToMerge);
            return 1;
        }
        
        // Merge configurations
        auto mergeKeys = mergeConfig->getAllKeys();
        for (const auto& key : mergeKeys) {
            try {
                int64_t intValue = mergeConfig->getOr<int64_t>(key, 0);
                config->set(key, intValue, ConfigSource::File);
            } catch (...) {
                try {
                    float floatValue = mergeConfig->getOr<float>(key, 0.0f);
                    config->set(key, floatValue, ConfigSource::File);
                } catch (...) {
                    try {
                        std::string strValue = mergeConfig->getOr<std::string>(key, "");
                        config->set(key, strValue, ConfigSource::File);
                    } catch (...) {}
                }
            }
        }
        
        reporter.log("Configuration merged successfully");
        return 0;
    }
    
private:
    CLIConfigManager& configManager;
};

int main(int argc, char** argv) {
    // Create and run configuration CLI
    NLMCLI cli(argc, argv, "nlm_config");
    
    if (cli.getConfigManager().hasOption("--help") || 
        cli.getConfigManager().getConfig("subcommand") == "help") {
        cli.printUsage();
        return 0;
    }
    
    if (cli.getConfigManager().hasOption("--version")) {
        std::cout << "NLM Configuration CLI" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Advanced configuration management" << std::endl;
        return 0;
    }
    
    // Run configuration command
    CLIProgressReporter reporter(std::cout, cli.getConfigManager().getBool("--quiet", false));
    ConfigurationCLI configCLI(cli.getConfigManager());
    
    return configCLI.run(reporter);
}