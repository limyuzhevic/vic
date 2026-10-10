// NLM (熙然) - Neural Learning Machine
// Implementation: Command Line Interface
// Provides flexible command-line parsing for NLM tests

#include "tests/BasicConnectivityTest.hpp"
#include "tests/PlasticityTest.hpp"
#include "tests/StdpTest.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm::cli;

CommandLineParser::Options CommandLineParser::parse(int argc, char** argv) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg.substr(0, 9) == "--config=") {
            options.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--test" || arg == "-t") {
            if (i + 1 < argc) {
                std::string testArg = argv[++i];
                if (testArg == "connectivity") options.testModes.push_back("connectivity");
                else if (testArg == "plasticity") options.testModes.push_back("plasticity");
                else if (testArg == "stdp") options.testModes.push_back("stdp");
                else if (testArg == "all") options.testModes.push_back("all");
            }
        } else if (arg == "--test-mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "basic") options.testModes.push_back("basic");
            else if (mode == "learning") options.testModes.push_back("learning");
            else if (mode == "plasticity") options.testModes.push_back("plasticity");
            else if (mode == "stdp") options.testModes.push_back("stdp");
        }
    }
    
    return options;
}

void CommandLineParser::printHelp() {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Usage: nlm [OPTIONS]

Options:
  -h, --help              Show this help message and exit
  -v, --verbose           Enable verbose output
  -t, --test [MODE]       Run specific test (connectivity, plasticity, stdp, all)
      --test-mode MODE     Run test in specified mode (basic, learning, plasticity, stdp)
      --config FILE        Load configuration from FILE (default: configs/default.cfg)

Test Modes:
  connectivity    Run basic connectivity test (spike propagation)
  plasticity      Run plasticity learning experiment
  stdp            Run STDP verification test
  all             Run all tests (default)

Configuration:
  The configuration file should be in JSON format with the following keys:
  - random_seed (int64)
  - simulation_timestep (double)
  - neuron_count (int64)
  - region_count (int64)
  - connection_probability (float)
  - stdp_ltp_weight (float)
  - stdp_ltd_weight (float)
  - stdp_tau (float)
  - synaptogenesis_rate (float)
  - pruning_rate (float)

Examples:
  nlm                          Run all tests with default configuration
  nlm --test connectivity     Run only connectivity test
  nlm --test stdp --verbose    Run STDP test with verbose output
  nlm --config my_config.cfg   Load custom configuration
)" << std::endl;
}
