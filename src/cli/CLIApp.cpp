// NLM (熙然) - Neural Learning Machine CLI Application Implementation
//
// This file provides the implementation for the main CLI application.

#include "cli/CLIApp.hpp"
#include "core/Logger/Logger.hpp"
#include "errors/ErrorTypes.hpp"
#include "errors/ErrorContext.hpp"
#include "error_reporting/ErrorReporter.hpp"
#include "monitoring/SystemMonitor.hpp"
#include <iostream>

namespace nlm {
namespace cli {

CLIApp::CLIApp() {
    NLM_LOG_INFO("Initializing NLM CLI Application");
}

int CLIApp::run(int argc, char** argv) {
    NLM_LOG_INFO("NLM CLI Application started");
    
    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        NLM_LOG_ERROR("Failed to parse command line arguments");
        return 1;
    }
    
    // Handle help and version requests
    if (parser.showHelp() || parser.getCommand() == CommandType::Help) {
        helpSystem.displayFullHelp();
        return 0;
    }
    
    if (parser.showVersion() || parser.getCommand() == CommandType::Version) {
        parser.displayVersion();
        return 0;
    }
    
    if (parser.listPresets() || parser.getCommand() == CommandType::ListPresets) {
        parser.displayPresets();
        return 0;
    }
    
    // Configure logging
    loggerConfig.configure(parser.getVerbosity(), parser.getLogFile(), parser.getOutputFile());
    
    // Initialize configuration manager
    if (parser.getConfigFile().empty()) {
        NLM_LOG_INFO("Using default configuration");
    } else {
        if (!configManager.loadFromFile(parser.getConfigFile())) {
            NLM_LOG_WARNING("Failed to load configuration file, using defaults");
        }
    }
    
    // Apply command line overrides
    configManager.applyOverrides(parser.getOptions());
    
    // Get the configuration
    config = configManager.getConfig();
    
    // Load preset if specified
    std::string preset = parser.getOptions().count("preset") ? parser.getOptions().at("preset") : "";
    if (!preset.empty()) {
        if (!configManager.applyPreset(preset)) {
            NLM_LOG_ERROR("Failed to apply preset: " + preset);
            return 1;
        }
    }
    
    // Execute the requested command
    int result = execute();
    
    NLM_LOG_INFO("NLM CLI Application finished with exit code: " + std::to_string(result));
    return result;
}

int CLIApp::execute() {
    CommandType command = parser.getCommand();
    
    switch (command) {
        case CommandType::Run:
            NLM_LOG_INFO("Executing run command");
            if (commandHandler.executeRunCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Run command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Run command failed");
                return 1;
            }
        
        case CommandType::Experiment:
            NLM_LOG_INFO("Executing experiment command");
            if (commandHandler.executeExperimentCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Experiment command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Experiment command failed");
                return 1;
            }
        
        case CommandType::Analyze:
            NLM_LOG_INFO("Executing analyze command");
            if (commandHandler.executeAnalyzeCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Analyze command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Analyze command failed");
                return 1;
            }
        
        case CommandType::Benchmark:
            NLM_LOG_INFO("Executing benchmark command");
            if (commandHandler.executeBenchmarkCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Benchmark command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Benchmark command failed");
                return 1;
            }
        
        case CommandType::Configure:
            NLM_LOG_INFO("Executing configure command");
            if (commandHandler.executeConfigureCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Configure command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Configure command failed");
                return 1;
            }
        
        case CommandType::ShowConfig:
            NLM_LOG_INFO("Executing show-config command");
            if (commandHandler.executeShowConfigCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Show-config command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Show-config command failed");
                return 1;
            }
        
        case CommandType::SaveConfig:
            NLM_LOG_INFO("Executing save-config command");
            if (commandHandler.executeSaveConfigCommand(parser.getOptions(), config)) {
                NLM_LOG_INFO("Save-config command completed successfully");
                return 0;
            } else {
                NLM_LOG_ERROR("Save-config command failed");
                return 1;
            }
        
        default:
            NLM_LOG_ERROR("Unknown command");
            return 1;
    }
}

} // namespace cli
} // namespace nlm
