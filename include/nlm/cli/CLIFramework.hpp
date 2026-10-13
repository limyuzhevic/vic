// NLM CLI Framework
// Provides comprehensive command-line interface support for all NLM executables
// Includes argument parsing, help/usage generation, and validation

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdint>

namespace nlm {
namespace cli {

// Command line options structure
struct CLIOptions {
    bool helpRequested = false;
    bool versionRequested = false;
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "info";
    std::string outputFile = "";
    bool quiet = false;
    bool verbose = false;
    uint32_t numEpisodes = 5;
    uint32_t stepsPerEpisode = 200;
    uint32_t numTrials = 50;
    uint32_t maxSteps = 2000;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    double customTimestep = 0.0; // 0.0 means use default
    uint64_t customSeed = 0; // 0 means use default
    std::map<std::string, std::string> customConfigs;
    bool listConfigs = false;
    bool validateConfigs = false;
    std::string environmentName = "default";
    uint32_t worldWidth = 20;
    uint32_t worldHeight = 20;
    uint32_t visionWidth = 16;
    uint32_t visionHeight = 16;
    bool autoSave = false;
    uint32_t autoSaveInterval = 1000;
    bool enableNeuromodulation = true;
    bool enableCuriosity = true;
    bool enablePrediction = true;
    bool enableMemory = true;
    double rewardDecay = 0.99;
    double explorationRate = 0.1;
    double exploitationRate = 0.9;
    std::string mode = "standard"; // standard, performance, learning, debug
    std::string resultFormat = "text"; // text, json, csv, yaml
    bool benchmarkMode = false;
    uint32_t benchmarkDuration = 60; // seconds
    std::string benchmarkOutput = "benchmark_results.json";
    // Performance monitoring options
    bool enableMemoryMonitoring = false;
    bool enableFPGMonitoring = false;
    bool enableNetworkMonitoring = false;
    std::string performanceOutputFile = "performance_metrics.json";
    bool detailedProfiling = false;
    uint32_t profilingSampleRate = 1000;
    std::string profilingOutput = "profile_data.txt";
    bool enableRealTimeStats = false;
    uint32_t statsInterval = 100;
};

// Argument parser class
class ArgumentParser {
public:
    struct Argument {
        std::string name;
        char shortOpt;
        std::string description;
        bool hasValue;
        bool required = false;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
    };

private:
    std::vector<Argument> arguments;
    std::map<std::string, std::string> parsedArgs;
    std::string programName;
    std::string version = "NLM 0.1.0";
    std::string usage = "Usage: {program} [options]";
    std::string description = "NLM (Neural Learning Machine) - An Experimental Artificial Developmental Brain";
    std::string helpText = "";

public:
    ArgumentParser(const std::string& name = "nlm") : programName(name) {
        // Add common arguments
        addArgument("--help", 'h', "Show help information and exit", false);
        addArgument("--version", '\0', "Show version information and exit", false);
        addArgument("--config", 'c', "Configuration file path", true);
        addArgument("--log-level", '\0', "Logging level (trace, debug, info, warn, error, fatal)", true);
        addArgument("--output", 'o', "Output file path", true);
        addArgument("--quiet", '\0', "Suppress non-essential output", false);
        addArgument("--verbose", '\0', "Enable verbose output", false);
        addArgument("--benchmark", '\0', "Enable benchmarking mode", false);
        addArgument("--benchmark-output", '\0', "Benchmark results output file", true);
        addArgument("--benchmark-duration", '\0', "Benchmark duration in seconds", true);
        // Performance monitoring arguments
        addArgument("--enable-memory-monitoring", '\0', "Enable memory usage monitoring", false);
        addArgument("--enable-fp-monitoring", '\0', "Enable floating-point operations monitoring", false);
        addArgument("--enable-network-monitoring", '\0', "Enable network activity monitoring", false);
        addArgument("--performance-output", '\0', "Performance metrics output file", true);
        addArgument("--detailed-profiling", '\0', "Enable detailed profiling", false);
        addArgument("--profiling-sample-rate", '\0', "Profiling sample rate (steps between samples)", true);
        addArgument("--profiling-output", '\0', "Profiling output file", true);
        addArgument("--enable-real-time-stats", '\0', "Enable real-time statistics", false);
        addArgument("--stats-interval", '\0', "Real-time stats interval (steps between updates)", true);
    }

