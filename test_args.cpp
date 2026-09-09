#include "Args.hpp"

int main(int argc, char** argv) {
    nlm::ArgumentParser parser;
    
    // Test parsing
    if (!parser.parse(argc, argv)) {
        std::cerr << "Error: Failed to parse command line arguments" << std::endl;
        return 1;
    }
    
    // Test help
    if (parser.wasHelpRequested()) {
        std::cout << parser.generateHelp() << std::endl;
        return 0;
    }
    
    // Test version
    if (parser.wasVersionRequested()) {
        std::cout << "NLM " << nlm::ArgumentParser::generateVersion() << std::endl;
        return 0;
    }
    
    // Test mode
    try {
        nlm::ExecutionMode mode = parser.getMode();
        std::cout << "Mode: " << nlm::executionModeToString(mode) << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Test flags
    try {
        if (parser.getFlag("verbose")) {
            std::cout << "Verbose mode enabled" << std::endl;
        }
        if (parser.getFlag("quiet")) {
            std::cout << "Quiet mode enabled" << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Test values
    try {
        std::string configFile = parser.getValue("config");
        std::cout << "Config file: " << configFile << std::endl;
        
        std::string outputFile = parser.getValue("output");
        std::cout << "Output file: " << outputFile << std::endl;
    } catch (const std::runtime_error& e) {
        // These options might not be set, so that's OK
    }
    
    // Test all args
    auto allArgs = parser.getAllArgs();
    std::cout << "All arguments:" << std::endl;
    for (const auto& pair : allArgs) {
        std::cout << "  " << pair.first << ": ";
        if (std::holds_alternative<std::string>(pair.second)) {
            std::cout << std::get<std::string>(pair.second);
        } else if (std::holds_alternative<bool>(pair.second)) {
            std::cout << (std::get<bool>(pair.second) ? "true" : "false");
        } else if (std::holds_alternative<nlm::ExecutionMode>(pair.second)) {
            std::cout << nlm::executionModeToString(std::get<nlm::ExecutionMode>(pair.second));
        }
        std::cout << std::endl;
    }
    
    return 0;
}