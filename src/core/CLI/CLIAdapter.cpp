// CLIAdapter implementation - adapts existing executables to use new CLI
#include "CLIAdapter.hpp"
#include "CLIFramework.hpp"
#include "CLIMacros.hpp"
#include <iostream>

namespace nlm {

// Static member initialization
std::unique_ptr<CLIParser> CLIAdapter::parser = std::make_unique<CLIParser>();
std::unique_ptr<CLIFactory> CLIAdapter::factory = std::make_unique<CLIFactory>();

int CLIAdapter::adaptMain(int argc, char* argv[]) {
    if (shouldShowHelpOrVersion(argc, argv)) {
        parser->printHelp();
        return 0;
    }
    
    CLIConfig config;
    if (!parser->parse(argc, argv, config)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }
    
    if (!parser->validate(config)) {
        return 1;
    }
    
    // Create configuration
    auto nlmConfig = factory->createConfig(config);
    
    // Setup logging
    factory->setupLogging(config);
    
    // Load configuration from file
    if (!config.config_file.empty()) {
        auto loadedConfig = factory->loadConfiguration(config.config_file, config);
        if (loadedConfig) {
            nlmConfig = loadedConfig;
        }
    }
    
    // Execute command
    return factory->executeCommand(config, nlmConfig);
}

int CLIAdapter::adaptPhase3Demo(int argc, char* argv[]) {
    if (shouldShowHelpOrVersion(argc, argv)) {
        parser->printHelp();
        return 0;
    }
    
    CLIConfig config;
    if (!parser->parse(argc, argv, config)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }
    
    if (!parser->validate(config)) {
        return 1;
    }
    
    // Set demo type to phase3
    config.command = "demo";
    config.demo_type = "phase3";
    config.command_args = std::vector<std::string>(argv + 1, argv + argc);
    
    // Create configuration
    auto nlmConfig = factory->createConfig(config);
    
    // Setup logging
    factory->setupLogging(config);
    
    // Load configuration from file
    if (!config.config_file.empty()) {
        auto loadedConfig = factory->loadConfiguration(config.config_file, config);
        if (loadedConfig) {
            nlmConfig = loadedConfig;
        }
    }
    
    // Execute command
    return factory->executeCommand(config, nlmConfig);
}

int CLIAdapter::adaptPhase4Demo(int argc, char* argv[]) {
    if (shouldShowHelpOrVersion(argc, argv)) {
        parser->printHelp();
        return 0;
    }
    
    CLIConfig config;
    if (!parser->parse(argc, argv, config)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }
    
    if (!parser->validate(config)) {
        return 1;
    }
    
    // Set demo type to phase4
    config.command = "demo";
    config.demo_type = "phase4";
    config.command_args = std::vector<std::string>(argv + 1, argv + argc);
    
    // Create configuration
    auto nlmConfig = factory->createConfig(config);
    
    // Setup logging
    factory->setupLogging(config);
    
    // Load configuration from file
    if (!config.config_file.empty()) {
        auto loadedConfig = factory->loadConfiguration(config.config_file, config);
        if (loadedConfig) {
            nlmConfig = loadedConfig;
        }
    }
    
    // Execute command
    return factory->executeCommand(config, nlmConfig);
}

int CLIAdapter::adaptPhase6Demo(int argc, char* argv[]) {
    // For now, use the original Phase 6 demo logic
    // In the future, this could call factory->executeDemoCommand with demo_type = "phase6"
    return 0;
}

int CLIAdapter::adaptBenchmark(int argc, char* argv[]) {
    // For now, use the original benchmark logic
    // In the future, this could call factory->executeBenchmarkCommand
    return 0;
}

bool CLIAdapter::shouldShowHelpOrVersion(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "--version") {
            return true;
        }
    }
    return false;
}

} // namespace nlm