// src/cli/AdvancedCLI.cpp
#include "AdvancedCLI.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <cassert>

namespace nlm {

struct AdvancedCLI::Impl {
    std::string configFile;
    bool verbose;
    bool interactive;
    std::map<std::string, std::string> commandDescriptions;
    
    Impl() : configFile("")
           , verbose(false)
           , interactive(false) {
        // Initialize command descriptions for help system
        commandDescriptions["run"] = "Run NLM simulation with specified configuration";
        commandDescriptions["config"] = "Manage configuration files (load, save, list)";
        commandDescriptions["visualize"] = "Launch interactive visualization mode";
        commandDescriptions["script"] = "Execute script with multiple commands";
        commandDescriptions["help"] = "Show help information for commands";
        commandDescriptions["version"] = "Show version information";
    }
};

AdvancedCLI::AdvancedCLI() : pImpl(std::make_unique<Impl>()) {
}

AdvancedCLI::~AdvancedCLI() = default;

CLIStatus AdvancedCLI::parseArguments(int argc, char* argv[]) {
    NLM_LOG_INFO("NLM Advanced CLI - parsing arguments...");
    
    if (argc <= 1) {
        displayHelp("nlm");
        return CLIStatus::HelpRequested;
    }
    
    std::string command = argv[1];
    std::vector<std::string> arguments;
    
    // Parse command and arguments
    for (int i = 2; i < argc; ++i) {
        arguments.push_back(argv[i]);
    }
    
    // Check for global flags first
    for (const auto& arg : arguments) {
        if (arg == "--verbose" || arg == "-v") {
            pImpl->verbose = true;
        } else if (arg == "--interactive" || arg == "-i") {
            pImpl->interactive = true;
        } else if (arg.find("--config=") == 0) {
            setConfigFile(arg.substr(9)); // Remove "--config=" prefix
        }
    }
    
    // Remove processed arguments for command execution
    std::vector<std::string> commandArgs;
    for (const auto& arg : arguments) {
        if (arg != "--verbose" && arg != "-v" && 
            arg != "--interactive" && arg != "-i" &&
            arg.find("--config=") != 0) {
            commandArgs.push_back(arg);
        }
    }
    
    // Execute command
    CLIStatus result = executeCommand(command, commandArgs);
    
    if (result == CLIStatus::Error) {
        NLM_LOG_ERROR("Command execution failed: " + command);
        return CLIStatus::Error;
    }
    
    return CLIStatus::Success;
}

bool AdvancedCLI::setConfigFile(const std::string& configPath) {
    std::filesystem::path path(configPath);
    
    if (!std::filesystem::exists(path)) {
        NLM_LOG_ERROR("Configuration file not found: " + configPath);
        return false;
    }
    
    if (!std::filesystem::is_regular_file(path)) {
        NLM_LOG_ERROR("Path is not a regular file: " + configPath);
        return false;
    }
    
    pImpl->configFile = configPath;
    NLM_LOG_INFO("Configuration file set to: " + configPath);
    return true;
}

std::string AdvancedCLI::getConfigFile() const {
    return pImpl->configFile;
}

bool AdvancedCLI::loadConfiguration(std::shared_ptr<Config> config) {
    if (pImpl->configFile.empty()) {
        NLM_LOG_ERROR("No configuration file specified");
        return false;
    }
    
    if (config->loadFromFile(pImpl->configFile)) {
        NLM_LOG_INFO("Configuration loaded successfully from: " + pImpl->configFile);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + pImpl->configFile);
        return false;
    }
}

bool AdvancedCLI::saveConfiguration(std::shared_ptr<Config> config, const std::string& outputPath) {
    std::filesystem::path path(outputPath);
    std::filesystem::path parentDir = path.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        if (!std::filesystem::create_directories(parentDir)) {
            NLM_LOG_ERROR("Failed to create directory: " + parentDir.string());
            return false;
        }
    }
    
    if (config->saveToFile(outputPath)) {
        NLM_LOG_INFO("Configuration saved successfully to: " + outputPath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to save configuration to: " + outputPath);
        return false;
    }
}

void AdvancedCLI::displayHelp(const std::string& programName) {
    std::cout << "NLM (Neural Learning Machine) - Advanced Command Line Interface" << std::endl;
    std::cout << "=================================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [command] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    
    for (const auto& pair : pImpl->commandDescriptions) {
        std::cout << "  " << pair.first << " - " << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Global options:" << std::endl;
    std::cout << "  --verbose, -v    Enable verbose output" << std::endl;
    std::cout << "  --interactive, -i  Enable interactive mode" << std::endl;
    std::cout << "  --config=file    Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " run --config=simulation.cfg" << std::endl;
    std::cout << "  " << programName << " config --save=default.cfg" << std::endl;
    std::cout << "  " << programName << " visualize --config=debug.cfg" << std::endl;
    std::cout << "  " << programName << " help run" << std::endl;
}

void AdvancedCLI::displayVersion(const std::string& programName) {
    std::cout << programName << " - NLM (Neural Learning Machine)" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

std::vector<std::string> AdvancedCLI::getAvailableCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : pImpl->commandDescriptions) {
        commands.push_back(pair.first);
    }
    return commands;
}