    void setVersion(const std::string& ver) { version = ver; }
    void setUsage(const std::string& usageStr) { usage = usageStr; }
    void setDescription(const std::string& desc) { description = desc; }

    void addArgument(const std::string& longOpt, char shortOpt, const std::string& desc,
                     bool hasValue, bool required = false, const std::string& defaultVal = "") {
        Argument arg;
        arg.name = longOpt;
        arg.shortOpt = shortOpt;
        arg.description = desc;
        arg.hasValue = hasValue;
        arg.required = required;
        arg.defaultValue = defaultVal;
        arguments.push_back(arg);

        // Add to help text
        std::ostringstream helpLine;
        helpLine << "  ";
        if (shortOpt != '\0') {
            helpLine << "-" << shortOpt << ", ";
        }
        helpLine << "--" << longOpt;
        if (hasValue) {
            helpLine << "=<value>";
        }
        helpLine << "\n    " << desc;
        if (!defaultValue.empty()) {
            helpLine << " (default: " << defaultValue << ")";
        }
        if (required) {
            helpLine << " (required)";
        }
        helpText += helpLine.str() + "\n";
    }

    CLIOptions parse(int argc, char* argv[]) {
        CLIOptions options;
        
        // Process arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for help/version
            if (arg == "--help" || arg == "-h") {
                options.helpRequested = true;
                continue;
            }
            if (arg == "--version") {
                options.versionRequested = true;
                continue;
            }
            
            // Find argument definition
            Argument* argDef = nullptr;
            for (auto& arg : arguments) {
                if (arg.name == arg && (arg.shortOpt != '\0' && std::string(1, arg.shortOpt) == arg.substr(1) || arg.name == arg)) {
                    argDef = &arg;
                    break;
                }
            }
            
            if (!argDef) {
                std::cerr << "Unknown option: " << arg << std::endl;
                std::exit(1);
            }
            
            // Process argument with value
            if (argDef->hasValue) {
                if (i + 1 >= argc) {
                    std::cerr << "Error: Option " << argDef->name << " requires a value" << std::endl;
                    std::exit(1);
                }
                
                std::string value = argv[++i];
                
                // Handle --key=value format
                size_t pos = argDef->name.find('=');
                if (pos != std::string::npos && argDef->name.substr(pos + 1) == value) {
                    value = argDef->name.substr(pos + 1);
                }
                
                parsedArgs[argDef->name] = value;
            } else {
                // Boolean flag
                parsedArgs[argDef->name] = "true";
            }
        }
        
        // Convert parsed args to options
        options = convertParsedArgsToOptions(parsedArgs);
        
        return options;
    }

    void printHelp() {
        std::cout << usage << std::endl;
        std::cout << std::endl;
        std::cout << description << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << helpText << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  {program} --help                     # Show help information" << std::endl;
        std::cout << "  {program} --config myconfig.cfg      # Use custom config" << std::endl;
        std::cout << "  {program} --log-level debug         # Enable debug logging" << std::endl;
        std::cout << "  {program} --benchmark --benchmark-output results.json" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/neural-learning-machine/nlm" << std::endl;
    }

    void printVersion() {
        std::cout << version << std::endl;
    }

