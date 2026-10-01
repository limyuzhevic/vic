#include "CLI/CLIApplication.hpp"
#include <iostream>

namespace nlm {

CLIApplication::CLIApplication(const std::string& programName, const std::string& description)
    : parser(programName, description) {
    parser.setVersion("0.1.0");
}

int CLIApplication::run(int argc, char** argv) {
    // Register standard arguments
    registerStandardArguments();
    
    // Parse command line
    if (!parser.parse(argc, argv)) {
        std::cerr << "Error: Failed to parse command line arguments" << std::endl;
        return 1;
    }
    
    // Handle help/version
    if (parser.hasArgument("help") || parser.hasArgument("h")) {
        parser.printHelp();
        return 0;
    }
    
    if (parser.hasArgument("version") || parser.hasArgument("v")) {
        parser.printVersion();
        return 0;
    }
    
    // Initialize application
    if (!initialize()) {
        std::cerr << "Error: Application initialization failed" << std::endl;
        return 1;
    }
    
    // Execute main functionality
    int result = execute();
    
    // Cleanup
    cleanup();
    
    return result;
}

void CLIApplication::registerStandardArguments() {
    // Register standard NLM arguments using the global CLI helper
    nlm::GlobalCLI::registerStandardArguments(parser);
}

void CLIApplication::logInfo(const std::string& message) const {
    std::cout << "[INFO] " << message << std::endl;
}

void CLIApplication::logError(const std::string& message) const {
    std::cerr << "[ERROR] " << message << std::endl;
}

void CLIApplication::logWarning(const std::string& message) const {
    std::cerr << "[WARNING] " << message << std::endl;
}

} // namespace nlm
