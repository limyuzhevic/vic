// NLM (熙然) - Neural Learning Machine
// Unified CLI Architecture
//
// This phase implements the unified CLI architecture that replaces multiple
// phase-specific executables with a single 'nlm' command supporting subcommands.
// The new architecture provides:
// - Single entry point with subcommand pattern: nlm <command> [options] [args]
// - Command registration system for extensibility
// - Backward compatibility with existing demo executables
// - Clean separation between CLI interface and core logic

#include "nlm/CliCommand.hpp"
#include "nlm/Subcommands/RunCommand.hpp"
#include "nlm/Subcommands/AnalyzeCommand.hpp"
#include "nlm/Subcommands/ExportCommand.hpp"
#include "nlm/Subcommands/VisualizeCommand.hpp"
#include "nlm/Subcommands/ConfigCommand.hpp"
#include "nlm/Subcommands/ToolsCommand.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

namespace nlm {
    
    // Global initialization function
    void initializeCLI() {
        static bool initialized = false;
        if (!initialized) {
            // Force registration of all commands
            RunCommand runCmd;
            AnalyzeCommand analyzeCmd;
            ExportCommand exportCmd;
            VisualizeCommand visualizeCmd;
            ConfigCommand configCmd;
            ToolsCommand toolsCmd;
            
            initialized = true;
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize the CLI system
    nlm::initializeCLI();
    
    // Check for minimum arguments
    if (argc < 2) {
        CommandRegistry::getInstance().printHelp(std::cout);
        return 0;
    }
    
    // Get the command name
    std::string commandName = argv[1];
    
    // Collect remaining arguments
    std::vector<std::string> args;
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
    
    // Get command from registry
    CommandPtr command = CommandRegistry::getInstance().getCommand(commandName);
    if (!command) {
        std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
        std::cerr << std::endl;
        CommandRegistry::getInstance().printHelp(std::cerr);
        return 1;
    }
    
    // Show command help if requested
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