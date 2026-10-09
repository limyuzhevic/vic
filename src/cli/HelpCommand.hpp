// Help Command - Display help information

#pragma once

#include "CommandBase.hpp"
#include "CLIParser.hpp"
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

class HelpCommand : public CommandBase {
private:
    std::map<std::string, CommandBase*>* commands;
    
public:
    HelpCommand(std::map<std::string, CommandBase*>* cmds) 
        : CommandBase("help", "Show help information"), commands(cmds) {}
    
    int execute(std::shared_ptr<Config> config, const CLIParser& parser) override {
        std::vector<std::string> args = parser.getPositionalArgs();
        
        if (args.empty()) {
            // Show general help
            printGeneralHelp();
            return 0;
        }
        
        std::string commandName = args[0];
        
        // Find the command
        auto it = commands->find(commandName);
        if (it == commands->end()) {
            std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
            return 1;
        }
        
        CommandBase* command = it->second;
        
        // Show command-specific help
        printCommandHelp(command, parser);
        
        return 0;
    }
    
private:
    void printGeneralHelp() {
        std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm <command> [options] [arguments]" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        
        for (const auto& pair : *commands) {
            const std::string& cmd = pair.first;
            CommandBase* command = pair.second;
            
            // Left-align command names
            std::cout << "  " << std::left << std::setw(15) << cmd 
                      << "  " << command->getDescription() << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Use 'nlm <command> --help' for more information about a specific command." << std::endl;
        std::cout << std::endl;
        std::cout << "Global options:" << std::endl;
        std::cout << "  --verbose, -v    Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q      Suppress non-error messages" << std::endl;
        std::cout << "  --config <file>   Load configuration from file" << std::endl;
        std::cout << "  --help, -h        Show this help message" << std::endl;
        std::cout << "  --version         Show version information" << std::endl;
    }
    
    void printCommandHelp(CommandBase* command, const CLIParser& parser) {
        std::string cmdName = command->getName();
        std::string cmdDesc = command->getDescription();
        
        std::cout << "Help for command: " << cmdName << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        std::cout << cmdDesc << std::endl;
        std::cout << std::endl;
        
        // Command-specific options
        printCommandOptions(command, parser);
        
        // Examples (placeholder)
        printExamples(commandName);
    }
    
    void printCommandOptions(CommandBase* command, const CLIParser& parser) {
        // In a real implementation, each command would provide its own options
        // For now, just show generic options
        std::cout << "Options:" << std::endl;
        
        // Common options for many commands
        if (command->getName() != "config") {
            std::cout << "  --verbose, -v    Enable verbose output" << std::endl;
        }
        
        if (command->getName() != "save" && command->getName() != "load") {
            std::cout << "  --help, -h        Show this help message" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void printExamples(const std::string& commandName) {
        std::cout << "Examples:" << std::endl;
        
        if (commandName == "run") {
            std::cout << "  nlm run                                    Run simulation with default config" << std::endl;
            std::cout << "  nlm run --verbose                         Run with verbose output" << std::endl;
            std::cout << "  nlm run --config my_config.cfg             Run with custom config" << std::endl;
        } else if (commandName == "config") {
            std::cout << "  nlm config                                 Show current configuration" << std::endl;
            std::cout << "  nlm config --key neuron_count --value 2000   Set neuron count" << std::endl;
            std::cout << "  nlm config save --output my_config.cfg     Save config to file" << std::endl;
            std::cout << "  nlm config load --input my_config.cfg     Load config from file" << std::endl;
        } else if (commandName == "save") {
            std::cout << "  nlm save                                   Save brain state" << std::endl;
            std::cout << "  nlm save --output brain_state.nbm           Save to custom file" << std::endl;
        } else if (commandName == "load") {
            std::cout << "  nlm load --input brain_state.nbm          Load brain state" << std::endl;
        } else if (commandName == "benchmark") {
            std::cout << "  nlm benchmark                               Run default benchmarks" << std::endl;
            std::cout << "  nlm benchmark 1000,500                    Run custom benchmarks" << std::endl;
            std::cout << "  nlm benchmark --detailed                   Show detailed results" << std::endl;
        } else if (commandName == "debug") {
            std::cout << "  nlm debug                                   Show debug menu" << std::endl;
            std::cout << "  nlm debug config                            Debug configuration" << std::endl;
            std::cout << "  nlm debug memory                            Memory diagnostics" << std::endl;
        } else if (commandName == "visualize") {
            std::cout << "  nlm visualize                               Show visualization menu" << std::endl;
            std::cout << "  nlm visualize network                      Show network topology" << std::endl;
            std::cout << "  nlm visualize activity --steps 500          Show activity patterns" << std::endl;
        }
        
        std::cout << std::endl;
    }
};
