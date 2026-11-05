#include "../core/CLI/CLIParser.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <cctype>

// CLI Argument Parser Implementation

CLIParser::CLIParser(int argc, char** argv) : argc_(argc), argv_(argv) {}

CLIOptions CLIParser::parse() {
    CLIOptions options;
    
    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (arg == "--config" || arg == "-c") {
            options.configFile = argv_[++i];
        } else if (arg == "--steps" || arg == "-s") {
            options.simulationSteps = std::stoull(argv_[++i]);
        } else if (arg == "--neurons" || arg == "-n") {
            options.neuronCount = std::stoull(argv_[++i]);
        } else if (arg == "--regions" || arg == "-r") {
            options.regionCount = std::stoull(argv_[++i]);
        } else if (arg == "--connection-prob" || arg == "-cp") {
            options.connectionProbability = std::stof(argv_[++i]);
        } else if (arg == "--run-tests") {
            options.runTests = true;
        } else if (arg == "--benchmark") {
            options.runBenchmark = true;
        } else if (arg == "--phase3") {
            options.runPhase3Demo = true;
        } else if (arg == "--phase4") {
            options.runPhase4Demo = true;
        } else if (arg == "--phase5") {
            options.runPhase5Demo = true;
        } else if (arg == "--phase6") {
            options.runPhase6Demo = true;
        } else if (arg == "--log-level") {
            options.logLevel = argv_[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--timestep") {
            options.timestep = std::stod(argv_[++i]);
        } else if (arg == "--help-file") {
            if (++i < argc_) {
                loadOptionsFromFile(argv_[i], options);
            }
        } else if (arg == "--save-config") {
            if (++i < argc_) {
                saveOptionsToFile(argv_[i], options);
            }
        } else {
            // Try to parse as key=value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                parseKeyValueOption(key, value, options);
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
                std::cerr << "Use --help for usage information." << std::endl;
                exit(1);
            }
        }
    }
    
    return options;
}

void CLIParser::parseKeyValueOption(const std::string& key, const std::string& value, CLIOptions& options) {
    if (key == "stdp_ltp_weight") {
        options.stdpLTPWeight = std::stof(value);
    } else if (key == "stdp_ltd_weight") {
        options.stdpLTDWeight = std::stof(value);
    } else if (key == "stdp_tau") {
        options.stdpTau = std::stof(value);
    } else if (key == "synaptogenesis_rate") {
        options.synaptogenesisRate = std::stof(value);
    } else if (key == "pruning_rate") {
        options.pruningRate = std::stof(value);
    } else if (key == "enable_debug") {
        options.enableDebug = (value == "true" || value == "1" || value == "yes");
    } else if (key == "profile") {
        options.profile = (value == "true" || value == "1" || value == "yes");
    } else if (key == "checkpoint_interval") {
        options.checkpointInterval = std::stoull(value);
    } else if (key == "checkpoint_path") {
        options.checkpointPath = value;
    } else if (key == "use_gpu") {
        options.useGPU = (value == "true" || value == "1" || value == "yes");
    } else if (key == "max_memory_gb") {
        options.maxMemoryGB = std::stoul(value);
    } else {
        std::cerr << "Unknown configuration key: " << key << std::endl;
        std::cerr << "Ignoring." << std::endl;
    }
}

