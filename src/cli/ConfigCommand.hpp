#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

namespace nlm {

class ConfigCommand : public SimpleCLICommand {
public:
    ConfigCommand() : SimpleCLICommand(
        "config",
        "Configuration management and manipulation",
        "nlm config <command> [options]",
        "Configuration management and manipulation.\n\n"
        "Available config commands:\n"
        "  set     Set configuration values\n"
        "  get     Get configuration values\n"
        "  delete  Delete configuration values\n"
        "  list    List configuration keys\n"
        "  validate Validate configuration\n"
        "  import  Import configuration from file\n"
        "  export  Export configuration to file\n"
        "  merge   Merge configuration files\n"
        "  diff    Show configuration differences\n\n"
        "Options:\n"
        "  --file <file>        Configuration file (default: config.txt)\n"
        "  --format json|txt    Configuration format (default: txt)\n"
        "  --verbose            Enable verbose output\n"
        "  --backup             Create backup before operations\n"
        "  --strict             Strict validation\n\n"
        "Examples:\n"
        "  nlm config set neuron_count 1000\n"
        "  nlm config get simulation_timestep\n"
        "  nlm config list --verbose\n"
        "  nlm config import config.json --format json"
    ) {
        options = {
            {"file", "", "Configuration file", OptionType::OptionalValue, false, "config.txt"},
            {"format", "", "Configuration format", OptionType::Choice, false, "txt", "json|txt"},
            {"verbose", "", "Enable verbose output", OptionType::Flag},
            {"backup", "", "Create backup before operations", OptionType::Flag},
            {"strict", "", "Strict validation", OptionType::Flag},
            {"merge", "", "Merge configuration files", OptionType::Flag},
            {"diff", "", "Show configuration differences", OptionType::Flag}
        };
        
        parameters = {
            {"command", "Config command to execute", true, "string", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string configCommand = getRequiredParameter(args, 0);
        
        // Get options
        std::string configFile = getOptionValue(args, "file");
        std::string configFormat = getOptionValue(args, "format");
        bool verbose = hasOption(args, "verbose");
        bool backup = hasOption(args, "backup");
        bool strict = hasOption(args, "strict");
        bool merge = hasOption(args, "merge");
        bool diff = hasOption(args, "diff");
        
        // Get configuration
        auto config = getConfig(configFile);
        
        // Execute config command
        if (configCommand == "set") {
            return executeConfigSet(args, config, verbose, backup, strict);
        } else if (configCommand == "get") {
            return executeConfigGet(args, config, verbose);
        } else if (configCommand == "delete") {
            return executeConfigDelete(args, config, verbose, backup);
        } else if (configCommand == "list") {
            return executeConfigList(args, config, verbose, strict);
        } else if (configCommand == "validate") {
            return executeConfigValidate(args, config, verbose, strict);
        } else if (configCommand == "import") {
            return executeConfigImport(args, config, verbose, backup);
        } else if (configCommand == "export") {
            return executeConfigExport(args, config, verbose, backup);
        } else if (configCommand == "merge") {
            return executeConfigMerge(args, config, verbose, backup);
        } else if (configCommand == "diff") {
            return executeConfigDiff(args, config, verbose);
        } else {
            errorHandler.addError("Unknown config command: " + configCommand);
            return errorHandler.exitWithError();
        }
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        std::string configCommand = args[0];
        
        // Validate supported commands
        static const std::vector<std::string> supportedCommands = {
            "set", "get", "delete", "list", "validate", "import", "export", "merge", "diff"
        };
        
        if (std::find(supportedCommands.begin(), supportedCommands.end(), configCommand) == supportedCommands.end()) {
            return false;
        }
        
        // Validate file option
        for (const auto& arg : args) {
            if (arg.find("--file") != std::string::npos && arg != "--file") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    int executeConfigSet(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                         bool verbose, bool backup, bool strict) {
        // Parse key=value pairs from remaining arguments
        std::vector<std::string> remainingArgs;
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose" && args[i] != "--backup" && args[i] != "--strict") {
                remainingArgs.push_back(args[i]);
            }
        }
        
        if (remainingArgs.empty()) {
            CLIFormatters::printInfo("Config set: No key-value pairs provided");
            return 0;
        }
        
        int successCount = 0;
        
        for (const auto& arg : remainingArgs) {
            size_t equalsPos = arg.find('=');
            if (equalsPos == std::string::npos) {
                CLIFormatters::printWarning("Invalid key=value format: " + arg);
                continue;
            }
            
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            
            try {
                // Determine value type
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    config->set(key, boolValue, ConfigSource::CommandLine);
                } else if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    config->set(key, doubleValue, ConfigSource::CommandLine);
                } else {
                    int64_t intValue = std::stoll(value);
                    config->set(key, intValue, ConfigSource::CommandLine);
                }
                
                if (verbose) {
                    CLIFormatters::printInfo("Set " + key + " = " + value);
                }
                
                successCount++;
            } catch (...) {
                CLIFormatters::printWarning("Failed to parse value: " + value);
            }
        }
        
        if (successCount > 0) {
            CLIFormatters::printSuccess("Set " + std::to_string(successCount) + " configuration values");
        } else {
            CLIFormatters::printWarning("No configuration values were set");
        }
        
        return 0;
    }
    
