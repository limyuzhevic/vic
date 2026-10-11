#pragma once

#include "AdvancedCLI.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

// Original main function with backward compatibility
int mainOriginal(int argc, char* argv[]) {
    // This is a simplified version - actual main would load config, initialize brain, etc.
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // For now, just show help
    printHelp();
    
    return 0;
}

// Advanced CLI main function
int mainAdvanced(int argc, char* argv[]) {
    // Initialize (placeholder - in real code would load config and brain)
    std::cout << "Initializing NLM Advanced CLI...\n" << std::endl;
    
    // Simple parsing for demonstration
    bool showHelp = false;
    bool showVersion = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            showVersion = true;
        }
    }
    
    if (showHelp) {
        printAdvancedHelp();
        return 0;
    }
    
    if (showVersion) {
        printVersion();
        return 0;
    }
    
    // Show available commands
    std::cout << "NLM Advanced CLI is ready.\n";
    std::cout << "Use --advanced <command> --help for detailed command help.\n";
    
    return 0;
}

// Hybrid main function that decides which CLI to run
int main(int argc, char* argv[]) {
    // Check if we should run original or advanced CLI
    bool runAdvanced = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--advanced" || arg == "-a") {
            runAdvanced = true;
            break;
        }
    }
    
    if (runAdvanced) {
        return mainAdvanced(argc, argv);
    } else {
        return mainOriginal(argc, argv);
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Advanced Command-Line Interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM Advanced CLI v2.0 (Phase 6)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic commands:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced commands:" << std::endl;
    std::cout << "  --advanced, -a          Use advanced command mode" << std::endl;
    std::cout << "  <command> <args>        Execute advanced command" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on specific commands, use --advanced <command> --help" << std::endl;
}

void printAdvancedHelp() {
    std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Available command groups:" << std::endl;
    
    // Group 1: Brain Management Commands
    std::cout << "  1. Brain Management:" << std::endl;
    std::cout << "     --brain-config <file>          Load brain configuration" << std::endl;
    std::cout << "     --brain-save <file>            Save brain state to file" << std::endl;
    std::cout << "     --brain-load <file>            Load brain state from file" << std::endl;
    std::cout << "     --brain-info                   Display detailed brain information" << std::endl;
    std::cout << "     --brain-reset                  Reset brain to initial state" << std::endl;
    std::cout << "     --brain-dump <file>            Dump brain state in binary format" << std::endl;
    std::cout << "     --brain-diff <file1> <file2>   Compare two brain states" << std::endl;
    std::cout << "     --brain-status                 Display current brain status" << std::endl;
    std::cout << "     --brain-validate               Validate brain state consistency" << std::endl;
    
    // Group 2: Simulation Control Commands
    std::cout << "  2. Simulation Control:" << std::endl;
    std::cout << "     --simulate <steps>             Run simulation for specified steps" << std::endl;
    std::cout << "     --simulate-continue <steps>     Continue from current state" << std::endl;
    std::cout << "     --simulate-pause                Pause simulation" << std::endl;
    std::cout << "     --simulate-resume               Resume simulation" << std::endl;
    std::cout << "     --simulate-step                 Execute single simulation step" << std::endl;
    std::cout << "     --simulate-rewind <steps>       Rewind simulation" << std::endl;
    std::cout << "     --simulate-fastforward <factor> Run simulation at accelerated speed" << std::endl;
    std::cout << "     --simulate-loop <steps>         Run simulation in loop" << std::endl;
    std::cout << "     --simulate-profile              Profile simulation performance" << std::endl;
    std::cout << "     --simulate-benchmark            Run performance benchmark" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Use --advanced <command> --help for detailed help on specific commands" << std::endl;
}