std::string CLIParser::getHelpText() {
    return R"(
NLM — Neural Learning Machine

USAGE:
  nlm [OPTIONS] [KEY=VALUE ...]

OPTIONS:
  --help, -h                    Show this help message
  --version                      Show version information
  --config, -c FILE             Load configuration from FILE
  --steps, -s STEPS             Number of simulation steps (default: 1000)
  --neurons, -n NEURONS         Number of neurons (default: 500)
  --regions, -r REGIONS         Number of regions (default: 1)
  --connection-prob, -cp PROB   Connection probability (default: 0.15)
  --run-tests                    Run all tests
  --benchmark                    Run performance benchmark
  --phase3                       Run Phase 3 demo
  --phase4                       Run Phase 4 demo
  --phase5                       Run Phase 5 demo
  --phase6                       Run Phase 6 demo
  --log-level LEVEL              Set logging level (debug, info, warning, error, critical)
  --verbose, -v                  Enable verbose output
  --quiet, -q                    Suppress non-essential output
  --timestep TS                  Simulation timestep in seconds (default: 0.001)
  --help-file FILE               Load additional options from FILE
  --save-config FILE             Save current options to FILE
  --enable-debug                 Enable debug mode
  --profile                       Enable profiling
  --checkpoint-interval INTERVAL  Checkpoint interval in steps (default: 100)
  --checkpoint-path PATH         Checkpoint file path

KEY=VALUE OPTIONS:
  stdp_ltp_weight=VALUE           STDP LTP weight
  stdp_ltd_weight=VALUE          STDP LTD weight
  stdp_tau=VALUE                 STDP time constant
  synaptogenesis_rate=VALUE      Synaptogenesis rate
  pruning_rate=VALUE             Pruning rate
  enable_debug=BOOLEAN           Enable debug mode (true/false)
  profile=BOOLEAN                Enable profiling (true/false)
  checkpoint_interval=VALUE      Checkpoint interval
  checkpoint_path=PATH           Checkpoint file path
  use_gpu=BOOLEAN                Enable GPU acceleration (true/false)
  max_memory_gb=VALUE            Maximum memory in GB

EXAMPLES:
  nlm --neurons 1000 --steps 5000
  nlm --phase6 --verbose
  nlm --config myconfig.cfg stdp_ltp_weight=0.03
  nlm --save-config myconfig.cfg

CONFIGURATION:
  Configuration can be loaded from files using --config option.
  Additional options can be specified as KEY=VALUE pairs.
  Options from files are overridden by command line arguments.

RUNNING TESTS:
  Use --run-tests to run the full test suite.
  Individual tests can be run by specifying the demo with --phaseX.

PERFORMANCE:
  Use --benchmark to measure performance with different network sizes.
  Use --profile for detailed performance profiling.
  --enable-debug provides additional debugging output.

For more information, visit: https://github.com/nlm-project/nlm
)";
}

std::string CLIParser::getVersion() {
    return "NLM Phase 2 - Build v0.1.0 (2026-10-31)";
}

bool CLIParser::validate(const CLIOptions& options, std::string& errorMessage) {
    if (options.neuronCount == 0) {
        errorMessage = "Neuron count must be greater than 0";
        return false;
    }
    
    if (options.simulationSteps == 0) {
        errorMessage = "Simulation steps must be greater than 0";
        return false;
    }
    
    if (options.connectionProbability < 0.0f || options.connectionProbability > 1.0f) {
        errorMessage = "Connection probability must be between 0.0 and 1.0";
        return false;
    }
    
    if (options.stdpLTPWeight < 0.0f || options.stdpLTPWeight > 1.0f) {
        errorMessage = "STDP LTP weight must be between 0.0 and 1.0";
        return false;
    }
    
    if (options.stdpLTDWeight < 0.0f || options.stdpLTDWeight > 1.0f) {
        errorMessage = "STDP LTD weight must be between 0.0 and 1.0";
        return false;
    }
    
    if (options.timestep <= 0.0) {
        errorMessage = "Timestep must be positive";
        return false;
    }
    
    if (options.maxMemoryGB == 0) {
        errorMessage = "Maximum memory must be greater than 0 GB";
        return false;
    }
    
    return true;
}

