# Advanced command-line argument parsing and validation
// Comprehensive argument handling for NLM with validation and type conversion

#include "CLIAdvanced.hpp"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <vector>

using namespace nlm;

namespace {
    // Trim whitespace from string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    // Convert string to lowercase
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Check if string starts with prefix
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
}

CLIAdvanced::Options CLIAdvanced::parse(int argc, char** argv) {
    Options options;
    
    // Parse positional arguments
    std::vector<std::string> positionalArgs;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (startsWith(arg, "--")) {
            // Long option
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                // --option=value format
                std::string key = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                parseOption(options, key, value);
            } else {
                // Check if next argument is a value
                if (i + 1 < argc && !startsWith(argv[i + 1], "--")) {
                    parseOption(options, arg.substr(2), argv[++i]);
                } else {
                    // Flag option
                    parseFlagOption(options, arg.substr(2));
                }
            }
        } else if (startsWith(arg, "-")) {
            // Short option
            std::string key = arg.substr(1);
            if (key.size() == 1) {
                parseShortOption(options, key[0], (i + 1 < argc && !startsWith(argv[i + 1], "-")) ? argv[++i] : nullptr);
            } else {
                // Multiple short options like -abc
                for (char c : key) {
                    parseShortOption(options, c, (i + 1 < argc && !startsWith(argv[i + 1], "-")) ? argv[++i] : nullptr);
                }
            }
        } else {
            // Positional argument
            positionalArgs.push_back(arg);
        }
    }
    
    // Handle positional arguments
    for (const auto& arg : positionalArgs) {
        if (options.mode.empty() && (arg == "phase2" || arg == "phase4" || arg == "phase6" ||
                                     arg == "benchmark" || arg == "integration" || arg == "custom")) {
            options.mode = arg;
        } else if (options.configFile.empty() && (arg.find(".cfg") != std::string::npos || arg.find(".json") != std::string::npos)) {
            options.configFile = arg;
        } else {
            std::cerr << "WARNING: Ignoring unrecognized positional argument: " << arg << std::endl;
        }
    }
    
    // Validate and set defaults
    validateAndSetDefaults(options);
    
    return options;
}

