#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "src/core/Config/EnhancedConfig.hpp"

void printUsage() {
    std::cout << "NLM Enhanced Configuration System Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --profile <name>        Load configuration profile" << std::endl;
    std::cout << "  --validate              Validate current configuration" << std::endl;
    std::cout << "  --show-profile-help     Show available profiles" << std::endl;
    std::cout << "  --create-backup <name>  Create a backup" << std::endl;
    std::cout << "  --restore-backup <name> Restore from backup" << std::endl;
    std::cout << "  --load-env              Load from environment variables" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  ./demo --config=research.cfg --validate" << std::endl;
    std::cout << "  ./demo --profile=production --create-backup=my_backup" << std::endl;
    std::cout << std::endl;
}

void showExampleConfig() {
    std::cout << "Example Configuration File (research.cfg):" << std::endl;
    std::cout << "# Research Configuration Profile" << std::endl;
    std::cout << "# Optimized for research and experimentation" << std::endl;
    std::cout << std::endl;
    std::cout << "random_seed = 12345" << std::endl;
    std::cout << "simulation_timestep = 0.0001" << std::endl;
    std::cout << "neuron_count = 500" << std::endl;
    std::cout << "region_count = 3" << std::endl;
    std::cout << "connection_probability = 0.15" << std::endl;
    std::cout << "plasticity_learning_rate = 0.005" << std::endl;
    std::cout << "stdp_ltp_weight = 0.008" << std::endl;
    std::cout << "stdp_ltd_weight = 0.012" << std::endl;
    std::cout << "log_level = DEBUG" << std::endl;
    std::cout << "log_to_file = true" << std::endl;
    std::cout << "visualization_enabled = true" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        nlm::Config config;
        nlm::ConfigSchema schema;
        
        // Add example parameters to schema
        schema.addParameter(
            nlm::ConfigParameter("random_seed", nlm::ConfigType::Integer, "Random seed for reproducible experiments")
                .defaultValue(42)
                .range(1, 1000000)
                .positive()
        );
        
        schema.addParameter(
            nlm::ConfigParameter("simulation_timestep", nlm::ConfigType::Float, "Simulation timestep in seconds")
                .defaultValue(0.001)
                .range(0.0001, 1.0)
                .positive()
                .nonNegative()
        );
        
        schema.addParameter(
            nlm::ConfigParameter("neuron_count", nlm::ConfigType::Integer, "Number of neurons in the brain")
                .defaultValue(1000)
                .range(10, 100000)
                .positive()
                .nonNegative()
        );
        
        schema.addParameter(
            nlm::ConfigParameter("log_level", nlm::ConfigType::String, "Logging level (DEBUG, INFO, WARNING, ERROR)")
                .defaultValue("INFO")
        );
        
        std::cout << "NLM Enhanced Configuration System" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << std::endl;
        
        // Process command line arguments
        bool validate = false;
        std::string configFile;
        std::string profile;
        std::string backupName;
        bool showProfileHelp = false;
        bool loadEnv = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage();
                return 0;
            } else if (arg == "--validate") {
                validate = true;
            } else if (arg.substr(0, 9) == "--config=") {
                configFile = arg.substr(9);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg.substr(0, 10) == "--profile=") {
                profile = arg.substr(10);
            } else if (arg == "--profile" && i + 1 < argc) {
                profile = argv[++i];
            } else if (arg.substr(0, 14) == "--create-backup=") {
                backupName = arg.substr(14);
            } else if (arg == "--create-backup" && i + 1 < argc) {
                backupName = argv[++i];
            } else if (arg.substr(0, 16) == "--restore-backup=") {
                backupName = arg.substr(16);
            } else if (arg == "--restore-backup" && i + 1 < argc) {
                backupName = argv[++i];
            } else if (arg == "--show-profile-help") {
                showProfileHelp = true;
            } else if (arg == "--load-env") {
                loadEnv = true;
            } else {
                std::cout << "Unknown option: " << arg << std::endl;
                printUsage();
                return 1;
            }
        }
        
        // Handle profile help
        if (showProfileHelp) {
            std::cout << config.getProfileHelp() << std::endl;
            return 0;
        }
        
        // Load profile if specified
        if (!profile.empty()) {
            std::transform(profile.begin(), profile.end(), profile.begin(), ::tolower);
            if (profile == "research") {
                config.loadFromProfile(nlm::ConfigProfile::Research, schema);
            } else if (profile == "simulation") {
                config.loadFromProfile(nlm::ConfigProfile::Simulation, schema);
            } else if (profile == "production") {
                config.loadFromProfile(nlm::ConfigProfile::Production, schema);
            } else if (profile == "development") {
                config.loadFromProfile(nlm::ConfigProfile::Development, schema);
            } else if (profile == "benchmark") {
                config.loadFromProfile(nlm::ConfigProfile::Benchmark, schema);
            } else {
                std::cout << "Unknown profile: " << profile << std::endl;
                printUsage();
                return 1;
            }
            
            std::cout << "Loaded " << profile << " profile." << std::endl;
        }
        
        // Load from file if specified
        if (!configFile.empty()) {
            if (!config.loadFromFile(configFile)) {
                std::cout << "Failed to load configuration from: " << configFile << std::endl;
                return 1;
            }
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
        
        // Load from environment variables
        if (loadEnv) {
            if (!config.loadFromEnvironment()) {
                std::cout << "Warning: Failed to load from environment variables" << std::endl;
            } else {
                std::cout << "Loaded configuration from environment variables." << std::endl;
            }
        }
        
        // Create backup if requested
        if (!backupName.empty()) {
            if (!config.createBackup(backupName, "Configuration backup")) {
                std::cout << "Failed to create backup: " << backupName << std::endl;
                return 1;
            }
            std::cout << "Created backup: " << backupName << std::endl;
        }
        
        // Validate configuration if requested
        if (validate && !schema.getKeys().empty()) {
            auto errors = config.validateAll(schema);
            if (!errors.empty()) {
                std::cout << "Configuration validation failed:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << error.toString() << std::endl;
                }
                std::cout << std::endl;
                std::cout << "Suggestions:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << config.getErrorSuggestion(error) << std::endl;
                }
                return 1;
            }
            std::cout << "Configuration validation passed." << std::endl;
        }
        
        // Show configuration help if no configuration was loaded
        if (configFile.empty() && profile.empty() && backupName.empty()) {
            std::cout << "Use --help to see all available options." << std::endl;
            std::cout << "Use --show-profile-help to see available profiles." << std::endl;
        }
        
        // Show configuration summary
        std::cout << std::endl;
        std::cout << config.summary(schema) << std::endl;
        
        // Show example configuration file
        std::cout << std::endl;
        showExampleConfig();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}