bool CLIParser::loadOptionsFromFile(const std::string& filepath, CLIOptions& options) {
    // Simple TOML-like configuration file parsing
    // This is a basic implementation - could be enhanced with proper TOML parsing
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Parse based on key type
            if (key == "neuron_count") {
                options.neuronCount = std::stoull(value);
            } else if (key == "simulation_steps") {
                options.simulationSteps = std::stoull(value);
            } else if (key == "region_count") {
                options.regionCount = std::stoull(value);
            } else if (key == "connection_probability") {
                options.connectionProbability = std::stof(value);
            } else if (key == "stdp_ltp_weight") {
                options.stdpLTPWeight = std::stof(value);
            } else if (key == "stdp_ltd_weight") {
                options.stdpLTDWeight = std::stof(value);
            } else if (key == "stdp_tau") {
                options.stdpTau = std::stof(value);
            } else if (key == "synaptogenesis_rate") {
                options.synaptogenesisRate = std::stof(value);
            } else if (key == "pruning_rate") {
                options.pruningRate = std::stof(value);
            } else if (key == "timestep") {
                options.timestep = std::stod(value);
            } else if (key == "log_level") {
                options.logLevel = value;
            } else if (key == "verbose") {
                options.verbose = (value == "true" || value == "1" || value == "yes");
            } else if (key == "quiet") {
                options.quiet = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_debug") {
                options.enableDebug = (value == "true" || value == "1" || value == "yes");
            } else if (key == "profile") {
                options.profile = (value == "true" || value == "1" || value == "yes");
            } else if (key == "checkpoint_interval") {
                options.checkpointInterval = std::stoull(value);
            } else if (key == "checkpoint_path") {
                options.checkpointPath = value;
            } else if (key == "use_gpu") {
                options.useGPU = (value == "true" || value == "1" || value == "yes");
            } else if (key == "max_memory_gb") {
                options.maxMemoryGB = std::stoul(value);
            }
        }
    }
    
    file.close();
    return true;
}

bool CLIParser::saveOptionsToFile(const std::string& filepath, const CLIOptions& options) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM CLI parser" << std::endl;
    file << std::endl;
    file << "[simulation]" << std::endl;
    file << "neuron_count = " << options.neuronCount << std::endl;
    file << "simulation_steps = " << options.simulationSteps << std::endl;
    file << "region_count = " << options.regionCount << std::endl;
    file << "connection_probability = " << options.connectionProbability << std::endl;
    file << "timestep = " << options.timestep << std::endl;
    file << std::endl;
    file << "[plasticity]" << std::endl;
    file << "stdp_ltp_weight = " << options.stdpLTPWeight << std::endl;
    file << "stdp_ltd_weight = " << options.stdpLTDWeight << std::endl;
    file << "stdp_tau = " << options.stdpTau << std::endl;
    file << std::endl;
    file << "[structural_plasticity]" << std::endl;
    file << "synaptogenesis_rate = " << options.synaptogenesisRate << std::endl;
    file << "pruning_rate = " << options.pruningRate << std::endl;
    file << std::endl;
    file << "[logging]" << std::endl;
    file << "log_level = " << options.logLevel << std::endl;
    file << "verbose = " << (options.verbose ? "true" : "false") << std::endl;
    file << "quiet = " << (options.quiet ? "true" : "false") << std::endl;
    file << std::endl;
    file << "[debug]" << std::endl;
    file << "enable_debug = " << (options.enableDebug ? "true" : "false") << std::endl;
    file << "profile = " << (options.profile ? "true" : "false") << std::endl;
    file << "checkpoint_interval = " << options.checkpointInterval << std::endl;
    if (!options.checkpointPath.empty()) {
        file << "checkpoint_path = \"" << options.checkpointPath << "\"" << std::endl;
    }
    file << std::endl;
    file << "[advanced]" << std::endl;
    file << "use_gpu = " << (options.useGPU ? "true" : "false") << std::endl;
    file << "max_memory_gb = " << options.maxMemoryGB << std::endl;
    
    file.close();
    return true;
}

LogLevel CLIParser::stringToLogLevel(const std::string& level) {
    std::string lowerLevel = toLower(level);
    if (lowerLevel == "debug") {
        return LogLevel::Debug;
    } else if (lowerLevel == "info") {
        return LogLevel::Info;
    } else if (lowerLevel == "warning") {
        return LogLevel::Warning;
    } else if (lowerLevel == "error") {
        return LogLevel::Error;
    } else if (lowerLevel == "critical") {
        return LogLevel::Critical;
    }
    return LogLevel::Info; // Default
}

std::string CLIParser::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "debug";
        case LogLevel::Info: return "info";
        case LogLevel::Warning: return "warning";
        case LogLevel::Error: return "error";
        case LogLevel::Critical: return "critical";
        default: return "info";
    }
}

std::string CLIParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CLIParser::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}