// NLM Command Line Interface Main Entry Point
// This is the primary CLI executable that provides advanced command-line
// interface for NLM Neural Learning Machine with comprehensive subcommands,
// JSON output, and extensive configuration options.

#include "cli/NLMCommandLine.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

// NLM version information
#define NLM_VERSION_MAJOR 0
#define NLM_VERSION_MINOR 1
#define NLM_VERSION_PATCH 0

std::string getVersionString() {
    return std::to_string(NLM_VERSION_MAJOR) + "." +
           std::to_string(NLM_VERSION_MINOR) + "." +
           std::to_string(NLM_VERSION_PATCH);
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments using NLMCommandLine
        auto parseResult = nlm::cli::NLMCommandLine::parse(argc, argv);
        
        if (!parseResult.first) {
            // CLI parsing failed, print error and exit
            std::cerr << "[ERROR] " << parseResult.second << std::endl;
            std::cerr << "Use 'nlm --help' for usage information." << std::endl;
            return 1;
        }
        
        // Create and configure NLM CLI instance
        nlm::cli::NLMCommandLine cli;
        
        // Execute the parsed command
        bool success = cli.execute();
        
        if (!success) {
            std::cerr << "[ERROR] Command execution failed" << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        // Handle unexpected exceptions
        std::cerr << "[ERROR] Unexpected error: " << e.what() << std::endl;
        return 2;
    } catch (...) {
        // Handle unknown exceptions
        std::cerr << "[ERROR] Unknown error occurred" << std::endl;
        return 3;
    }
}
