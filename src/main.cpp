// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced command-line interface with comprehensive configuration options

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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>

using namespace nlm;

// Enhanced command-line argument parsing with comprehensive options
struct CommandLineArgs {
    std::string configFile;
    std::string outputFile;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    uint64_t randomSeed;
    double simulationTimestep;
    bool enableVisualization;
    bool enableDebugLogging;
    bool runExperiments;
    bool saveResults;
    std::string experimentMode;
    std::vector<std::string> customArgs;
    
    CommandLineArgs() : neuronCount(500), regionCount(1), connectionProbability(0.15f),
                       randomSeed(42), simulationTimestep(0.001), enableVisualization(false),
                       enableDebugLogging(false), runExperiments(true), saveResults(false),
                       experimentMode("basic") {}
};

// Enhanced banner with version and features
void printEnhancedBanner() {
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
    ║     - Enhanced configuration system                         ║
    ║     - Comprehensive CLI interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "NLM Neural Learning Machine - Phase 2 Enhanced Edition" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "Real spiking neural computation with advanced plasticity" << std::endl;
    std::cout << "Enhanced configuration management and CLI interface" << std::endl;
}

// Enhanced help/usage information
void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Neural Learning Machine - Command Line Interface" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --config FILE, -c FILE         Load configuration from file (JSON/YAML/simple key=value)" << std::endl;
    std::cout << "  --output FILE, -o FILE         Save results to file" << std::endl;
    std::cout << "  --neurons N, -n N             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions R, -r R              Number of regions (default: 1)" << std::endl;
    std::cout << "  --connection C, -p C           Connection probability (default: 0.15)" << std::endl;
    std::cout << "  --seed S, -s S                 Random seed (default: 42)" << std::endl;
    std::cout << "  --timestep T, -t T             Simulation timestep in ms (default: 0.001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --experiment MODE              Experiment mode: basic, learning, plasticity, stress (default: basic)" << std::endl;
    std::cout << "  --debug, -d                    Enable debug logging" << std::endl;
    std::cout << "  --no-experiments               Disable running experiments" << std::endl;
    std::cout << "  --save-results                 Save experiment results" << std::endl;
    std::cout << "  --visualization, -vis          Enable visualization (if available)" << std::endl;
    std::cout << "  --config-key KEY=VALUE         Set configuration key (can be used multiple times)" << std::endl;
    std::cout << "  --set KEY VALUE                Set configuration key to value (alternative format)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration File Formats:" << std::endl;
    std::cout << "  Simple: key = value" << std::endl;
    std::cout << "  JSON:   {\"key\": \"value\", ...}" << std::endl;
    std::cout << "  YAML:   key: value" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example Usage:" << std::endl;
    std::cout << "  " << programName << " --neurons 1000 --regions 2 --seed 123" << std::endl;
    std::cout << "  " << programName << " --config config.json --output results.txt --experiment learning" << std::endl;
    std::cout << "  " << programName << " -n 2000 -r 3 -p 0.2 -s 456 -d" << std::endl;
    std::cout << "  " << programName << " --config settings.cfg --set stdp_ltp_weight 0.05 --save-results" << std::endl;
}

// Enhanced version information
void printVersion() {
    std::cout << "NLM Neural Learning Machine - Version 2.1.0" << std::endl;
    std::cout << "Phase: Real Neural Computation (Enhanced)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  - Enhanced configuration management" << std::endl;
    std::cout << "  - Comprehensive CLI interface" << std::endl;
    std::cout << "  - Advanced performance profiling" << std::endl;
    std::cout << "  - Detailed visualization capabilities" << std::endl;
}

// Enhanced command-line argument parsing
CommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    CommandLineArgs args;
    
    // Parse arguments using enhanced logic
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Help and version flags
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            exit(0);
        }
        
        // --key=value format
        else if (arg.find("=") != std::string::npos && arg.find("=") > 1) {
            size_t eqPos = arg.find("=");
            std::string key = arg.substr(0, eqPos);
            std::string value = arg.substr(eqPos + 1);
            
            if (key == "--config" || key == "-c") {
                args.configFile = value;
            } else if (key == "--output" || key == "-o") {
                args.outputFile = value;
            } else if (key == "--experiment") {
                args.experimentMode = value;
            } else if (key == "--set") {
                // Store custom argument for later processing
                args.customArgs.push_back(arg);
            } else {
                // Try to parse as configuration key-value
                try {
                    if (key == "--neurons" || key == "-n") {
                        args.neuronCount = std::stoul(value);
                    } else if (key == "--regions" || key == "-r") {
                        args.regionCount = std::stoul(value);
                    } else if (key == "--connection" || key == "-p") {
                        args.connectionProbability = std::stof(value);
                    } else if (key == "--seed" || key == "-s") {
                        args.randomSeed = std::stoull(value);
                    } else if (key == "--timestep" || key == "-t") {
                        args.simulationTimestep = std::stod(value);
                    }
                } catch (...) {
                    // Ignore parsing errors for unknown options
                }
            }
        }
        
        // --key value format (without =)
        else if (arg.find("=") == std::string::npos && arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {
            std::string key = arg;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                
                if (key == "--config") {
                    args.configFile = value;
                } else if (key == "--output") {
                    args.outputFile = value;
                } else if (key == "--experiment") {
                    args.experimentMode = value;
                } else if (key == "--set") {
                    args.customArgs.push_back(key + " " + value);
                }
            } else {
                // Boolean flag
                if (key == "--debug") {
                    args.enableDebugLogging = true;
                } else if (key == "--no-experiments") {
                    args.runExperiments = false;
                } else if (key == "--save-results") {
                    args.saveResults = true;
                } else if (key == "--visualization") {
                    args.enableVisualization = true;
                }
            }
        }
        
        // -key value format
        else if (arg.size() > 1 && arg[0] == '-') {
            char key = arg[1];
            
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                
                if (key == 'n') {
                    args.neuronCount = std::stoul(value);
                } else if (key == 'r') {
                    args.regionCount = std::stoul(value);
                } else if (key == 'p') {
                    args.connectionProbability = std::stof(value);
                } else if (key == 's') {
                    args.randomSeed = std::stoull(value);
                } else if (key == 't') {
                    args.simulationTimestep = std::stod(value);
                }
            } else {
                // Boolean flag
                if (key == 'd') {
                    args.enableDebugLogging = true;
                }
            }
        } else {
            // Positional argument
            args.customArgs.push_back(arg);
        }
    }
    
    return args;
}