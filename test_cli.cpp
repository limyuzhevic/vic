#include "CLI/CLI.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // Create simple CLI parser for testing
    nlm::CLIArgumentParser parser("test-cli", "Test CLI framework");
    parser.setVersion("1.0.0");
    
    // Add standard arguments
    nlm::GlobalCLI::registerStandardArguments(parser);
    
    // Add custom argument
    parser.addArgument("neurons", "n", "Number of neurons", "100",
                     nlm::CLI::validatePositiveInt, nullptr, false, "int");
    
    // Add flag
    parser.addFlag("verbose", "Enable verbose output", nullptr);
    parser.addFlag("quiet", "Suppress output", nullptr);
    
    // Parse arguments
    if (!parser.parse(argc, argv)) {
        std::cerr << "Failed to parse arguments" << std::endl;
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
    
    // Test getting arguments
    std::cout << "Program: " << parser.getProgramName() << std::endl;
    
    if (parser.hasArgument("neurons")) {
        auto neuronsOpt = parser.getArgument<int>("neurons");
        if (neuronsOpt) {
            std::cout << "Neurons: " << *neuronsOpt << std::endl;
        }
    }
    
    if (parser.hasFlag("verbose")) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    if (parser.hasFlag("quiet")) {
        std::cout << "Quiet mode enabled" << std::endl;
    }
    
    if (parser.hasArgument("config")) {
        auto configOpt = parser.getArgument<std::string>("config");
        if (configOpt) {
            std::cout << "Config file: " << *configOpt << std::endl;
        }
    }
    
    return 0;
}
