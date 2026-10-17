// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This is the updated main entry point for NLM Phase 2.
// The new implementation uses a comprehensive CLI system for better
// modularity, extensibility, and user experience.

#include "cli/CLI.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

// Simple function to display original banner for backward compatibility
void printOriginalBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    // Print original banner for backward compatibility
    printOriginalBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Run the new CLI system
    return runCLI(argc, argv);
}
