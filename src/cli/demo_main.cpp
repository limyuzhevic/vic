# NLM CLI Demo Application
# Demonstrates the new CLI interface for NLM Phase 2

#include "cli.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace nlm;

int main(int argc, char** argv) {
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║            NLM Phase 2 CLI Demo - Neural Learning Machine       ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                    Command Line Interface                     ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        // Create parser and executor
        CommandLineParser parser;
        CLIExecutor executor;
        
        // Parse command line arguments
        CLIOptions options;
        if (!parser.parse(argc, argv, options)) {
            std::cerr << "Failed to parse command line arguments." << std::endl;
            return 1;
        }
        
        // Handle special options
        if (options.help) {
            std::cout << parser.getHelp() << std::endl;
            return 0;
        }
        
        if (options.version) {
            std::cout << parser.getVersion() << std::endl;
            return 0;
        }
        
        if (options.listTests) {
            std::cout << "Available Tests:" << std::endl;
            for (const auto& test : parser.getAvailableTests()) {
                std::cout << "  " << test.name << " (" << test.category << ") - " << test.description << std::endl;
            }
            return 0;
        }
        
        if (options.listCategories) {
            std::cout << "Available Categories:" << std::endl;
            for (const auto& category : parser.getAvailableCategories()) {
                std::cout << "  " << category << std::endl;
            }
            return 0;
        }
        
        // Prepare execution
        if (!parser.prepareExecution(options)) {
            std::cerr << "Failed to prepare execution." << std::endl;
            return 1;
        }
        
        // Execute based on mode
        CommandResult result = executor.execute(options);
        
        // Output results based on format
        if (options.outputFormat == "json") {
            std::cout << executor.OutputFormatter::formatJSON(result.output) << std::endl;
        } else if (options.outputFormat == "csv") {
            std::cout << executor.OutputFormatter::formatCSV(result.output) << std::endl;
        } else if (options.outputFormat == "html") {
            std::cout << executor.OutputFormatter::formatHTML(result.output) << std::endl;
        } else {
            std::cout << result.output;
        }
        
        // Handle errors
        if (!result.success) {
            std::cerr << "Error: " << result.errorMessage << std::endl;
            return result.exitCode;
        }
        
        return result.exitCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}