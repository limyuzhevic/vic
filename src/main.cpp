// NLM (熙然) - Neural Learning Machine
// Unified Command Line Interface
// Phase 6: Final Integration

#include "core/UnifiedCLI.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // Create and run unified CLI
    nlm::UnifiedCLI cli;
    return cli.run(argc, argv);
}