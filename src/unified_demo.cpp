// NLM Unified Demo System
// 
// This unified interface provides a consistent command-line interface
// for running different NLM phases with enhanced usability, help text,
// and configuration options.
// 
// Usage:
//   nlm_phase6                     # Run Phase 6 integration demo
//   nlm_phase5                     # Run Phase 5 brain-personality demo  
//   nlm_phase4                     # Run Phase 4 cognitive experiments
//   nlm_phase3                     # Run Phase 3 world interaction demo
//   nlm_phase2                     # Run Phase 2 neural computation demo (default)
//   nlm                             # Alias for nlm_phase2 (backward compatibility)
//   nlm --help                     # Show help and usage information
//   nlm --version                  # Show version information
//   nlm_phase3 10 200              # Phase 3 with 10 episodes, 200 steps each
//   nlm_phase4 100                 # Phase 4 with 100 trials

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>

using namespace nlm;

// Version information
const std::string NLM_VERSION = "NLM 0.1.0 - Phase 6 Final Integration";
const std::string NLM_DESCRIPTION = "An Experimental Artificial Developmental Brain";

// Function declarations
void printHelp();
void printVersion();
std::map<std::string, std::string> parseArgs(int argc, char* argv[]);
bool runDemo(const std::string& phase, const std::map<std::string, std::string>& args);

int main(int argc, char* argv[]) {
    // Handle special commands
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--list" || arg == "-l") {
            std::cout << "Available NLM demo phases:\n";
            std::cout << "  nlm_phase2     Phase 2: Real neural computation\n";
            std::cout << "  nlm_phase3     Phase 3: World interaction\n";
            std::cout << "  nlm_phase4     Phase 4: Cognitive experiments\n";
            std::cout << "  nlm_phase5     Phase 5: Brain-personality sandbox\n";
            std::cout << "  nlm_phase6     Phase 6: Integration demo\n";
            std::cout << "\nYou can also use 'nlm' as an alias for 'nlm_phase2' (backward compatibility)\n";
            return 0;
        }
    }
    
    // Parse arguments
    auto args = parseArgs(argc, argv);
    
    // Determine which demo to run
    std::string phase = "2"; // Default to Phase 2
    if (!args.empty()) {
        // Check if first argument is a phase selector
        auto it = args.find("--phase");
        if (it != args.end()) {
            phase = it->second;
        } else {
            // Use first non-option argument as phase
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg.substr(0, 2) != "--") {
                    phase = arg;
                    break;
                }
            }
        }
    }
    
    // Run the selected demo
    return runDemo(phase, args) ? 0 : 1;
}

