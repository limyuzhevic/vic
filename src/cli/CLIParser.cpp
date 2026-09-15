#include "cli/CLIParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstring>
#include <cmath>
#include <thread>

namespace nlm {

CLIParser::CLIParser() {
    // Initialize built-in arguments
    builtinArguments = {
        {"help", "h", "Show help information", "false", false, false, "bool"},
        {"version", "v", "Show version information", "false", false, false, "bool"},
        {"config", "c", "Configuration file path", "configs/default.cfg", false, true, "string"},
        {"log-level", "l", "Set log level (debug, info, warning, error, critical)", "info", false, true, "string"},
        {"log-file", "", "Log output file", "", false, true, "string"},
        {"verbose", "", "Enable verbose output", "false", false, false, "bool"},
        {"quiet", "", "Suppress output", "false", false, false, "bool"},
        {"debug", "", "Enable debug mode with detailed output", "false", false, false, "bool"},
        {"json-output", "", "Output in JSON format", "false", false, false, "bool"},
        {"yaml-output", "", "Output in YAML format", "false", false, false, "bool"},
        {"performance-mode", "", "Performance monitoring mode (0=basic,1=detailed,2=full)", "0", false, true, "int"},
        {"max-memory", "", "Maximum memory usage in MB", "0", false, true, "int"},
        {"timeout", "", "Execution timeout in seconds", "0", false, true, "double"},
        {"experiment", "e", "Experiment name", "", false, true, "string"},
        {"plugin", "p", "Load plugin", "", false, true, "string"},
        {"output-format", "o", "Output format (console, json, yaml, human)", "console", false, true, "string"}
    };
    
    helpText = """
NLM (熙然) - Neural Learning Machine
Phase 2: Real Neural Computation

USAGE:
    nlm [OPTIONS] [CONFIG_ARGS...]

DESCRIPTION:
    An experimental artificial developmental brain implementing:
    - Leaky Integrate-and-Fire (LIF) neuron dynamics
    - Event-driven spike propagation with synaptic delays
    - STDP and Hebbian plasticity rules
    - Structural plasticity (synaptogenesis/pruning)

OPTIONS:
    -h, --help                  Show this help message
    -v, --version               Show version information
    -c, --config FILE          Load configuration from FILE
    -l, --log-level LEVEL      Set log level (debug, info, warning, error, critical)
    --log-file FILE            Log output to FILE (in addition to console)
    --verbose                  Enable verbose output
    --quiet                    Suppress non-essential output
    --debug                    Enable debug mode with detailed internal state
    --json-output              Output results in JSON format
    --yaml-output              Output results in YAML format
    --performance-mode MODE     Performance monitoring: 0=basic, 1=detailed, 2=full
    --max-memory MB            Maximum memory usage in MB (0=unlimited)
    --timeout SECONDS          Execution timeout in seconds (0=unlimited)
    -e, --experiment NAME      Set experiment name
    -p, --plugin NAME          Load plugin (can be specified multiple times)
    -o, --output-format FORMAT  Output format (console, json, yaml, human)

CONFIGURATION:
    Configuration values can be set via command line:
        --key=value          Set numeric/string boolean configuration
        -k value             Set short format configuration

    Example:
        nlm --config myconfig.cfg --neuron_count=1000 --stdp_ltp_weight=0.03

PERFORMANCE MONITORING:
    The performance monitor tracks execution time and memory usage.
    Use --performance-mode to control detail level.
    Output is available in JSON, YAML, or human-readable format.

DEBUGGING:
    --debug enables detailed debugging information including:
    - Internal brain state snapshots
    - Neuron membrane potential history
    - Synapse weight evolution
    - Network connectivity statistics

PLUGINS:
    Plugins can extend NLM functionality. Use --plugin NAME to load.
""";

    versionText = "NLM 2.0.0 (Phase 2: Real Neural Computation)";
}

bool CLIParser::parse(int argc, char** argv, CLIOptions& options) {
    // Initialize options with defaults
    options = CLIOptions();
    options.logLevel = "info";
    options.outputFormat = "console";
    
    errorMessage.clear();
    
    // Process arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Skip empty arguments
        if (arg.empty()) continue;
        
        // Check for help/version flags
        if (arg == "-h" || arg == "--help") {
            options.help = true;
            continue;
        }
        
        if (arg == "-v" || arg == "--version") {
            options.version = true;
            continue;
        }
        
        // Check for options with values
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setOptionFromString(key, value, options);
            } else {
                // --key value format
                std::string key = arg.substr(2);
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    setOptionFromString(key, value, options);
                } else {
                    errorMessage = "Option --" + key + " requires a value";
                    return false;
                }
            }
        } else if (arg.substr(0, 1) == "-") {
            // Check for short options with values
            bool found = false;
            for (const auto& argDef : builtinArguments) {
                if (argDef.shortName == arg) {
                    if (i + 1 < argc) {
                        setOptionFromString(argDef.name, argv[++i], options);
                        found = true;
                    } else {
                        errorMessage = "Option " + arg + " requires a value";
                        return false;
                    }
                    break;
                }
            }
            if (!found) {
                errorMessage = "Unknown option: " + arg;
                return false;
            }
        } else {
            // Positional argument - treat as config override
            setOptionFromString("custom_args", arg, options);
        }
    }
    
    // Post-process options
    postProcessOptions(options);
    
    return validateOptions(options);
}

