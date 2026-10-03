// ==============================================
// NLM Command Line Interface
// ==============================================
//
// This is the unified command-line interface for all NLM demos and tests.
// It provides access to different demo modes with clear separation of concerns.
//
// Usage Examples:
//   ./nlm                            # Default: Phase 6 complete integration test
//   ./nlm --mode integration          # Phase 6 integration test
//   ./nlm --mode quick                # Quick integration verification
//   ./nlm --mode plasticity           # Plasticity learning experiment
//   ./nlm --mode memory              # Memory systems test
//   ./nlm --mode world-interaction   # Phase 3 world interaction (legacy)
//   ./nlm --help                     # Show help message
//
// ==============================================

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace nlm;

enum class DemoMode {
    DEFAULT,
    INTEGRATION,
    QUICK,
    PLASTICITY,
    MEMORY,
    WORLD_INTERACTION,
    HELP
};

DemoMode parseDemoMode(const std::string& modeStr) {
    std::string mode = modeStr;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    if (mode == "integration" || mode == "phase6" || mode == "demo") {
        return DemoMode::INTEGRATION;
    } else if (mode == "quick" || mode == "fast") {
        return DemoMode::QUICK;
    } else if (mode == "plasticity" || mode == "plastic") {
        return DemoMode::PLASTICITY;
    } else if (mode == "memory" || mode == "mem") {
        return DemoMode::MEMORY;
    } else if (mode == "world-interaction" || mode == "world" || mode == "phase3") {
        return DemoMode::WORLD_INTERACTION;
    } else if (mode == "help" || mode == "--help" || mode == "-h") {
        return DemoMode::HELP;
    }
    
    // Default to integration mode for unrecognized modes
    return DemoMode::INTEGRATION;
}

void printWelcomeMessage() {
    std::cout << "===========================================\n";
    std::cout << "NLM - Neural Learning Machine Command Line Interface";
    std::cout << "\n===========================================\n\n";
}

void printWelcomeMessagePhase3() {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
}

void printDemoMessage(DemoMode mode) {
    switch (mode) {
        case DemoMode::INTEGRATION:
            std::cout << "Phase 6 Complete Integration Test\n";
            std::cout << "Testing the fully integrated artificial brain...\n\n";
            break;
        case DemoMode::QUICK:
            std::cout << "Phase 6 Quick Integration Verification\n";
            std::cout << "Fast verification of system integration...\n\n";
            break;
        case DemoMode::PLASTICITY:
            std::cout << "Phase 6 Plasticity Learning Experiment\n";
            std::cout << "Comprehensive learning with all plasticity rules...\n\n";
            break;
        case DemoMode::MEMORY:
            std::cout << "Phase 6 Memory Systems Test\n";
            std::cout << "Testing integrated memory functionality...\n\n";
            break;
        case DemoMode::WORLD_INTERACTION:
            printWelcomeMessagePhase3();
            break;
        case DemoMode::HELP:
            printWelcomeMessage();
            std::cout << "Available demo modes:\n";
            std::cout << "  integration    Phase 6 complete integration test (default)\n";
            std::cout << "  quick         Fast integration verification\n";
            std::cout << "  plasticity     Comprehensive learning experiment\n";
            std::cout << "  memory        Memory systems test\n";
            std::cout << "  world-interaction  Phase 3 world interaction (legacy)\n";
            std::cout << "  help          Show this help message\n\n";
            return;
        case DemoMode::DEFAULT:
            std::cout << "Phase 6 Complete Integration Test (Default)\n";
            std::cout << "Testing the fully integrated artificial brain...\n\n";
            break;
    }
}

DemoMode parseArguments(int argc, char* argv[]) {
    DemoMode mode = DemoMode::DEFAULT;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            mode = DemoMode::HELP;
            break;
        } else if (arg.substr(0, 7) == "--mode=") {
            mode = parseDemoMode(arg.substr(7));
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = parseDemoMode(argv[++i]);
        }
    }
    
    return mode;
}

void showDemoInfo(DemoMode mode) {
    std::cout << "===========================================\n";
    std::cout << "NLM Command Line Interface Demonstration";
    std::cout << "\nMode: " << (mode == DemoMode::INTEGRATION ? "Phase 6 Integration" :
                      mode == DemoMode::QUICK ? "Quick Verification" :
                      mode == DemoMode::PLASTICITY ? "Plasticity Experiment" :
                      mode == DemoMode::MEMORY ? "Memory Test" :
                      mode == DemoMode::WORLD_INTERACTION ? "World Interaction (Legacy)" :
                      "Help") << "\n";
    std::cout << "===========================================\n\n";
}

// Note: This header file contains the command-line interface implementation.
// The actual demo implementations would be in separate .cpp files or integrated
// into the main NLM application. This provides a unified interface for accessing
// different demonstration modes.

// The interface supports:
// 1. Multiple demo modes with clear separation
// 2. Command-line argument parsing
// 3. User-friendly help messages
// 4. Backward compatibility with legacy demos
// 5. Easy extension for new demo modes

// This interface makes it easy for users to access different NLM demos
// and for developers to add new demonstration modes in the future.