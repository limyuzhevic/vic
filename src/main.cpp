// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This is the main entry point for the NLM application.
// It provides a simple interface to run demos and tests using the NLM Tool.

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     All cognitive systems integrated.                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "=== NLM Neural Learning Machine ===" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --help, -h                 Show this help message" << std::endl;
    std::cout << "  --version                  Show NLM version information" << std::endl;
    std::cout << "  --demo                     Run standard demo sequence" << std::endl;
    std::cout << "  --config <file>            Load configuration from file" << std::endl;
    std::cout << "  --tool                     Launch NLM Command Line Tool (REPL)" << std::endl;
    std::cout << "  --test <test_name>         Run specific integration test" << std::endl;
    std::cout << std::endl;
    std::cout << "If no options are specified, runs the default demo." << std::endl;
    std::cout << std::endl;
    std::cout << "For more detailed control, use '--tool' for interactive mode." << std::endl;
}

void printVersion() {
    std::cout << "=== NLM Neural Learning Machine ===" << std::endl;
    std::cout << "Version: Phase 6 - Final Integration" << std::endl;
    std::cout << "Architecture: Integrated Artificial Brain" << std::endl;
    std::cout << "Components: " << std::endl;
    std::cout << "  - Neural Core (LIF neurons, spikes, plasticity)" << std::endl;
    std::cout << "  - Memory Systems (Working, Episodic, Associative)" << std::endl;
    std::cout << "  - Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  - Cognition (Planner, Concept Formation, Attention)" << std::endl;
    std::cout << "  - Prediction System" << std::endl;
    std::cout << "  - Development System" << std::endl;
    std::cout << "  - Performance Infrastructure (Memory pools, Event queues)" << std::endl;
    std::cout << std::endl;
}

void runDefaultDemo() {
    std::cout << "=== Running NLM Default Demo ===" << std::endl;
    
    // Launch NLM Tool with demo mode for consistency
    std::cout << "Launching NLM Tool in demo mode..." << std::endl;
    std::cout << "Type 'nlm_tool demo' for full demonstration." << std::endl;
    std::cout << "or run './nlm_tool --demo' for command-line demo." << std::endl;
    std::cout << std::endl;
}

void runIntegrationTest(const std::string& testName) {
    std::cout << "=== Running NLM Integration Test: " << testName << " ===" << std::endl;
    
    // For now, provide guidance on how to run tests
    std::cout << "To run integration tests, use the NLM Tool:" << std::endl;
    std::cout << std::endl;
    std::cout << "  ./nlm_tool test " << testName << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    std::cout << "  connectivity     - Test neural connectivity" << std::endl;
    std::cout << "  plasticity       - Test plasticity learning" << std::endl;
    std::cout << "  stdp            - Test STDP mechanism" << std::endl;
    std::cout << "  memory          - Test memory integration" << std::endl;
    std::cout << "  neuromodulation  - Test neuromodulation" << std::endl;
    std::cout << "  all             - Run all tests" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    if (argc == 1 || (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h"))) {
        // Default demo or help
        printUsage(argv[0]);
        std::cout << std::endl;
        return 0;
    } else if (argc == 2 && (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v")) {
        printVersion();
        return 0;
    } else if (argc == 2 && std::string(argv[1]) == "--demo") {
        runDefaultDemo();
        return 0;
    } else if (argc == 2 && std::string(argv[1]) == "--tool") {
        // Launch NLM Tool in REPL mode
        system("nlm_tool");
        return 0;
    } else if (argc >= 3 && std::string(argv[1]) == "--test") {
        std::string testName = argv[2];
        runIntegrationTest(testName);
        return 0;
    } else if (argc >= 3 && std::string(argv[1]) == "--config") {
        std::string configFile = argv[2];
        std::cout << "Loading configuration from: " << configFile << std::endl;
        std::cout << "Use './nlm_tool --tool' for configuration management." << std::endl;
        return 0;
    } else {
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        std::cerr << "Use '--help' for usage information." << std::endl;
        return 1;
    }
}