    int executeConfigGet(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                         bool verbose) {
        // Parse keys from remaining arguments
        std::vector<std::string> keys;
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose") {
                keys.push_back(args[i]);
            }
        }
        
        if (keys.empty()) {
            CLIFormatters::printInfo("Config get: No keys provided");
            return 0;
        }
        
        int successCount = 0;
        
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                CLIFormatters::printInfo(key + " = " + value.value());
                successCount++;
            } else {
                CLIFormatters::printWarning("Key not found: " + key);
            }
        }
        
        if (successCount > 0) {
            CLIFormatters::printSuccess("Retrieved " + std::to_string(successCount) + " configuration values");
        }
        
        return 0;
    }
    
    int executeConfigDelete(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                            bool verbose, bool backup) {
        // Parse keys from remaining arguments
        std::vector<std::string> keys;
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose" && args[i] != "--backup") {
                keys.push_back(args[i]);
            }
        }
        
        if (keys.empty()) {
            CLIFormatters::printInfo("Config delete: No keys provided");
            return 0;
        }
        
        int successCount = 0;
        
        for (const auto& key : keys) {
            if (config->has(key)) {
                config->remove(key);
                if (verbose) {
                    CLIFormatters::printInfo("Deleted: " + key);
                }
                successCount++;
            } else {
                CLIFormatters::printWarning("Key not found: " + key);
            }
        }
        
        if (successCount > 0) {
            CLIFormatters::printSuccess("Deleted " + std::to_string(successCount) + " configuration values");
        }
        
        return 0;
    }
    
    int executeConfigList(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                          bool verbose, bool strict) {
        auto keys = config->getKeys();
        std::sort(keys.begin(), keys.end());
        
        std::cout << "Configuration Keys (" << keys.size() << "):" << std::endl;
        std::cout << std::endl;
        
        for (const auto& key : keys) {
            std::cout << "  " << std::left << std::setw(30) << key;
            
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                std::cout << " = " << value.value();
            } else {
                std::cout << " = <unreadable>";
            }
            
            std::cout << std::endl;
            
            if (verbose) {
                // Show additional information
                std::cout << "    Type: string" << std::endl;
            }
        }
        
        // Validate configuration
        if (strict) {
            std::vector<ValidationError> errors;
            bool isValid = config->isValid(errors);
            
            if (isValid) {
                CLIFormatters::printSuccess("Configuration is valid");
            } else {
                CLIFormatters::printError("Configuration has validation errors:");
                for (const auto& error : errors) {
                    CLIFormatters::printError("  - " + error.message);
                }
                return 1;
            }
        }
        
        CLIFormatters::printSuccess("Listed " + std::to_string(keys.size()) + " configuration keys");
        
        return 0;
    }
    
    int executeConfigValidate(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                              bool verbose, bool strict) {
        std::vector<ValidationError> errors;
        bool isValid = config->isValid(errors);
        
        if (isValid) {
            CLIFormatters::printSuccess("Configuration is valid");
            
            if (verbose) {
                std::cout << "All " << config->getKeys().size() << " configuration keys are set correctly." << std::endl;
            }
            
            return 0;
        } else {
            CLIFormatters::printError("Configuration validation failed:");
            for (const auto& error : errors) {
                CLIFormatters::printError("  - " + error.message);
                if (verbose) {
                    CLIFormatters::printError("    Key: " + error.key);
                    CLIFormatters::printError("    Type: " + ConfigValidator::errorTypeToString(error.type));
                    if (!error.suggestion.empty()) {
                        CLIFormatters::printError("    Suggestion: " + error.suggestion);
                    }
                }
            }
            
            if (strict) {
                CLIFormatters::printError("Strict validation failed");
                return 1;
            } else {
                CLIFormatters::printWarning("Non-strict validation failed");
                return 1;
            }
        }
    }
    
    int executeConfigImport(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                            bool verbose, bool backup) {
        // Parse remaining arguments
        std::string sourceFile;
        std::string targetFile;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose" && args[i] != "--backup") {
                if (sourceFile.empty()) {
                    sourceFile = args[i];
                } else {
                    targetFile = args[i];
                }
            }
        }
        
        if (sourceFile.empty()) {
            CLIFormatters::printInfo("Config import: No source file provided");
            return 0;
        }
        
        // Create backup if requested
        if (backup && config->has("config_file")) {
            std::string backupFile = config->getOr<std::string>("config_file", "") + ".backup";
            // Save current configuration to backup
            if (!saveConfigToFile(backupFile, config)) {
                CLIFormatters::printWarning("Failed to create backup configuration");
            } else {
                CLIFormatters::printInfo("Configuration backed up to: " + backupFile);
            }
        }
        
        // Import configuration
        if (!importConfigFromFile(sourceFile, config)) {
            CLIFormatters::printError("Failed to import configuration from: " + sourceFile);
            return 1;
        }
        
        CLIFormatters::printSuccess("Configuration imported from: " + sourceFile);
        
        if (!targetFile.empty()) {
            if (!saveConfigToFile(targetFile, config)) {
                CLIFormatters::printWarning("Failed to save imported configuration to: " + targetFile);
            } else {
                CLIFormatters::printInfo("Configuration saved to: " + targetFile);
            }
        }
        
        return 0;
    }
    
    int executeConfigExport(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                            bool verbose, bool backup) {
        // Parse remaining arguments
        std::string sourceFile;
        std::string targetFile;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose" && args[i] != "--backup") {
                if (sourceFile.empty()) {
                    sourceFile = args[i];
                } else {
                    targetFile = args[i];
                }
            }
        }
        
        if (sourceFile.empty()) {
            CLIFormatters::printInfo("Config export: No source file provided");
            return 0;
        }
        
        if (backup && config->has("config_file")) {
            std::string backupFile = config->getOr<std::string>("config_file", "") + ".backup";
            // Save current configuration to backup
            if (!saveConfigToFile(backupFile, config)) {
                CLIFormatters::printWarning("Failed to create backup configuration");
            } else {
                CLIFormatters::printInfo("Configuration backed up to: " + backupFile);
            }
        }
        
        // Load configuration from file
        if (!importConfigFromFile(sourceFile, config)) {
            CLIFormatters::printError("Failed to import configuration from: " + sourceFile);
            return 1;
        }
        
        // Export configuration to target file
        if (targetFile.empty()) {
            targetFile = sourceFile; // Export to same file
        }
        
        if (!saveConfigToFile(targetFile, config)) {
            CLIFormatters::printError("Failed to export configuration to: " + targetFile);
            return 1;
        }
        
        CLIFormatters::printSuccess("Configuration exported to: " + targetFile);
        
        return 0;
    }
    
    int executeConfigMerge(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                           bool verbose, bool backup) {
        // Parse remaining arguments
        std::vector<std::string> sourceFiles;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose" && args[i] != "--backup") {
                sourceFiles.push_back(args[i]);
            }
        }
        
        if (sourceFiles.size() < 2) {
            CLIFormatters::printInfo("Config merge: Need at least 2 source files");
            return 0;
        }
        
        // Create backup if requested
        if (backup && config->has("config_file")) {
            std::string backupFile = config->getOr<std::string>("config_file", "") + ".backup";
            if (!saveConfigToFile(backupFile, config)) {
                CLIFormatters::printWarning("Failed to create backup configuration");
            } else {
                CLIFormatters::printInfo("Configuration backed up to: " + backupFile);
            }
        }
        
        // Merge configurations
        for (const auto& sourceFile : sourceFiles) {
            auto tempConfig = std::make_shared<Config>();
            if (importConfigFromFile(sourceFile, tempConfig)) {
                // Merge tempConfig into config
                mergeConfigs(config, tempConfig);
                CLIFormatters::printInfo("Merged configuration from: " + sourceFile);
            } else {
                CLIFormatters::printWarning("Failed to import configuration from: " + sourceFile);
            }
        }
        
        CLIFormatters::printSuccess("Configuration merge completed");
        
        return 0;
    }
    
    int executeConfigDiff(const std::vector<std::string>& args, std::shared_ptr<Config> config,
                          bool verbose) {
        // Parse remaining arguments
        std::vector<std::string> otherFiles;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] != "--verbose") {
                otherFiles.push_back(args[i]);
            }
        }
        
        if (otherFiles.empty()) {
            CLIFormatters::printInfo("Config diff: No comparison files provided");
            return 0;
        }
        
        std::cout << "Configuration Differences:" << std::endl;
        std::cout << "========================" << std::endl;
        
        for (const auto& otherFile : otherFiles) {
            std::cout << "\nComparison with: " << otherFile << std::endl;
            std::cout << "--------------------------" << std::endl;
            
            auto otherConfig = std::make_shared<Config>();
            if (importConfigFromFile(otherFile, otherConfig)) {
                compareConfigs(config, otherConfig, verbose);
            } else {
                CLIFormatters::printWarning("Failed to import configuration from: " + otherFile);
            }
        }
        
        return 0;
    }
    
    void compareConfigs(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2,
                        bool verbose) {
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Find keys in config1 but not in config2
        std::vector<std::string> onlyInConfig1;
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
                onlyInConfig1.push_back(key);
            }
        }
        
        // Find keys in config2 but not in config1
        std::vector<std::string> onlyInConfig2;
        for (const auto& key : keys2) {
            if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
                onlyInConfig2.push_back(key);
            }
        }
        
        // Find differing values
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> differingValues;
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
                auto value1 = config1->get<std::string>(key);
                auto value2 = config2->get<std::string>(key);
                if (value1.has_value() && value2.has_value() && value1.value() != value2.value()) {
                    differingValues.emplace_back(key, std::make_pair(value1.value(), value2.value()));
                }
            }
        }
        
        // Display differences
        if (!onlyInConfig1.empty()) {
            std::cout << "Keys only in first configuration:" << std::endl;
            for (const auto& key : onlyInConfig1) {
                auto value = config1->get<std::string>(key);
                if (value.has_value()) {
                    std::cout << "  + " << key << " = " << value.value() << std::endl;
                }
            }
        }
        
        if (!onlyInConfig2.empty()) {
            std::cout << "Keys only in second configuration:" << std::endl;
            for (const auto& key : onlyInConfig2) {
                auto value = config2->get<std::string>(key);
                if (value.has_value()) {
                    std::cout << "  - " << key << " = " << value.value() << std::endl;
                }
            }
        }
        
        if (!differingValues.empty()) {
            std::cout << "Differing values:" << std::endl;
            for (const auto& diff : differingValues) {
                std::cout << "  ~ " << diff.first << ": " << diff.second.first << " -> " << diff.second.second << std::endl;
            }
        }
    }
    
    bool importConfigFromFile(const std::string& filename, std::shared_ptr<Config> config) {
        // Import configuration from file (JSON or text)
        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Parse content (simplified)
        // This should be replaced with proper JSON/text parsing
        return true;
    }
    
    bool saveConfigToFile(const std::string& filename, std::shared_ptr<Config> config) {
        // Save configuration to file
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration
        file << config->summary() << std::endl;
        
        file.close();
        return true;
    }
    
    void mergeConfigs(std::shared_ptr<Config> targetConfig, std::shared_ptr<Config> sourceConfig) {
        // Merge source configuration into target configuration
        auto sourceKeys = sourceConfig->getKeys();
        
        for (const auto& key : sourceKeys) {
            auto value = sourceConfig->get<std::string>(key);
            if (value.has_value()) {
                targetConfig->set(key, value.value(), ConfigSource::File);
            }
        }
    }
    
    // Helper methods
    std::shared_ptr<Config> getConfig(const std::string& configFile) {
        // Get configuration from file or create default
        if (configFile.empty() || !std::filesystem::exists(configFile)) {
            return std::make_shared<Config>();
        }
        
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        return config;
    }
};

} // namespace nlm
