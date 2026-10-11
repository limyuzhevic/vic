// NLMCLIApp implementation
#include "NLMCLIApp.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <iomanip>

NLMCLIApp::NLMCLIApp() : commandHistory_(100), shouldExit_(false), isInteractive_(false),
                  lineNumber_(1), autoSaveEnabled_(true) {
    logger = &Logger::getGlobal();
    
    // Initialize command parser
    parser = new CommandParser();
    
    // Initialize simulation controller
    controller = new SimulationController();
    
    // Setup command handlers
    setupCommandHandlers();
    
    // Load configuration
    config = std::make_shared<Config>();
    loadConfiguration();
}

NLMCLIApp::~NLMCLIApp() {
    delete parser;
    delete controller;
}

bool NLMCLIApp::initialize() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Initialize simulation controller with configuration
    if (!controller->initialize(config)) {
        logger->error("Failed to initialize simulation controller");
        return false;
    }

    logger->info("NLM CLI Application initialized successfully");
    printWelcomeBanner();
    
    return true;
}

void NLMCLIApp::run() {
    std::cout << "Type 'help' for available commands or 'help <command>' for command details." << std::endl;
    
    if (isInteractive_) {
        runInteractiveMode();
    } else {
        runBatchMode();
    }
}

void NLMCLIApp::runInteractiveMode() {
    std::string line;
    
    while (!shouldExit_) {
        // Display prompt
        std::cout << ">>> ";
        std::cout.flush();
        
        // Read input line
        if (!std::getline(std::cin, line)) {
            // EOF or error
            break;
        }
        
        // Trim leading/trailing whitespace
        line = trim(line);
        
        if (line.empty()) {
            continue;
        }
        
        // Add to history
        commandHistory_.addCommand(line);
        
        // Parse and execute command
        executeCommand(line);
        
        // Display prompt again
        std::cout << std::endl;
    }
}

void NLMCLIApp::runBatchMode() {
    // In batch mode, read from file or stdin
    std::string line;
    
    while (!shouldExit_ && std::getline(std::cin, line)) {
        line = trim(line);
        
        if (line.empty()) {
            continue;
        }
        
        // Parse and execute command
        executeCommand(line);
    }
}

bool NLMCLIApp::executeCommand(const std::string& input) {
    std::string commandName;
    std::vector<std::string> args;
    
    if (!parser->parseCommand(input, commandName, args)) {
        std::cout << "Error: Invalid command format" << std::endl;
        return false;
    }
    
    // Check if command exists
    const CommandParser::Command* cmd = parser->getCommand(commandName);
    if (!cmd) {
        std::cout << "Error: Unknown command: '" << commandName << "'" << std::endl;
        return false;
    }
    
    // Check if simulation is required
    if (cmd->requiresSimulation && !controller->isSimulationRunning()) {
        std::cout << "Error: Command requires a running simulation" << std::endl;
        return false;
    }
    
    // Execute command handler
    try {
        bool result = cmd->handler(args, controller);
        if (!result) {
            std::cout << "Error: Command execution failed" << std::endl;
        }
        return result;
    } catch (const std::exception& e) {
        std::cout << "Error: Command execution exception: " << e.what() << std::endl;
        return false;
    }
}

void NLMCLIApp::displayHelp(const std::string& commandName) {
    parser->displayHelp(commandName);
}

void NLMCLIApp::saveSession(const std::string& sessionName) {
    commandHistory_.saveSession(sessionName);
    logger->info("Session saved: " + sessionName);
}

void NLMCLIApp::restoreSession(const std::string& sessionName) {
    commandHistory_.restoreSession(sessionName);
    logger->info("Session restored: " + sessionName);
}

void NLMCLIApp::clearHistory() {
    commandHistory_.clearSession();
    logger->info("Command history cleared");
}

void NLMCLIApp::showHistory(size_t count) {
    std::vector<std::string> history = commandHistory_.getHistory();
    size_t start = (count > history.size()) ? 0 : history.size() - count;
    
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                Command History                                ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    for (size_t i = start; i < history.size(); ++i) {
        std::cout << "║ " << std::setw(2) << (i + 1) << ": " << history[i] << "..." << std::endl;
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void NLMCLIApp::setAutoSave(bool enabled) {
    autoSaveEnabled_ = enabled;
    if (enabled) {
        logger->info("Auto-save enabled");
    } else {
        logger->info("Auto-save disabled");
    }
}

void NLMCLIApp::printWelcomeBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     A comprehensive CLI for controlling and monitoring         ║
    ║     the NLM brain simulation.                                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════╝
        )" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Welcome to NLM Phase 6 - Final Integration CLI" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    std::cout << std::endl;
}

void NLMCLIApp::loadConfiguration() {
    // Try to load from default config file
    std::string configFile = "configs/nlm_cli.cfg";
    if (config->loadFromFile(configFile)) {
        logger->info("Loaded CLI configuration from: " + configFile);
    } else {
        logger->info("Using default CLI configuration");
    }
}

std::string NLMCLIApp::trim(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

void NLMCLIApp::setupCommandHandlers() {
    // This is a simplified approach - in real implementation, you'd use
    // a more sophisticated registration mechanism
}