void CLIParser::printHelp(const std::string& programName) {
    std::cout << helpText << std::endl;
}

void CLIParser::printVersion() {
    std::cout << versionText << std::endl;
}

bool CLIParser::validateOptions(const CLIOptions& options) {
    // Validate log level
    std::string lowerLevel = options.logLevel;
    std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
    if (lowerLevel != "debug" && lowerLevel != "info" && 
        lowerLevel != "warning" && lowerLevel != "error" && 
        lowerLevel != "critical") {
        errorMessage = "Invalid log level: " + options.logLevel + ". "
                     "Valid levels: debug, info, warning, error, critical";
        return false;
    }
    
    // Validate performance mode
    if (options.performanceMode < 0 || options.performanceMode > 2) {
        errorMessage = "Invalid performance mode: " + std::to_string(options.performanceMode) + \
                      ". Valid modes: 0, 1, 2\";
        return false;
    }
    
    // Validate output format
    std::string lowerFormat = options.outputFormat;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    if (lowerFormat != "console" && lowerFormat != "json" && 
        lowerFormat != "yaml" && lowerFormat != "human") {
        errorMessage = "Invalid output format: " + options.outputFormat + \
                      ". Valid formats: console, json, yaml, human\";
        return false;
    }
    
    // Check conflicting options
    if (options.jsonOutput && options.yamlOutput) {
        errorMessage = "Cannot use both --json-output and --yaml-output\";
        return false;
    }
    
    if (options.quiet && options.verbose) {
        errorMessage = "Cannot use both --quiet and --verbose\";
        return false;
    }
    
    if (options.outputFormat != "console" && (options.jsonOutput || options.yamlOutput)) {
        errorMessage = "Output format conflicts with --json-output or --yaml-output\";
        return false;
    }
    
    return true;
}

void CLIParser::setOptionFromString(const std::string& key, const std::string& value, CLIOptions& options) {
    if (key == "config") {
        options.configFile = value;
    } else if (key == "log-level") {
        options.logLevel = value;
    } else if (key == "log-file") {
        options.logFile = value;
    } else if (key == "verbose") {
        options.verbose = (value == "true" || value == "1" || value == "yes");
    } else if (key == "quiet") {
        options.quiet = (value == "true" || value == "1" || value == "yes");
    } else if (key == "debug") {
        options.debug = (value == "true" || value == "1" || value == "yes");
    } else if (key == "json-output") {
        options.jsonOutput = (value == "true" || value == "1" || value == "yes");
    } else if (key == "yaml-output") {
        options.yamlOutput = (value == "true" || value == "1" || value == "yes");
    } else if (key == "performance-mode") {
        options.performanceMode = std::stoi(value);
    } else if (key == "max-memory") {
        options.maxMemory = std::stoi(value);
    } else if (key == "timeout") {
        options.timeout = std::stod(value);
    } else if (key == "experiment") {
        options.experimentName = value;
    } else if (key == "plugin") {
        options.plugins.push_back(value);
    } else if (key == "output-format") {
        options.outputFormat = value;
    } else if (key == "custom-args") {
        // Handle custom configuration arguments
        size_t pos = value.find('=');
        if (pos != std::string::npos) {
            std::string configKey = value.substr(0, pos);
            std::string configValue = value.substr(pos + 1);
            options.customArgs[configKey] = configValue;
        }
    }
}

void CLIParser::postProcessOptions(CLIOptions& options) {
    // Determine effective log level based on other options
    if (options.debug) {
        options.logLevel = "debug";
    }
    
    if (options.quiet) {
        options.logLevel = "error";
    }
    
    // Set output format based on explicit flags
    if (options.jsonOutput) {
        options.outputFormat = "json";
    } else if (options.yamlOutput) {
        options.outputFormat = "yaml";
    }
    
    // Handle log file
    if (!options.logFile.empty()) {
        // Will be handled by Logger when configured
    }
}