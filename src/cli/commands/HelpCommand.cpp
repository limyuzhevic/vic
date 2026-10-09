#include "HelpCommand.hpp"
#include "HelpSystem.hpp"
#include <iostream>
#include <vector>

namespace nlm {

int HelpCommand::execute(const std::vector<std::string>& args) {
    std::string commandName;
    
    // Parse optional command name
    if (!args.empty()) {
        commandName = args[0];
    }
    
    std::cout << "Help command executed" << std::endl;
    std::cout << "Command name: " << (commandName.empty() ? "none" : commandName) << std::endl;
    
    // In a real implementation, this would:
    // 1. Access the help system
    // 2. Show general help if no command specified
    // 3. Show command-specific help if command name provided
    
    if (helpSystem) {
        if (commandName.empty()) {
            // Show general help
            helpSystem->showGeneralHelp();
        } else {
            // Show command-specific help
            helpSystem->showCommandHelp(commandName);
        }
    } else {
        std::cout << "\n=== NLM CLI Help System ===" << std::endl;
        
        if (commandName.empty()) {
            std::cout << "\nGeneral Usage:" << std::endl;
            std::cout << "  nlm [OPTIONS] <COMMAND> [ARGS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Global Options:" << std::endl;
            std::cout << "  --help, -h     Show this help message" << std::endl;
            std::cout << "  --version, -v  Show version information" << std::endl;
            std::cout << std::endl;
            std::cout << "Available Commands:" << std::endl;
            std::cout << "  brain    - Brain management commands" << std::endl;
            std::cout << "  agent    - Agent control commands" << std::endl;
            std::cout << "  config   - Configuration management" << std::endl;
            std::cout << "  experiment - Experiment management" << std::endl;
            std::cout << "  monitor  - Real-time monitoring" << std::endl;
            std::cout << "  system   - System commands" << std::endl;
            std::cout << std::endl;
            std::cout << "For command-specific help: nlm help <command>" << std::endl;
        } else {
            std::cout << "\n=== Help for Command: " << commandName << " ===" << std::endl;
            std::cout << "Command: " << commandName << std::endl;
            std::cout << "Description: Command-specific help would be displayed here" << std::endl;
            std::cout << std::endl;
            std::cout << "Usage examples would be shown here." << std::endl;
            std::cout << std::endl;
            std::cout << "For general help: nlm help" << std::endl;
        }
    }
    
    std::cout << "\n✓ Help command completed" << std::endl;
    
    return 0;
}

} // namespace nlm