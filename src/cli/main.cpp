// NLM (熙然) - Neural Learning Machine Advanced CLI System
// Phase 6: Advanced Command-Line Interface Implementation

#include "cli/main_cli.hpp"
#include "cli/cli_parser.hpp"
#include "cli/command_factory.hpp"
#include "cli/status.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "visualization/terminal_renderer.hpp"
#include "tools/debug.hpp"
#include 
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <csignal>
#include <thread>

namespace nlm {

// Global CLI state
struct CLIState {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<terminal_renderer> terminalRenderer;
    std::shared_ptr<debug_tools> debugTools;
    std::string lastConfigFile;
    bool running = true;
    bool debugMode = false;
    bool quietMode = false;
};

CLIState globalState;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    globalState.running = false;
    if (!globalState.quietMode) {
        std::cout << "\nReceived shutdown signal, exiting gracefully..." << std::endl;
    }
}

// Initialize CLI with configuration
bool initializeCLI(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Initializing NLM Advanced CLI System...");
    
    // Load configuration
    globalState.config = std::make_shared<Config>();
    
    // Try to load from config file specified in command line
    std::string configFile = "configs/config.json";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 9) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config with auto-detection
    if (globalState.config->loadFromFileAuto(configFile)) {
        globalState.lastConfigFile = configFile;
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command line overrides
    globalState.config->loadFromArgs(argc, argv);
    globalState.config->applyDefaults();
    
    // Initialize brain from config
    globalState.brain = std::make_shared<Brain>(globalState.config);
    if (!globalState.brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    // Initialize terminal renderer
    globalState.terminalRenderer = std::make_shared<terminal_renderer>();
    globalState.terminalRenderer->initialize();
    
    // Initialize debug tools
    globalState.debugTools = std::make_shared<debug_tools>();
    
    NLM_LOG_INFO("NLM Advanced CLI System initialized successfully.");
    return true;
}

// Display help information
void displayHelp(const std::string& command = "") {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "NLM Advanced Command-Line Interface" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (command.empty()) {
        std::cout << "\nAvailable Commands:" << std::endl;
        std::cout << "  config    - Load and validate configuration files" << std::endl;
        std::cout << "  brain     - Brain manipulation and exploration tools" << std::endl;
        std::cout << "  visualize - Neural network visualization (ASCII/terminal)" << std::endl;
        std::cout << "  profile    - Performance profiling and benchmarking" << std::endl;
        std::cout << "  replay     - Replay saved brain states/checkpoints" << std::endl;
        std::cout << "  explore    - Random exploration and learning discovery" << std::endl;
        std::cout << "  teach      - Manual teaching and demonstration" << std::endl;
        std::cout << "  debug      - Debug and debugging tools" << std::endl;
        std::cout << "\nGlobal Options:" << std::endl;
        std::cout << "  --help                     Show this help message" << std::endl;
        std::cout << "  --version                  Show version information" << std::endl;
        std::cout << "  --config=<file>           Load configuration from file" << std::endl;
        std::cout << "  --debug                    Enable debug mode" << std::endl;
        std::cout << "  --quiet                    Suppress output" << std::endl;
        std::cout << "  --batch                    Run in batch mode" << std::endl;
    } else {
        // Show help for specific command
        std::cout << "\nHelp for command: " << command << std::endl;
        // TODO: Implement command-specific help
        std::cout << "Command-specific help not yet implemented." << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

// Main CLI entry point
int main(int argc, char** argv) {
    // Set up signal handling
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    CLIParser parser;
    parser.parse(argc, argv);
    
    // Check for global help/version flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::string command = "";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                command = argv[++i];
            }
            displayHelp(command);
            return 0;
        } else if (arg == "--version") {
            std::cout << "NLM Advanced CLI System v2.0.0" << std::endl;
            return 0;
        }
    }
    
    // Initialize CLI system
    if (!initializeCLI(argc, argv)) {
        NLM_LOG_ERROR("Failed to initialize CLI system.");
        return 1;
    }
    
    // Process commands in batch mode or interactive mode
    if (parser.isBatchMode()) {
        // Process all commands in batch mode
        while (!parser.commands.empty() && globalState.running) {
            auto cmd = parser.commands.front();
            parser.commands.pop();
            
            if (!globalState.quietMode) {
                std::cout << "Executing: " << cmd.toString() << std::endl;
            }
            
            // TODO: Execute command
            // commandFactory.execute(cmd, globalState);
        }
    } else {
        // Interactive mode
        std::cout << "\nNLM Advanced CLI System started in interactive mode." << std::endl;
        std::cout << "Type 'help' for available commands or 'quit' to exit." << std::endl;
        
        std::string line;
        while (globalState.running) {
            std::cout << "nlm> ";
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            if (line == "quit" || line == "exit") {
                break;
            } else if (line == "help") {
                displayHelp();
            } else if (line == "status") {
                displayStatus();
            } else {
                // Parse and execute command
                // TODO: Parse command line and execute
            }
        }
    }
    
    // Cleanup
    if (globalState.terminalRenderer) {
        globalState.terminalRenderer->close();
    }
    
    NLM_LOG_INFO("NLM Advanced CLI System shutdown complete.");
    return 0;
}

}