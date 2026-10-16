#pragma once

#include "NLMCLI.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

class HelpCommand : public ICommand {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show detailed help for all commands"; }
    std::string getUsage() const override { return "help [command]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        // No required arguments
        return true;
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli help [command]" << std::endl;
        std::cout << "Show detailed help for all commands or a specific command." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  <command>            Command name to show help for" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h           Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            // Show general help
            std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                               NLM CLI Help                                     ║" << std::endl;
            std::cout << "║                                                                               ║" << std::endl;
            std::cout << "║  nlm-cli is the comprehensive command-line interface for the Neural Learning   ║" << std::endl;
            std::cout << "║  Machine (NLM) brain simulation project.                                      ║" << std::endl;
            std::cout << "║                                                                               ║" << std::endl;
            std::cout << "║  Usage:                                                                        ║" << std::endl;
            std::cout << "║    nlm-cli [global-options] <command> [command-options]                       ║" << std::endl;
            std::cout << "║                                                                               ║" << std::endl;
            std::cout << "║  Global Options:                                                               ║" << std::endl;
            std::cout << "║    --config <file>                    Configuration file                        ║" << std::endl;
            std::cout << "║    --output <format>                  Output format (json, csv, text)         ║" << std::endl;
            std::cout << "║    --log-level <level>                Logging level                           ║" << std::endl;
            std::cout << "║    --verbose, -v                      Verbose output                         ║" << std::endl;
            std::cout << "║    --quiet, -q                        Quiet output                           ║" << std::endl;
            std::cout << "║    --help                             Show help                              ║" << std::endl;
            std::cout << "║    --version                          Show version                           ║" << std::endl;
            std::cout << "║                                                                               ║" << std::endl;
            std::cout << "║  Commands:                                                                    ║" << std::endl;
            std::cout << "║    run      Run a simulation with specified scenario and configuration      ║" << std::endl;
            std::cout << "║    config   Configuration management (create, edit, validate, save/load)     ║" << std::endl;
            std::cout << "║    experiment Run predefined experiments                                    ║" << std::endl;
            std::cout << "║    visualize Generate visualization outputs                                 ║" << std::endl;
            std::cout << "║    benchmark Performance benchmarking                                      ║" << std::endl;
            std::cout << "║    status    Show system status and information                              ║" << std::endl;
            std::cout << "║    help      Show detailed help for all commands                             ║" << std::endl;
            std::cout << "║                                                                               ║" << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        // Show help for specific command
        std::string commandName = args[0];
        auto& commandRunner = getCommandRunner();
        
        if (commandRunner.hasCommand(commandName)) {
            std::string helpText = commandRunner.getCommandHelp(commandName);
            std::cout << helpText;
            
            result.success = true;
            result.message = "Help for command: " + commandName;
            result.output = helpText;
        } else {
            std::ostringstream oss;
            oss << "Unknown command: " << commandName << std::endl;
            oss << "Available commands:" << std::endl;
            auto descriptions = commandRunner.getCommandDescriptions();
            for (const auto& pair : descriptions) {
                oss << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            result.success = false;
            result.message = "Unknown command: " + commandName;
            result.exitCode = 1;
            result.output = oss.str();
        }
        
        return result;
    }
    
}; // class HelpCommand

} // namespace nlm