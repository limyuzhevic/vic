// CLIParser implementation
//
// Argument parsing with subcommand support

#include "CLIParser.hpp"
#include "CLICommand.hpp"
#include "HelpSystem.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLIParser::CLIParser(const std::string& name, const std::string& description) 
    : name(name), description(description) {}

CLIParser::~CLIParser() = default;

void CLIParser::addCommand(std::unique_ptr<CLICommand> command) {
    if (command) {
        commands[command->getName()] = std::move(command);
    }
}

bool CLIParser::parse(int argc, char** argv) {
    if (argc < 2) {
        return false;
    }
    
    std::string arg = argv[1];
    
    // Check if it's a global option (--help, --version)
    if (arg == "--help" || arg == "-h" || arg == "--version" || arg == "-v") {
        return false; // Global option, handled by main
    }
    
    // Find the command
    if (findCommand(arg)) {
        currentCommandName = arg;
        parsedArguments.clear();
        
        // Parse remaining arguments (skip command name)
        for (int i = 2; i < argc; ++i) {
            parsedArguments.push_back(argv[i]);
        }
        
        return true;
    }
    
    // Command not found
    return false;
}

bool CLIParser::findCommand(const std::string& commandName) {
    return commands.find(commandName) != commands.end();
}

std::string CLIParser::findCommandDescription(const std::string& commandName) const {
    auto it = commands.find(commandName);
    if (it != commands.end()) {
        return it->second->getDescription();
    }
    return "";
}

void CLIParser::showHelpForCommand(const std::string& commandName) {
    auto it = commands.find(commandName);
    if (it != commands.end()) {
        auto& command = it->second;
        std::cout << "Command: " << commandName << std::endl;
        std::cout << "Description: " << command->getDescription() << std::endl;
        std::cout << "Usage: " << command->getUsage() << std::endl;
        
        int requiredArgs = command->getRequiredArgCount();
        if (requiredArgs > 0) {
            std::cout << "Arguments: " << requiredArgs << " required" << std::endl;
        }
        
        std::string optionalHelp = command->getOptionalArgsHelp();
        if (!optionalHelp.empty()) {
            std::cout << "Optional arguments: " << optionalHelp << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << commandName << std::endl;
    }
}

void CLIParser::showHelp() {
    std::cout << "Usage: " << name << " [OPTIONS] <COMMAND> [ARGS]" << std::endl << std::endl;
    
    std::cout << "Available commands:" << std::endl;
    for (const auto& pair : commands) {
        auto& command = pair.second;
        std::cout << "  " << pair.first << " - " << command->getDescription() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Global options:" << std::endl;
    std::cout << "  --help, -h     Show this help message" << std::endl;
    std::cout << "  --version, -v  Show version information" << std::endl;
}

int CLIParser::executeCommand() {
    auto it = commands.find(currentCommandName);
    if (it == commands.end()) {
        std::cerr << "Error: Command not found: " << currentCommandName << std::endl;
        return 1;
    }
    
    auto& command = it->second;
    
    // Validate argument count
    int requiredArgs = command->getRequiredArgCount();
    if (requiredArgs > 0 && static_cast<int>(parsedArguments.size()) < requiredArgs) {
        std::cerr << "Error: Command '" << currentCommandName << "' requires at least "
                  << requiredArgs << " argument(s)" << std::endl;
        return 1;
    }
    
    // Execute the command
    return command->execute(parsedArguments);
}

} // namespace nlm