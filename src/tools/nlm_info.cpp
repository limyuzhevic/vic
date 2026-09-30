// nlm_info - Display brain status and statistics
// Command-line tool for inspecting NLM brain state

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <iomanip>

using namespace nlm;

void printUsage(const std::string& programName) {
    std::cout << "nlm_info - Display NLM brain status and statistics" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --config <file>             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose output" << std::endl;
    std::cout << "  --format <format>           Output format: text, json, yaml" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                                    Show brain status and statistics" << std::endl;
    std::cout << "  " << programName << " --verbose                           Show detailed information" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg             Use custom configuration" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    std::string format = "text";

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
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
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
            }
        } else if (!arg.starts_with("--")) {
            // Extra argument, ignore or error
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    std::cout << "=== NLM Brain Information ===" << std::endl;
    std::cout << "Loading brain from configuration: " << configFile << std::endl;

    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "✓ Configuration loaded successfully" << std::endl;
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

    if (format == "text") {
        std::cout << "\n--- Brain Status ---" << std::endl;
        brain->logStatus();

        if (verbose) {
            std::cout << "\n--- Configuration Details ---" << std::endl;
            if (config) {
                std::cout << "Random seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
                std::cout << "Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
                std::cout << "Neuron count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
                std::cout << "Region count: " << config->getOr<int64_t>("region_count", 2) << std::endl;
                std::cout << "Connection probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
            }

            std::cout << "\n--- System Information ---" << std::endl;
            std::cout << "Tool version: NLM Phase 6 (Final Integration)" << std::endl;
            std::cout << "Build type: Release" << std::endl;
            std::cout << "Compilation date: " << __DATE__ << " " << __TIME__ << std::endl;
        }
    } else {
        std::cout << "Unsupported format: " << format << std::endl;
        std::cout << "Only 'text' format is currently supported." << std::endl;
        return 1;
    }

    std::cout << "\n=== Brain Information Complete ===" << std::endl;
    return 0;
}
