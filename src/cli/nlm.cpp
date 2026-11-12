// NLM Main CLI Executable
// Unified command-line interface for all NLM subcommands

#include "nlm/CliCommand.hpp"
#include <iostream>
#include <string>

// Forward declarations for subcommand implementations
namespace nlm {
    class RunCommand;
    class AnalyzeCommand;
    class ExportCommand;
    class VisualizeCommand;
    class ConfigCommand;
    class ToolsCommand;
}

// Initialize CLI commands when the library is loaded
void initializeCLICommands() {
    // Force registration of all commands by including the registration file
    #include "nlm/SubcommandsRegistration.cpp"
}

int main(int argc, char* argv[]) {
    // Initialize command registration
    initializeCLICommands();
    
    if (argc < 2) {
        nlm::CommandRegistry::getInstance().printHelp(std::cout);
        return 0;
    }
    
    std::string commandName = argv[1];
    std::vector<std::string> args;
    
    // Collect remaining arguments
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Check for help flag
    bool showHelp = false;
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            break;
        }
    }
    
    // Get the command
    nlm::CommandPtr command = nlm::CommandRegistry::getInstance().getCommand(commandName);
    if (!command) {
        std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
        std::cerr << std::endl;
        nlm::CommandRegistry::getInstance().printHelp(std::cerr);
        return 1;
    }
    
    if (showHelp) {
        std::string help = command->getHelp();
        if (!help.empty()) {
            std::cout << help << std::endl;
        } else {
            std::cout << "Usage: nlm " << commandName << " [options] [args]" << std::endl;
        }
        return 0;
    }
    
    // Execute the command
    return command->execute(args);
}