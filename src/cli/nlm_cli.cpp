// NLM Unified CLI Tool
// Main entry point with banner and help system
//
// This provides the unified CLI interface for NLM operations

#include "CLICommand.hpp"
#include "CLIParser.hpp"
#include "HelpSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "BrainCreateCommand.hpp"
#include "BrainInitializeCommand.hpp"
#include "BrainStepCommand.hpp"
#include "BrainSaveCommand.hpp"
#include "BrainLoadCommand.hpp"
#include "BrainResetCommand.hpp"
#include "AgentEnableCommand.hpp"
#include "ConfigShowCommand.hpp"
#include "ConfigSetCommand.hpp"
#include "ConfigValidateCommand.hpp"
#include "ExperimentRunCommand.hpp"
#include "ExperimentStatusCommand.hpp"
#include "MonitorBrainCommand.hpp"
#include "MonitorAgentCommand.hpp"
#include "HelpCommand.hpp"
#include "VersionCommand.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

using namespace nlm;

class NLMCLI {
public:
    NLMCLI() : parser(nullptr), helpSystem(nullptr) {}
    
    ~NLMCLI() {
        delete parser;
        delete helpSystem;
    }
    
    int run(int argc, char** argv) {
        initialize();
        
        if (argc <= 1) {
            showHelp();
            return 0;
        }
        
        return parseAndExecute(argc, argv);
    }
    
private:
    CLIParser* parser;
    HelpSystem* helpSystem;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    
    void initialize() {
        printBanner();
        
        // Initialize help system
        helpSystem = new HelpSystem();
        helpSystem->loadCommandHelp();
        
        // Register commands
        registerCommands();
        
        // Initialize default configuration
        config = std::make_shared<Config>();
        
        std::cout << "NLM CLI initialized. Type 'help' for available commands." << std::endl;
    }
    
void registerCommands() {
        if (!parser) return;
        
        // System commands
        parser->addCommand(std::make_unique<VersionCommand>());
        parser->addCommand(std::make_unique<HelpCommand>());
        
        // Brain commands
        parser->addCommand(std::make_unique<BrainCreateCommand>());
        parser->addCommand(std::make_unique<BrainInitializeCommand>());
        parser->addCommand(std::make_unique<BrainStepCommand>());
        parser->addCommand(std::make_unique<BrainSaveCommand>());
        parser->addCommand(std::make_unique<BrainLoadCommand>());
        parser->addCommand(std::make_unique<BrainResetCommand>());
        
        // Agent commands
        parser->addCommand(std::make_unique<AgentEnableCommand>());
        
        // Config commands
        parser->addCommand(std::make_unique<ConfigShowCommand>());
        parser->addCommand(std::make_unique<ConfigSetCommand>());
        parser->addCommand(std::make_unique<ConfigValidateCommand>());
        
        // Experiment commands
        parser->addCommand(std::make_unique<ExperimentRunCommand>());
        parser->addCommand(std::make_unique<ExperimentStatusCommand>());
        
        // Monitor commands
        parser->addCommand(std::make_unique<MonitorBrainCommand>());
        parser->addCommand(std::make_unique<MonitorAgentCommand>());
        
        // Cast help command to access help system
        auto helpCommands = std::make_unique<HelpCommand>();
        auto* helpPtr = helpCommands.get();
        helpPtr->setHelpSystem(helpSystem);
        parser->addCommand(std::move(helpCommands));
    }
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified Command Line Interface                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
        
        std::cout << "Version 0.1.0 - Phase 1: Core CLI Framework" << std::endl;
        std::cout << "============================================" << std::endl << std::endl;
    }
    
    void showHelp() {
        std::cout << "Usage: nlm [OPTIONS] <COMMAND> [ARGS]" << std::endl << std::endl;
        
        std::cout << "Available commands:" << std::endl;
        std::cout << "  brain    - Brain management commands" << std::endl;
        std::cout << "  agent    - Agent control commands" << std::endl;
        std::cout << "  config   - Configuration management" << std::endl;
        std::cout << "  experiment - Experiment management" << std::endl;
        std::cout << "  monitor  - Real-time monitoring" << std::endl;
        std::cout << "  system   - System commands" << std::endl;
        std::cout << "  help     - Show help information" << std::endl;
        std::cout << "  version  - Show version information" << std::endl << std::endl;
        
        std::cout << "Global options:" << std::endl;
        std::cout << "  --help, -h     Show this help message" << std::endl;
        std::cout << "  --version, -v  Show version information" << std::endl << std::endl;
        
        std::cout << "For command-specific help: nlm help <command>" << std::endl;
    }
    
    int parseAndExecute(int argc, char** argv) {
        if (!parser) {
            std::cerr << "Error: CLI parser not initialized" << std::endl;
            return 1;
        }
        
        // Global option handling
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                parser->showHelp();
                return 0;
            } else if (arg == "--version" || arg == "-v") {
                executeVersionCommand();
                return 0;
            }
        }
        
        // Parse and execute command
        if (parser->parse(argc, argv)) {
            return parser->executeCommand();
        } else {
            std::cerr << "Unknown command: " << (argc > 1 ? argv[1] : "") << std::endl;
            std::cerr << "Type 'nlm help' for available commands." << std::endl;
            return 1;
        }
    }
    
    int executeVersionCommand() {
        std::cout << "NLM CLI Version 0.1.0" << std::endl;
        std::cout << "Phase 1: Core CLI Framework" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        return 0;
    }
    
    int executeExitCommand() {
        std::cout << "Exiting NLM CLI..." << std::endl;
        return 0;
    }
    
    int executeResetCommand() {
        std::cout << "Resetting all brains to default state..." << std::endl;
        // TODO: Implement brain reset functionality
        std::cout << "Reset functionality to be implemented in Phase 2" << std::endl;
        return 0;
    }
};

int main(int argc, char** argv) {
    try {
        NLMCLI nlmCli;
        return nlmCli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}