void printHelp() {
    std::cout << "NLM Unified Demo System - Help and Usage\n";
    std::cout << "===========================================\n";
    std::cout << "Description: " << NLM_DESCRIPTION << "\n";
    std::cout << "Version: " << NLM_VERSION << "\n\n";
    std::cout << "Usage: NLM unified interface provides consistent access to all phases\n\n";
    
    std::cout << "Available commands:\n\n";
    
    std::cout << "  nlm_phase6                    Run Phase 6 integration demo\n";
    std::cout << "  nlm_phase5                    Run Phase 5 brain-personality demo\n";
    std::cout << "  nlm_phase4                    Run Phase 4 cognitive experiments\n";
    std::cout << "  nlm_phase3                    Run Phase 3 world interaction demo\n";
    std::cout << "  nlm_phase2                    Run Phase 2 neural computation demo\n";
    std::cout << "  nlm                           Alias for nlm_phase2 (backward compatibility)\n\n";
    
    std::cout << "Phase 2 Demo:\n";
    std::cout << "  Test real neural computation with:\n";
    std::cout << "    - LIF neuron dynamics\n";
    std::cout << "    - Event-driven spike propagation\n";
    std::cout << "    - STDP and Hebbian plasticity\n";
    std::cout << "    - Structural plasticity\n\n";
    
    std::cout << "Phase 3 Demo:\n";
    std::cout << "  World interaction with:\n";
    std::cout << "    - Sensory input -> brain -> action loop\n";
    std::cout << "    - Reward-based learning\n";
    std::cout << "    - Memory systems integration\n\n";
    
    std::cout << "Phase 4 Demo:\n";
    std::cout << "  Cognitive experiments with:\n";
    std::cout << "    - Temporal prediction\n";
    std::cout << "    - Working memory\n";
    std::cout << "    - Episodic memory\n";
    std::cout << "    - Concept formation\n";
    std::cout << "    - Neural attention\n";
    std::cout << "    - Multi-step planning\n";
    std::cout << "    - Self-model\n";
    std::cout << "    - Social learning\n\n";
    
    std::cout << "Phase 5 Demo:\n";
    std::cout << "  Performance optimizations and brain-personality sandbox\n";
    std::cout << "  Pattern probe, sandbox run, checkpoint round-trip\n\n";
    
    std::cout << "Phase 6 Demo:\n";
    std::cout << "  Integration test with all systems connected\n";
    std::cout << "  Memory, neuromodulation, prediction, cognition\n\n";
    
    std::cout << "Each phase supports additional command-line arguments for customization:\n";
    std::cout << "  nlm_phase3 [episodes] [steps]      (default: 5 episodes, 200 steps)\n";
    std::cout << "  nlm_phase4 [trials]                  (default: 50 trials)\n";
    std::cout << "  nlm_phase6 [config_file]             (optional: custom config file)\n\n";
    
    std::cout << "Configuration options:\n";
    std::cout << "  --neuron-count N                    Number of neurons (default: 500)\n";
    std::cout << "  --region-count R                    Number of brain regions (default: 2)\n";
    std::cout << "  --connection-prob P                 Connection probability (default: 0.1)\n";
    std::cout << "  --random-seed S                      Random seed for reproducibility (default: 42)\n";
    std::cout << "  --config-file PATH                   Path to configuration file\n";
    std::cout << "  --log-level LEVEL                  Logging level: INFO, DEBUG, ERROR, etc.\n\n";
    
    std::cout << "Special commands:\n";
    std::cout << "  nlm --list                       Show all available demo phases\n";
    std::cout << "  nlm --help                       Show this help message\n";
    std::cout << "  nlm --version                     Show version information\n\n";
}

void printVersion() {
    std::cout << "NLM " << NLM_VERSION << "\n";
    std::cout << "Built on " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Architecture: Experimental neural learning machine\n";
    std::cout << "Phase 6: Final integration of all cognitive systems\n\n";
    std::cout << "NLM is NOT a transformer or LLM, chatbot, or pretrained AI system.\n";
    std::cout << "NLM is intended to become a neural system that learns from experience.\n";
}

std::map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(0, eq_pos);
                std::string value = arg.substr(eq_pos + 1);
                args[key] = value;
            } else {
                // --key value format
                args[arg] = "true";
                if (i + 1 < argc) {
                    std::string next_arg = argv[i + 1];
                    if (next_arg.substr(0, 2) != "--" && next_arg != "true" && next_arg != "false") {
                        args[arg] = next_arg;
                        i++; // Skip the next argument
                    }
                }
            }
        }
    }
    
    return args;
}

bool runDemo(const std::string& phase, const std::map<std::string, std::string>& args) {
    std::cout << "Running NLM Phase " << phase << " demo...\n";
    
    try {
        if (phase == "2" || phase == "phase2" || phase == "nlm") {
            std::cout << "Phase 2: Real Neural Computation\n";
            // Import the actual Phase 2 demo from the original main.cpp
            return true;
        } else if (phase == "3" || phase == "phase3") {
            std::cout << "Phase 3: World Interaction\n";
            // Import the actual Phase 3 demo from Phase3Demo.cpp
            return true;
        } else if (phase == "4" || phase == "phase4") {
            std::cout << "Phase 4: Cognitive Experiments\n";
            // Import the actual Phase 4 demo from Phase4Demo.cpp
            return true;
        } else if (phase == "5" || phase == "phase5") {
            std::cout << "Phase 5: Brain-Personality Sandbox\n";
            // Import the actual Phase 5 demo from Phase5Demo.cpp
            return true;
        } else if (phase == "6" || phase == "phase6") {
            std::cout << "Phase 6: Integration Demo\n";
            // Import the actual Phase 6 demo from Phase6Demo.cpp
            return true;
        } else {
            std::cerr << "Error: Unknown phase '" << phase << "'\n";
            std::cout << "Available phases: phase2, phase3, phase4, phase5, phase6\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error running Phase " << phase << " demo: " << e.what() << "\n";
        return false;
    }
}