CLIStatus AdvancedCLI::executeCommand(const std::string& command, const std::vector<std::string>& arguments) {
    NLM_LOG_INFO("Executing command: " + command);
    
    if (!isCommandAvailable(command)) {
        NLM_LOG_ERROR("Unknown command: " + command);
        displayHelp("nlm");
        return CLIStatus::Error;
    }
    
    if (command == "help") {
        displayHelp("nlm");
        return CLIStatus::Success;
    } else if (command == "version") {
        displayVersion("nlm");
        return CLIStatus::Success;
    } else if (command == "run") {
        return executeRunCommand(arguments);
    } else if (command == "config") {
        return executeConfigCommand(arguments);
    } else if (command == "visualize") {
        return executeVisualizeCommand(arguments);
    } else if (command == "script") {
        return executeScriptCommand(arguments);
    }
    
    NLM_LOG_ERROR("Command implementation not yet available: " + command);
    return CLIStatus::Error;
}

bool AdvancedCLI::isCommandAvailable(const std::string& command) const {
    return pImpl->commandDescriptions.find(command) != pImpl->commandDescriptions.end();
}

CLIStatus AdvancedCLI::execute() {
    NLM_LOG_INFO("Advanced CLI execution completed");
    return CLIStatus::Success;
}

int AdvancedCLI::executeRunCommand(const std::vector<std::string>& arguments) {
    NLM_LOG_INFO("Run command executed");
    
    // For now, just display the arguments
    std::cout << "Run command with arguments:" << std::endl;
    for (const auto& arg : arguments) {
        std::cout << "  " << arg << std::endl;
    }
    
    // TODO: Implement actual run functionality
    // This should integrate with existing NLM demos
    
    std::cout << "Note: Run command implementation is in progress." << std::endl;
    return 0;
}

int AdvancedCLI::executeConfigCommand(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        NLM_LOG_ERROR("Config command requires subcommand (load, save, list, validate)");
        return 1;
    }
    
    std::string subcommand = arguments[0];
    
    if (subcommand == "load") {
        if (arguments.size() < 2) {
            NLM_LOG_ERROR("Config load requires file path");
            return 1;
        }
        std::string configPath = arguments[1];
        // TODO: Implement config load functionality
        NLM_LOG_INFO("Config load command: " + configPath);
        return 0;
    } else if (subcommand == "save") {
        if (arguments.size() < 2) {
            NLM_LOG_ERROR("Config save requires file path");
            return 1;
        }
        std::string configPath = arguments[1];
        // TODO: Implement config save functionality
        NLM_LOG_INFO("Config save command: " + configPath);
        return 0;
    } else if (subcommand == "list") {
        // TODO: Implement config list functionality
        NLM_LOG_INFO("Config list command: show available configurations");
        return 0;
    } else if (subcommand == "validate") {
        if (arguments.size() < 2) {
            NLM_LOG_ERROR("Config validate requires file path");
            return 1;
        }
        std::string configPath = arguments[1];
        // TODO: Implement config validate functionality
        NLM_LOG_INFO("Config validate command: " + configPath);
        return 0;
    }
    
    NLM_LOG_ERROR("Unknown config subcommand: " + subcommand);
    return 1;
}

int AdvancedCLI::executeVisualizeCommand(const std::vector<std::string>& arguments) {
    NLM_LOG_INFO("Visualize command executed");
    
    // TODO: Implement visualization mode
    // This should launch interactive visualization
    
    std::cout << "Visualize command: launching interactive visualization" << std::endl;
    std::cout << "Note: Visualization implementation is in progress." << std::endl;
    return 0;
}

int AdvancedCLI::executeScriptCommand(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        NLM_LOG_ERROR("Script command requires script file path");
        return 1;
    }
    
    std::string scriptPath = arguments[0];
    std::filesystem::path path(scriptPath);
    
    if (!std::filesystem::exists(path)) {
        NLM_LOG_ERROR("Script file not found: " + scriptPath);
        return 1;
    }
    
    // TODO: Implement script execution
    NLM_LOG_INFO("Script command: " + scriptPath);
    std::cout << "Script execution: " << scriptPath << std::endl;
    std::cout << "Note: Script execution implementation is in progress." << std::endl;
    return 0;
}

} // namespace nlm