    bool validateOptions(const CLIOptions& options) {
        // Validate configuration file
        if (!options.configFile.empty()) {
            // File existence check could be added here
        }
        
        // Validate log level
        std::vector<std::string> validLogLevels = {"trace", "debug", "info", "warn", "error", "fatal"};
        if (std::find(validLogLevels.begin(), validLogLevels.end(), options.logLevel) == validLogLevels.end()) {
            std::cerr << "Invalid log level: " << options.logLevel << std::endl;
            return false;
        }
        
        // Validate numeric ranges
        if (options.numEpisodes == 0 || options.numEpisodes > 1000) {
            std::cerr << "Invalid number of episodes: " << options.numEpisodes << std::endl;
            return false;
        }
        
        if (options.stepsPerEpisode == 0 || options.stepsPerEpisode > 10000) {
            std::cerr << "Invalid steps per episode: " << options.stepsPerEpisode << std::endl;
            return false;
        }
        
        if (options.maxSteps == 0 || options.maxSteps > 100000) {
            std::cerr << "Invalid max steps: " << options.maxSteps << std::endl;
            return false;
        }
        
        if (options.benchmarkDuration == 0 || options.benchmarkDuration > 3600) {
            std::cerr << "Invalid benchmark duration: " << options.benchmarkDuration << std::endl;
            return false;
        }
        
        // Validate profiling sample rate
        if (options.profilingSampleRate == 0 || options.profilingSampleRate > 10000) {
            std::cerr << "Invalid profiling sample rate: " << options.profilingSampleRate << std::endl;
            return false;
        }
        
        // Validate stats interval
        if (options.statsInterval == 0 || options.statsInterval > 1000) {
            std::cerr << "Invalid stats interval: " << options.statsInterval << std::endl;
            return false;
        }
        
        // Validate world dimensions
        if (options.worldWidth == 0 || options.worldWidth > 1000) {
            std::cerr << "Invalid world width: " << options.worldWidth << std::endl;
            return false;
        }
        
        if (options.worldHeight == 0 || options.worldHeight > 1000) {
            std::cerr << "Invalid world height: " << options.worldHeight << std::endl;
            return false;
        }
        
        return true;
    }

    std::string formatUsage() {
        std::string formattedUsage = usage;
        // Replace {program} with actual program name
        size_t pos = formattedUsage.find("{program}");
        if (pos != std::string::npos) {
            formattedUsage.replace(pos, 9, programName);
        }
        return formattedUsage;
    }

private:
    CLIOptions convertParsedArgsToOptions(const std::map<std::string, std::string>& parsedArgs) {
        CLIOptions options;
        
        for (const auto& pair : parsedArgs) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            if (key == "--help") options.helpRequested = true;
            else if (key == "--version") options.versionRequested = true;
            else if (key == "--config") options.configFile = value;
            else if (key == "--log-level") options.logLevel = value;
            else if (key == "--output") options.outputFile = value;
            else if (key == "--quiet") options.quiet = true;
            else if (key == "--verbose") options.verbose = true;
            else if (key == "--benchmark") options.benchmarkMode = true;
            else if (key == "--benchmark-output") options.benchmarkOutput = value;
            else if (key == "--benchmark-duration") options.benchmarkDuration = std::stoi(value);
            // Add more mappings as needed
        }
        
        return options;
    }
};

// Configuration validator
class ConfigValidator {
public:
    static bool validateConfig(const std::string& configFile, const CLIOptions& options) {
        // Basic validation
        if (!configFile.empty()) {
            // File existence check
            // File format validation
        }
        
        // Validate that required components are enabled for the mode
        if (options.mode == "performance") {
            if (!options.enableNeuromodulation) {
                return false;
            }
        }
        
        if (options.mode == "learning") {
            if (!options.enableMemory || !options.enableCuriosity) {
                return false;
            }
        }
        
        // Validate combination of options
        if (options.enableCheckpointing && !options.enableDevelopment) {
            // Allow checkpointing without development, but warn
        }
        
        if (options.enableReplay && options.autoSave) {
            // Enable both replay and auto-save
        }
        
        return true;
    }
    
    static std::string getValidationError(const std::string& configFile, const CLIOptions& options) {
        std::string errors;
        
        // Validate numeric ranges
        if (options.numEpisodes == 0 || options.numEpisodes > 1000) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid number of episodes: " + std::to_string(options.numEpisodes);
        }
        
        if (options.stepsPerEpisode == 0 || options.stepsPerEpisode > 10000) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid steps per episode: " + std::to_string(options.stepsPerEpisode);
        }
        
        if (options.maxSteps == 0 || options.maxSteps > 100000) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid max steps: " + std::to_string(options.maxSteps);
        }
        
        if (options.benchmarkDuration == 0 || options.benchmarkDuration > 3600) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid benchmark duration: " + std::to_string(options.benchmarkDuration);
        }
        
        // Validate world dimensions
        if (options.worldWidth == 0 || options.worldWidth > 1000) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid world width: " + std::to_string(options.worldWidth);
        }
        
        if (options.worldHeight == 0 || options.worldHeight > 1000) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid world height: " + std::to_string(options.worldHeight);
        }
        
        // Validate consistency
        if (options.enableReplay && options.autoSave) {
            // This is valid, so no error
        }
        
        return errors;
    }
};

} // namespace cli
} // namespace nlm
