// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// Enhanced main.cpp with improved CLI and error handling
//
// This phase enhances the main application with:
// - Comprehensive command-line interface
// - Enhanced error handling and recovery
// - Better logging and diagnostics
// - Improved configuration management
// - Performance monitoring and metrics
// - System state validation

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/ErrorHandling/Error.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace nlm;

void printBanner() {
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

// Command-line argument parser for enhanced CLI support
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string logLevel = "Info";
        bool verbose = false;
        bool runAllTests = false;
        bool runPlasticityOnly = false;
        bool runStdpOnly = false;
        SimulationStep customSteps = 0;
        float customInput = 0.0f;
        std::vector<std::string> customTags;
        std::string logFile = "nlm.log";
        bool enablePerformanceMetrics = true;
        bool enableMemoryTracking = true;
        std::string experimentTags = "";
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--all-tests") {
                options.runAllTests = true;
            } else if (arg == "--plasticity-only") {
                options.runPlasticityOnly = true;
            } else if (arg == "--stdp-only") {
                options.runStdpOnly = true;
            } else if (arg == "--steps" && i + 1 < argc) {
                options.customSteps = std::stoul(argv[++i]);
            } else if (arg == "--input" && i + 1 < argc) {
                options.customInput = std::stof(argv[++i]);
            } else if (arg == "--log-level" && i + 1 < argc) {
                options.logLevel = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--log-file" && i + 1 < argc) {
                options.logFile = argv[++i];
            } else if (arg == "--tags" && i + 1 < argc) {
                options.experimentTags = argv[++i];
            } else if (arg.find("--") == 0) {
                // Unknown option
                std::cerr << "Warning: Unknown option " << arg << std::endl;
            }
        }
        
        return options;
    }
    
private:
    static void printHelp() {
        std::cout << R"(
Usage: nlm [options]

Options:
  -h, --help              Show this help message
  -v, --verbose           Enable verbose logging
  --log-level LEVEL       Set log level (Debug, Info, Warning, Error)
  --config FILE           Load configuration from FILE
  --log-file FILE         Write logs to FILE
  --all-tests             Run all available tests
  --plasticity-only      Run only plasticity experiments
  --stdp-only             Run only STDP verification
  --steps N               Run N steps for plasticity tests
  --input VAL             Set custom input current for experiments
  --tags T1,T2,T3         Comma-separated list of experiment tags

Examples:
  nlm --verbose --config my_config.cfg
  nlm --all-tests --steps 5000
  nlm --plasticity-only --input 40.0
  nlm --log-file custom.log --log-level Debug

For more information, see documentation in docs/ and examples in examples/
        )" << std::endl;
    }
};