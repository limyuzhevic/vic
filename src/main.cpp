#include "cli/CommandLineInterface.hpp"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    try {
        // Create command line interface
        auto cli = std::make_unique<nlm::CommandLineInterface>();
        
        // Parse command line arguments
        if (!cli->parseArguments(argc, argv)) {
            std::cerr << "Error parsing command line arguments." << std::endl;
            return 1;
        }
        
        // Handle help/version requests
        if (cli->hasOption("--help") || cli->hasOption("-h")) {
            cli->printHelp();
            return 0;
        }
        
        if (cli->hasOption("--version")) {
            cli->printVersion();
            return 0;
        }
        
        if (cli->hasOption("--benchmark")) {
            // Run benchmark mode
            std::cout << "Running benchmark..." << std::endl;
            return cli->executeSimulation();
        }
        
        // Print configuration summary if verbose
        if (cli->getFlag("--verbose") || cli->getFlag("--debug")) {
            cli->printConfigSummary();
        }
        
        // Execute simulation with current configuration
        return cli->executeSimulation();
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred." << std::endl;
        return 1;
    }
}