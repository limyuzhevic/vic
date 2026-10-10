// NLM Advanced CLI - Main Entry Point
// This is the main CLI interface that integrates all the new functionality

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <algorithm>

// Include the core NLM headers
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "visualization/terminal_renderer.hpp"
#include "tools/debug.hpp"
#include "cli/main_cli.hpp"
#include "cli/cli_parser.hpp"
#include "cli/command_factory.hpp"
#include "cli/status.hpp"
#include "cli/config_command.hpp"
#include "cli/brain_command.hpp"
#include "cli/visualize_command.hpp"
#include "cli/profile_command.hpp"

// Global state for the CLI
struct CLIExecutionState {
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::terminal_renderer> terminalRenderer;
    std::shared_ptr<nlm::debug_tools> debugTools;
    bool brainInitialized = false;
    bool running = true;
    bool debugMode = false;
    bool quietMode = false;
    uint64_t simulationStep = 0;
    double simulationTime = 0.0;
    std::string lastConfigFile;
};

// Global instance
CLIExecutionState globalState;

// Forward declaration
void setupSignalHandlers();
void displayWelcomeMessage();
void displayHelpMessage(const std::string& command = "");
void processInteractiveCommand(const std::string& command);
void runBatchMode(const std::vector<nlm::CLICommand>& commands);

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    globalState.running = false;
    if (!globalState.quietMode) {
        std::cout << "\n[INFO] Received shutdown signal, exiting gracefully..." << std::endl;
    }
}

void setupSignalHandlers() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

void displayWelcomeMessage() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "NLM Advanced Command-Line Interface (v2.0.0)" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "An experimental artificial developmental brain system." << std::endl;
    std::cout << "\nAvailable commands:" << std::endl;
    std::cout << "  config     - Load and validate configuration files" << std::endl;
    std::cout << "  brain      - Brain manipulation and exploration tools" << std::endl;
    std::cout << "  visualize  - Neural network visualization (ASCII/terminal)" << std::endl;
    std::cout << "  profile    - Performance profiling and benchmarking" << std::endl;
    std::cout << "  replay     - Replay saved brain states/checkpoints" << std::endl;
    std::cout << "  explore    - Random exploration and learning discovery" << std::endl;
    std::cout << "  teach      - Manual teaching and demonstration" << std::endl;
    std::cout << "  debug      - Debug and debugging tools" << std::endl;
    std::cout << "\nGlobal options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config=<file>          Load configuration from file" << std::endl;
    std::cout << "  --debug                   Enable debug mode" << std::endl;
    std::cout << "  --quiet                   Suppress output" << std::endl;
    std::cout << "  --batch                   Run in batch mode" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void displayHelpMessage(const std::string& command) {
    if (command.empty()) {
        std::cout << "\n=== Available Commands ===\n" << std::endl;
        auto commands = nlm::CommandFactory::getInstance().listCommands();
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cout << "\n=== Help for command: " << command << " ===\n" << std::endl;
        // TODO: Implement command-specific help
        std::cout << "Command-specific help would be displayed here." << std::endl;
    }
}

void initializeSystem() {
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::NLM_LOG_INFO("Initializing NLM Advanced CLI System...");
    
    // Load configuration
    globalState.config = std::make_shared<nlm::Config>();
    
    // Try to load from config file
    std::string configFile = "configs/config.json";
    globalState.lastConfigFile = configFile;
    
    if (globalState.config->loadFromFileAuto(configFile)) {
        nlm::NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        nlm::NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply defaults and set up brain
    globalState.config->applyDefaults();
    
    // Initialize brain
    globalState.brain = std::make_shared<nlm::Brain>(globalState.config);
    if (globalState.brain->initialize()) {
        globalState.brainInitialized = true;
        nlm::NLM_LOG_INFO("Brain initialized successfully.");
    } else {
        nlm::NLM_LOG_ERROR("Failed to initialize brain!");
    }
    
    // Initialize terminal renderer
    globalState.terminalRenderer = std::make_shared<nlm::terminal_renderer>();
    globalState.terminalRenderer->initialize();
    
    // Initialize debug tools
    globalState.debugTools = std::make_shared<nlm::debug_tools>();
    globalState.debugTools->setLevel(nlm::DebugLevel::Warning);
    
    nlm::NLM_LOG_INFO("NLM Advanced CLI System initialized successfully.");
}

void processInteractiveCommand(const std::string& commandLine) {
    if (commandLine.empty()) return;
    
    // Simple command parsing for now
    if (commandLine == "help") {
        displayHelpMessage();
        return;
    }
    
    if (commandLine == "status") {
        nlm::displayStatus();
        return;
    }
    
    if (commandLine == "config info") {
        // Execute config command with info action
        std::map<std::string, std::string> options;
        nlm::CommandFactory::getInstance().executeCommand("config", options);
        return;
    }
    
    if (commandLine == "brain info") {
        // Execute brain command with info action
        std::map<std::string, std::string> options;
        options["action"] = "info";
        nlm::CommandFactory::getInstance().executeCommand("brain", options);
        return;
    }
    
    if (commandLine == "quit" || commandLine == "exit") {
        globalState.running = false;
        return;
    }
    
    std::cout << "Unknown command: " << commandLine << std::endl;
    std::cout << "Type 'help' for available commands." << std::endl;
}

void runBatchMode(const std::vector<nlm::CLICommand>& commands) {
    std::cout << "Running in batch mode..." << std::endl;
    
    for (const auto& cmd : commands) {
        std::cout << "\nExecuting: " << cmd.toString() << std::endl;
        
        // Execute the command
        nlm::CommandFactory::getInstance().execute(cmd.name, cmd.options);
        
        if (globalState.debugTools) {
            globalState.debugTools->logEvent("Command executed: " + cmd.name);
        }
    }
    
    std::cout << "\nBatch mode completed." << std::endl;
}

int main(int argc, char** argv) {
    // Set up signal handlers
    setupSignalHandlers();
    
    // Parse command line arguments
    nlm::CLIParser parser;
    parser.parse(argc, argv);
    
    // Check for global flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::string command = "";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                command = argv[++i];
            }
            displayHelpMessage(command);
            return 0;
        }
        
        if (arg == "--version") {
            std::cout << "NLM Advanced CLI System v2.0.0" << std::endl;
            return 0;
        }
        
        if (arg == "--debug") {
            globalState.debugMode = true;
            if (globalState.debugTools) {
                globalState.debugTools->setLevel(nlm::DebugLevel::Debug);
            }
        }
        
        if (arg == "--quiet") {
            globalState.quietMode = true;
        }
    }
    
    // Initialize the system
    initializeSystem();
    
    // Set up command factory with commands
    auto& factory = nlm::CommandFactory::getInstance();
    factory.setConfig(globalState.config);
    factory.setBrain(globalState.brain);
    factory.setTerminalRenderer(globalState.terminalRenderer);
    factory.setDebugTools(globalState.debugTools);
    
    // Register commands (simplified for now)
    // In a real implementation, these would be properly registered
    
    if (parser.isBatchMode()) {
        // Run batch mode
        runBatchMode(parser.getCommands());
    } else {
        // Run interactive mode
        displayWelcomeMessage();
        
        std::string command;
        while (globalState.running) {
            std::cout << "nlm> ";
            std::getline(std::cin, command);
            processInteractiveCommand(command);
        }
    }
    
    // Cleanup
    if (globalState.terminalRenderer) {
        globalState.terminalRenderer->close();
    }
    
    nlm::NLM_LOG_INFO("NLM Advanced CLI System shutdown complete.");
    
    return 0;
}

} // namespace nlm

