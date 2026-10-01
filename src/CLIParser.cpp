// CLI Parser for NLM
// Parses command-line arguments and provides options for different execution modes

#include "CLIParser.hpp"
#include <iostream>
#include <algorithm>
#include <cstring>

using namespace nlm;

CLIParser::Options CLIParser::parse(int argc, char** argv) {
    Options options;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                options.mode = argv[++i];
            } else {
                std::cerr << "ERROR: --mode requires an argument" << std::endl;
                options.help = true;
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            } else {
                std::cerr << "ERROR: --config requires an argument" << std::endl;
                options.help = true;
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                try {
                    options.steps = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "ERROR: Invalid value for --steps" << std::endl;
                    options.help = true;
                }
            } else {
                std::cerr << "ERROR: --steps requires an argument" << std::endl;
                options.help = true;
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                try {
                    options.neurons = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "ERROR: Invalid value for --neurons" << std::endl;
                    options.help = true;
                }
            } else {
                std::cerr << "ERROR: --neurons requires an argument" << std::endl;
                options.help = true;
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "ERROR: --output requires an argument" << std::endl;
                options.help = true;
            }
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.verbose = false;
        } else {
            // Check if argument starts with --
            if (arg.substr(0, 2) == "--") {
                std::cerr << "ERROR: Unknown option " << arg << std::endl;
                options.help = true;
            }
            // Positional arguments are ignored for now
        }
    }
    
    // Validate mode
    std::transform(options.mode.begin(), options.mode.end(), options.mode.begin(), ::tolower);
    
    static const std::vector<std::string> validModes = {"phase2", "phase4", "phase6", "benchmark", "integration", "custom"};
    if (std::find(validModes.begin(), validModes.end(), options.mode) == validModes.end()) {
        std::cerr << "ERROR: Invalid mode '" << options.mode << "'. Valid modes: phase2, phase4, phase6, benchmark, integration, custom" << std::endl;
        options.help = true;
    }
    
    return options;
}

void CLIParser::printHelp() {
    std::cout << "NLM - Neural Learning Machine - Command Line Interface" << std::endl;
    std::cout << "====================================================" << std::endl << std::endl;
    
    std::cout << "USAGE: nlm [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  --mode <name>         Execution mode (phase2, phase4, phase6, benchmark, integration, custom)" << std::endl;
    std::cout << "                        Default: phase6" << std::endl << std::endl;
    
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config <file>       Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps <count>       Number of simulation steps" << std::endl;
    std::cout << "  --neurons <count>     Number of neurons (for demos)" << std::endl;
    std::cout << "  --output <file>       Output file for results" << std::endl;
    std::cout << "  --verbose              Enable verbose logging" << std::endl;
    std::cout << "  --quiet                Disable verbose logging" << std::endl;
    std::cout << "  -h, --help             Display this help message" << std::endl << std::endl;
    
    std::cout << "MODES DESCRIPTION:" << std::endl;
    std::cout << "  phase2        Phase 2: Real neural computation (LIF, STDP, Hebbian plasticity)" << std::endl;
    std::cout << "  phase4        Phase 4: Advanced cognition and memory systems" << std::endl;
    std::cout << "  phase6        Phase 6: Complete integration (all systems)" << std::endl;
    std::cout << "  benchmark     Performance benchmarking" << std::endl;
    std::cout << "  integration   Integration testing and verification" << std::endl;
    std::cout << "  custom        Custom experiment configuration" << std::endl << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm --mode phase6                    Run Phase 6 integration demo" << std::endl;
    std::cout << "  nlm --mode phase2 --steps 5000        Run Phase 2 demo with 5000 steps" << std::endl;
    std::cout << "  nlm --mode benchmark --neurons 1000   Benchmark with 1000 neurons" << std::endl;
    std::cout << "  nlm --mode integration --verbose      Run integration tests with verbose output" << std::endl;
    std::cout << "  nlm --help                          Display this help message" << std::endl << std::endl;
    
    std::cout << "For more information, see the documentation in the docs/ directory." << std::endl;
}