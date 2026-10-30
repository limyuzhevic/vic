// NLM - Neural Learning Machine
// Phase 6: Final Integration with Command Line Interface
//
// This is the unified main executable that provides access to all NLM modes
// through a modern command-line interface.

#include "core/CLI.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

int main(int argc, char** argv) {
    // Create the unified CLI instance
    NLMCLI cli;
    
    // Run with command line arguments
    int result = cli.run(argc, argv);
    
    return result;
}