void CLIAdvanced::parseOption(Options& options, const std::string& key, const std::string& value) {
    std::string normalizedKey = toLower(key);
    
    if (normalizedKey == "mode") {
        options.mode = value;
    } else if (normalizedKey == "config") {
        options.configFile = value;
    } else if (normalizedKey == "steps") {
        try {
            options.steps = std::stoull(value);
        } catch (...) {
            std::cerr << "ERROR: Invalid value for --steps: " << value << std::endl;
            options.help = true;
        }
    } else if (normalizedKey == "neurons") {
        try {
            options.neurons = std::stoull(value);
        } catch (...) {
            std::cerr << "ERROR: Invalid value for --neurons: " << value << std::endl;
            options.help = true;
        }
    } else if (normalizedKey == "output") {
        options.outputFile = value;
    } else if (normalizedKey == "verbose") {
        options.verbose = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "quiet") {
        options.verbose = !(value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "loglevel") {
        options.logLevel = value;
    } else if (normalizedKey == "checkpoint") {
        options.checkpointEnabled = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "replay") {
        options.replayEnabled = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "development") {
        options.developmentEnabled = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "curiosity") {
        options.curiosityEnabled = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "reward-modulation") {
        options.rewardModulationEnabled = (value == "true" || value == "1" || value == "yes");
    } else if (normalizedKey == "structural-plasticity") {
        options.structuralPlasticityEnabled = (value == "true" || value == "1" || value == "yes");
    } else {
        std::cerr << "WARNING: Unknown option: " << key << std::endl;
    }
}

void CLIAdvanced::parseFlagOption(Options& options, const std::string& key) {
    std::string normalizedKey = toLower(key);
    
    if (normalizedKey == "help") {
        options.help = true;
    } else if (normalizedKey == "version") {
        options.showVersion = true;
    } else if (normalizedKey == "verbose") {
        options.verbose = true;
    } else if (normalizedKey == "quiet") {
        options.verbose = false;
    } else if (normalizedKey == "checkpoint") {
        options.checkpointEnabled = true;
    } else if (normalizedKey == "replay") {
        options.replayEnabled = true;
    } else if (normalizedKey == "development") {
        options.developmentEnabled = true;
    } else if (normalizedKey == "curiosity") {
        options.curiosityEnabled = true;
    } else if (normalizedKey == "reward-modulation") {
        options.rewardModulationEnabled = true;
    } else if (normalizedKey == "structural-plasticity") {
        options.structuralPlasticityEnabled = true;
    } else {
        std::cerr << "WARNING: Unknown flag option: " << key << std::endl;
    }
}

void CLIAdvanced::parseShortOption(Options& options, char shortOpt, const char* value) {
    switch (shortOpt) {
        case 'h':
            options.help = true;
            break;
        case 'v':
            options.verbose = true;
            break;
        case 'q':
            options.verbose = false;
            break;
        case 'm':
            if (value) {
                options.mode = value;
            } else {
                std::cerr << "ERROR: -m requires a mode argument" << std::endl;
                options.help = true;
            }
            break;
        case 'c':
            if (value) {
                options.configFile = value;
            } else {
                std::cerr << "ERROR: -c requires a config file argument" << std::endl;
                options.help = true;
            }
            break;
        case 's':
            if (value) {
                try {
                    options.steps = std::stoull(value);
                } catch (...) {
                    std::cerr << "ERROR: Invalid value for -s: " << value << std::endl;
                    options.help = true;
                }
            } else {
                std::cerr << "ERROR: -s requires a steps argument" << std::endl;
                options.help = true;
            }
            break;
        case 'n':
            if (value) {
                try {
                    options.neurons = std::stoull(value);
                } catch (...) {
                    std::cerr << "ERROR: Invalid value for -n: " << value << std::endl;
                    options.help = true;
                }
            } else {
                std::cerr << "ERROR: -n requires a neurons argument" << std::endl;
                options.help = true;
            }
            break;
        case 'o':
            if (value) {
                options.outputFile = value;
            } else {
                std::cerr << "ERROR: -o requires an output file argument" << std::endl;
                options.help = true;
            }
            break;
        default:
            std::cerr << "WARNING: Unknown short option: -" << shortOpt << std::endl;
            break;
    }
}

void CLIAdvanced::validateAndSetDefaults(Options& options) {
    // Validate mode
    if (options.mode.empty()) {
        options.mode = "phase6"; // Default mode
    }
    
    // Normalize mode
    std::transform(options.mode.begin(), options.mode.end(), options.mode.begin(), ::tolower);
    
    // Validate mode
    static const std::vector<std::string> validModes = {"phase2", "phase4", "phase6", "benchmark", "integration", "custom"};
    if (std::find(validModes.begin(), validModes.end(), options.mode) == validModes.end()) {
        std::cerr << "ERROR: Invalid mode: " << options.mode << ". Valid modes: phase2, phase4, phase6, benchmark, integration, custom" << std::endl;
        options.help = true;
    }
    
    // Validate other parameters
    if (options.steps < 10) {
        std::cerr << "WARNING: Very few steps requested (" << options.steps << "). Consider increasing for meaningful results." << std::endl;
    }
    
    if (options.neurons < 10) {
        std::cerr << "WARNING: Very few neurons requested (" << options.neurons << "). Consider increasing for network effects." << std::endl;
    }
    
    // Set defaults based on mode
    if (options.mode == "phase6") {
        if (options.steps == 1000) options.steps = 2000; // Longer for complete integration
        if (options.neurons == 1000) options.neurons = 500; // Smaller for demo
    }
    
    // Enable important subsystems by default for phase6
    if (options.mode == "phase6") {
        options.checkpointEnabled = true;
        options.replayEnabled = true;
        options.developmentEnabled = true;
        options.curiosityEnabled = true;
        options.rewardModulationEnabled = true;
        options.structuralPlasticityEnabled = true;
    }
}

void CLIAdvanced::printHelp() {
    std::cout << "NLM - Neural Learning Machine - Advanced Command Line Interface" << std::endl;
    std::cout << "================================================================" << std::endl << std::endl;
    
    std::cout << "USAGE: nlm [OPTIONS] [POSITIONAL_ARGS]" << std::endl << std::endl;
    
    std::cout << "MODES (choose one):" << std::endl;
    std::cout << "  phase2           Phase 2: Real neural computation (LIF, STDP, Hebbian)" << std::endl;
    std::cout << "  phase4           Phase 4: Advanced cognition and memory systems" << std::endl;
    std::cout << "  phase6           Phase 6: Complete integration (all systems)" << std::endl;
    std::cout << "  benchmark        Performance benchmarking" << std::endl;
    std::cout << "  integration      Integration testing and verification" << std::endl;
    std::cout << "  custom           Custom experiment configuration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "LONG OPTIONS:" << std::endl;
    std::cout << "  --mode <name>                Execution mode (default: phase6)" << std::endl;
    std::cout << "  --config <file>              Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps <count>              Number of simulation steps" << std::endl;
    std::cout << "  --neurons <count>            Number of neurons" << std::endl;
    std::cout << "  --output <file>              Output file for results" << std::endl;
    std::cout << "  --verbose                     Enable verbose logging" << std::endl;
    std::cout << "  --quiet                       Disable verbose logging" << std::endl;
    std::cout << "  --loglevel <level>           Logging level (debug, info, warning, error)" << std::endl;
    std::cout << "  --checkpoint [=bool]          Enable checkpointing" << std::endl;
    std::cout << "  --replay [=bool]              Enable replay capability" << std::endl;
    std::cout << "  --development [=bool]        Enable development system" << std::endl;
    std::cout << "  --curiosity [=bool]           Enable curiosity system" << std::endl;
    std::cout << "  --reward-modulation [=bool]   Enable reward modulation" << std::endl;
    std::cout << "  --structural-plasticity [=bool] Enable structural plasticity" << std::endl;
    std::cout << "  -h, --help                    Display this help message" << std::endl;
    std::cout << "  --version                     Display version information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "SHORT OPTIONS:" << std::endl;
    std::cout << "  -h                           Show help" << std::endl;
    std::cout << "  -v                           Verbose logging" << std::endl;
    std::cout << "  -q                           Quiet mode" << std::endl;
    std::cout << "  -m <mode>                    Set execution mode" << std::endl;
    std::cout << "  -c <config>                  Set configuration file" << std::endl;
    std::cout << "  -s <steps>                   Set number of steps" << std::endl;
    std::cout << "  -n <neurons>                 Set number of neurons" << std::endl;
    std::cout << "  -o <output>                  Set output file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm --mode phase6                      Run Phase 6 integration demo" << std::endl;
    std::cout << "  nlm --mode phase2 --steps 5000          Run Phase 2 demo with 5000 steps" << std::endl;
    std::cout << "  nlm --mode benchmark --neurons 1000     Benchmark with 1000 neurons" << std::endl;
    std::cout << "  nlm --mode integration --verbose         Run integration tests with verbose output" << std::endl;
    std::cout << "  nlm --mode custom --config myconfig.json  Run custom configuration" << std::endl;
    std::cout << "  nlm -m phase6 -s 2000 -v               Run with short options" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ADVANCED EXAMPLES:" << std::endl;
    std::cout << "  nlm --mode phase6 --checkpoint=true --replay=true --development=true" << std::endl;
    std::cout << "  nlm --mode benchmark --steps 10000 --output benchmark_results.csv" << std::endl;
    std::cout << "  nlm --mode custom --neurons 2000 --steps 100 --verbose" << std::endl;
    std::cout << std::endl;
    
    std::cout << "For more information, see the documentation in the docs/ directory." << std::endl;
}

void CLIAdvanced::printVersion() {
    std::cout << "NLM Version 0.1.0" << std::endl;
    std::cout << "Phase 6: Complete Integration" << std::endl;
    std::cout << "Build: " << __DATE__ " " __TIME__ << std::endl;
    std::cout << "Author: NLM Project" << std::endl;
    std::cout << "License: MIT" << std::endl;
    std::cout << std::endl;
    std::cout << "For development information, visit: https://github.com/nlm-project/nlm" << std::endl;
}