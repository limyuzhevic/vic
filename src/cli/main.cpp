// NLM Configuration CLI Application
// Command-line interface for the enhanced NLM configuration system

#include "core/Config/Config.hpp"
#include "core/CLI/ConfigCLI.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Parse command line arguments
    nlm::ConfigCLI::CLIOptions options = nlm::ConfigCLI::parseArgs(argc, argv);
    
    nlm::ConfigCLI cli;
    
    // Handle help and version options
    if (options.help || options.showHelp) {
        nlm::ConfigCLI::printHelp(options);
        return 0;
    }
    
    if (options.version) {
        nlm::ConfigCLI::printVersion();
        return 0;
    }
    
    // Create configuration object
    nlm::Config config;
    
    // Load configuration
    if (!options.configFile.empty()) {
        std::cout << "Loading configuration from: " << options.configFile << std::endl;
        if (!cli.loadConfig(config, options.configFile, options.merge)) {
            std::cerr << "Failed to load configuration file: " << options.configFile << std::endl;
            return 1;
        }
    } else {
        std::cout << "Loading configuration from default location..." << std::endl;
        if (!cli.loadConfig(config, "", options.merge)) {
            std::cout << "Using default configuration." << std::endl;
        }
    }
    
    // Apply template if specified
    if (!options.templateName.empty()) {
        std::cout << "Applying template: " << options.templateName << std::endl;
        if (!cli.applyTemplate(config, options.templateName)) {
            std::cerr << "Failed to apply template: " << options.templateName << std::endl;
            return 1;
        }
    }
    
    // Set inheritance rules if specified
    if (!options.inheritanceRules.empty()) {
        std::cout << "Setting inheritance rules..." << std::endl;
        if (!cli.setInheritance(config, options.inheritanceRules)) {
            std::cerr << "Failed to set inheritance rules." << std::endl;
            return 1;
        }
    }
    
    // Validate configuration if requested
    if (options.validate) {
        std::cout << "Validating configuration..." << std::endl;
        if (!cli.validateConfig(config)) {
            std::cerr << "Configuration validation failed." << std::endl;
            return 1;
        }
        std::cout << "Configuration validation passed." << std::endl;
    }
    
    // Show configuration summary
    if (options.verbose) {
        std::cout << cli.showConfig(config) << std::endl;
    }
    
    // Create backup if requested
    if (options.backup) {
        std::cout << "Creating backup..." << std::endl;
        if (!cli.createBackup(config)) {
            std::cerr << "Failed to create backup." << std::endl;
            return 1;
        }
        std::cout << "Backup created successfully." << std::endl;
    }
    
    // Export configuration if requested
    if (options.exportAll) {
        std::cout << "Exporting configuration..." << std::endl;
        if (!cli.exportConfig(config, options.format)) {
            std::cerr << "Failed to export configuration." << std::endl;
            return 1;
        }
    }
    
    // Save configuration if output file specified
    if (!options.outputFile.empty()) {
        std::cout << "Saving configuration to: " << options.outputFile << std::endl;
        if (!cli.saveConfig(config, options.outputFile, options.format)) {
            std::cerr << "Failed to save configuration." << std::endl;
            return 1;
        }
        std::cout << "Configuration saved successfully." << std::endl;
    }
    
    // Show help if no action specified and no arguments
    if (argc == 1) {
        nlm::ConfigCLI::printHelp(options);
        return 0;
    }
    
    std::cout << "Configuration processing completed successfully." << std::endl;
    return 0;
}