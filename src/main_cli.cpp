// NLM Unified CLI
// Replaces multiple demo executables with a single powerful CLI tool

#include "cli/CLI.hpp"
#include "config/CommandLineParser.hpp"
#include "utils/ResultFormatter.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main(int argc, char* argv[]) {
    UnifiedCLI cli;
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Error initializing CLI" << std::endl;
        return 1;
    }
    
    if (!cli.run()) {
        return 1;
    }
    
    return 0;
}