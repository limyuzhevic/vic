// nlm_load - Load brain state from checkpoint
// Command-line tool for loading NLM brain state

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <filesystem>

using namespace nlm;
namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
    std::cout << "nlm_load - Load NLM brain state from checkpoint" << std::endl;
    std::cout << "Usage: " << programName << " <checkpoint_file> [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  <checkpoint_file>           File path to load brain checkpoint from" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --config <file>             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose output" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " brain.chk                                 Load brain state from file" << std::endl;
    std::cout << "  " << programName << " saved_brain.chk --verbose     Load with verbose output" << std::endl;
    std::cout << "  " << programName << " checkpoint.chk --config custom.cfg Use custom configuration" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string checkpointFile = argv[1];
    std::string configFile = "configs/default.cfg";
    bool verbose = false;

    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (!arg.starts_with("--")) {
            // Extra argument, ignore or error
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Check if checkpoint file exists
    if (!fs::exists(checkpointFile)) {
        std::cerr << "✗ Checkpoint file does not exist: " << checkpointFile << std::endl;
        return 1;
    }

    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    std::cout << "=== Loading NLM Brain State ===" << std::endl;
    std::cout << "Checkpoint file: " << checkpointFile << std::endl;

    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "✓ Configuration loaded from: " << configFile << std::endl;
    } else {
        std::cout << "⚠ Using default configuration" << std::endl;
    }

    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "✗ Failed to initialize brain" << std::endl;
        return 1;
    }

    std::cout << "✓ Brain initialized successfully" << std::endl;

    // Load brain state
    std::cout << "Loading brain state..." << std::endl;
    bool success = brain->load(checkpointFile);

    if (success) {
        std::cout << "✓ Brain state loaded successfully from: " << checkpointFile << std::endl;

        if (verbose) {
            std::cout << "\n--- Loaded Brain Information ---" << std::endl;
            brain->logStatus();
        }
    } else {
        std::cerr << "✗ Failed to load brain state" << std::endl;
        return 1;
    }

    std::cout << "\n=== Load Complete ===" << std::endl;
    return 0;
}
