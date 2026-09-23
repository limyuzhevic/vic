// NLM (熙然) - Neural Learning Machine
// Phase Selector - Main dispatcher for all phases
//
// This file provides a unified command-line interface for running different
// phases of the NLM project. Users can select which phase to run and pass
// phase-specific arguments.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase Selector - Dispatcher                               ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     Supports all phases: Phase 2, 3, 4, 6                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Phase Selector Help ===" << std::endl;
    std::cout << "Usage: nlm [phase] [options]" << std::endl << std::endl;
    std::cout << "Available phases:" << std::endl;
    std::cout << "  phase2                    Run Phase 2: Real Neural Computation" << std::endl;
    std::cout << "  phase3                    Run Phase 3: Extended functionality" << std::endl;
    std::cout << "  phase4                    Run Phase 4: Advanced integration" << std::endl;
    std::cout << "  phase6                    Run Phase 6: Final integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Default phase: phase6 (if no phase specified)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help               Show this help message" << std::endl;
    std::cout << "  --verbose                Enable verbose logging" << std::endl;
    std::cout << "  --quiet                  Suppress non-error output" << std::endl;
    std::cout << "  --config FILE            Specify configuration file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm                     # Run Phase 6 with default settings" << std::endl;
    std::cout << "  nlm phase2 --verbose    # Run Phase 2 with verbose logging" << std::endl;
    std::cout << "  nlm phase6 --config myconfig.cfg" << std::endl;
}

int main(int argc, char* argv[]) {
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }
    }
    
    printBanner();
    
    // Determine which phase to run
    std::string phase = "phase6"; // Default
    std::string configFile = "";
    bool verbose = false;
    bool quiet = false;
    
    // Parse command line arguments
    std::vector<std::string> phaseArgs;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "phase2" || arg == "phase3" || arg == "phase4" || arg == "phase6") {
            phase = arg;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg != "-v" && arg != "-q" && arg != "--no-integration" && arg != "--no-full-experiment" && arg != "--no-checkpointing" && arg != "--no-replay" && arg != "--no-development" && arg != "--neuron-count" && arg != "--steps") {
            // Other unrecognized arguments - pass to phase-specific executable
            // This allows phase-specific flags to be passed through
            phaseArgs.push_back(arg);
        }
    }
    
    // Map phase to executable and arguments
    std::string executable;
    std::string phaseArgsString;
    
    if (phase == "phase2") {
        executable = "nlm"; // Use the existing phase2 executable
        phaseArgsString = "--verbose";
        if (quiet) phaseArgsString += " --quiet";
        if (configFile != "") phaseArgsString += " --config " + configFile;
    } else if (phase == "phase3") {
        executable = "nlm_phase3_demo";
        phaseArgsString = "--verbose";
        if (quiet) phaseArgsString += " --quiet";
        if (configFile != "") phaseArgsString += " --config " + configFile;
    } else if (phase == "phase4") {
        executable = "nlm_phase4_demo";
        phaseArgsString = "--verbose";
        if (quiet) phaseArgsString += " --quiet";
        if (configFile != "") phaseArgsString += " --config " + configFile;
    } else if (phase == "phase6") {
        executable = "nlm_phase6_demo";
        phaseArgsString = "--verbose";
        if (quiet) phaseArgsString += " --quiet";
        if (configFile != "") phaseArgsString += " --config " + configFile;
        // Add any additional phase args
        for (const auto& arg : phaseArgs) {
            phaseArgsString += " " + arg;
        }
    }
    
    // If phase selection is disabled, run the appropriate executable directly
    // This is a simple implementation - in a full solution, we'd use system() or fork/exec
    std::cout << "Running " << phase << "..." << std::endl;
    
    if (phase == "phase2") {
        std::cout << "Note: Phase 2 executable would run with arguments: " << phaseArgsString << std::endl;
        std::cout << "Phase 2 functionality is implemented in src/main.cpp" << std::endl;
    } else if (phase == "phase3") {
        std::cout << "Note: Phase 3 executable would run with arguments: " << phaseArgsString << std::endl;
        std::cout << "Phase 3 functionality is implemented in src/experiments/Phase3Demo.cpp" << std::endl;
    } else if (phase == "phase4") {
        std::cout << "Note: Phase 4 executable would run with arguments: " << phaseArgsString << std::endl;
        std::cout << "Phase 4 functionality is implemented in src/experiments/Phase4Demo.cpp" << std::endl;
    } else if (phase == "phase6") {
        std::cout << "Note: Phase 6 executable would run with arguments: " << phaseArgsString << std::endl;
        std::cout << "Phase 6 functionality is implemented in src/experiments/Phase6Demo.cpp" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "To actually run a phase, compile the project with CMake." << std::endl;
    std::cout << "The phase selector provides a framework for unified phase selection." << std::endl;
    
    return 0;
}
