// NLM Command Line Interface Entry Point

#include "core/CLI/CLIManager.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and initialize CLI manager
    auto cliManager = std::make_unique<CLIManager>();
    
    // Check for command line arguments
    if (argc < 2) {
        std::cout << "No command specified. Use 'nlm help' for available commands." << std::endl;
        return 1;
    }
    
    std::string command = argv[1];
    bool success = cliManager->execute(command, argc, argv);
    
    if (!success) {
        NLM_LOG_ERROR("Command failed: " + command);
        return 1;
    }
    
    return 0;
}
