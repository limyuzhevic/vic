// NLM Common CLI Utilities Implementation

#include "core/CLI.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

CLIParser::CLIParser(const std::string& programName) : programName(programName) {
    // Set default version
    versionInfo = "NLM 0.1.0";
    description = "Neural Learning Machine - Experimental artificial developmental brain";
}

void CLIParser::addArgument(const std::string& name, const std::string& description,
                           const std::string& longDescription,
                           const std::string& defaultValue,
                           bool required,
                           std::function<bool(const std::string&)> validator) {
    argumentDescriptions[name] = description;
    argumentLongDescriptions[name] = longDescription;
    argumentDefaults[name] = defaultValue;
    argumentRequired[name] = required;
    argumentValidators[name] = validator;
}

void CLIParser::addFlag(const std::string& name, const std::string& description,
                       const std::string& longDescription) {
    flagDescriptions[name] = description;
    flagLongDescriptions[name] = longDescription;
}

CLIResult CLIParser::parse(int argc, char* argv[]) {
    CLIResult result;
    result.success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            result.success = false;  // Exit after help
            return result;
        } else if (arg == "--version") {
            printVersion();
            result.success = false;  // Exit after version
            return result;
        } else if (arg.substr(0, 2) == "--") {
            // Long option: --name=value or --name value
            std::string option = arg.substr(2);
            std::string value;
            
            size_t equalsPos = option.find('=');
            if (equalsPos != std::string::npos) {
                option = option.substr(0, equalsPos);
                value = option.substr(equalsPos + 1);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                // Next argument is the value
                value = argv[++i];
            }
            
            if (argumentDescriptions.find(option) != argumentDescriptions.end()) {
                if (!value.empty() && argumentTransformers.count(option)) {
                    value = argumentTransformers[option](value);
                }
                
                if (argumentValidators.count(option)) {
                    if (!argumentValidators[option](value)) {
                        result.success = false;
                        result.error = "Invalid value for option --" + option + ": " + value;
                        return result;
                    }
                }
                
                arguments[option] = value;
            } else if (flagDescriptions.find(option) != flagDescriptions.end()) {
                flags[option] = true;
            } else {
                result.success = false;
                result.error = "Unknown option: --" + option;
                return result;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option: -abc or -a value
            std::string option = arg.substr(1);
            
            if (option.length() == 1) {
                // Single short option
                char shortOpt = option[0];
                std::string longOpt(1, shortOpt);
                
                if (argumentDescriptions.find(longOpt) != argumentDescriptions.end()) {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        std::string value = argv[++i];
                        if (argumentTransformers.count(longOpt)) {
                            value = argumentTransformers[longOpt](value);
                        }
                        arguments[longOpt] = value;
                    } else {
                        result.success = false;
                        result.error = "Option -" + longOpt + " requires a value";
                        return result;
                    }
                } else if (flagDescriptions.find(longOpt) != flagDescriptions.end()) {
                    flags[longOpt] = true;
                } else {
                    result.success = false;
                    result.error = "Unknown option: -" + longOpt;
                    return result;
                }
            } else {
                // Multiple short options
                for (size_t j = 0; j < option.length(); ++j) {
                    char shortOpt = option[j];
                    std::string longOpt(1, shortOpt);
                    
                    if (argumentDescriptions.find(longOpt) != argumentDescriptions.end()) {
                        result.success = false;
                        result.error = "Option -" + longOpt + " requires a value";
                        return result;
                    } else if (flagDescriptions.find(longOpt) != flagDescriptions.end()) {
                        flags[longOpt] = true;
                    } else {
                        result.success = false;
                        result.error = "Unknown option: -" + longOpt;
                        return result;
                    }
                }
            }
        } else {
            result.success = false;
            result.error = "Unexpected argument: " + arg;
            return result;
        }
    }
    
    // Validate required arguments
    for (const auto& pair : argumentRequired) {
        const std::string& name = pair.first;
        bool required = pair.second;
        
        if (required && arguments.find(name) == arguments.end() && flags.find(name) == flags.end()) {
            result.success = false;
            result.error = "Required option --" + name + " is missing";
            return result;
        }
    }
    
    return result;
}

std::string CLIParser::getValue(const std::string& name) const {
    auto it = arguments.find(name);
    if (it != arguments.end()) {
        return it->second;
    }
    
    // Return default if available
    auto defaultIt = argumentDefaults.find(name);
    if (defaultIt != argumentDefaults.end()) {
        return defaultIt->second;
    }
    
    return "";
}

bool CLIParser::getFlag(const std::string& name) const {
    auto it = flags.find(name);
    return it != it->second;
}

bool CLIParser::hasArgument(const std::string& name) const {
    return arguments.find(name) != arguments.end();
}

void CLIParser::printError(const std::string& error) const {
    std::cerr << "Error: " << error << std::endl;
    std::cerr << "Use --help for usage information." << std::endl;
}

std::string CLIParser::formatHelp(const std::string& arg, const std::string& desc,
                                 const std::string& longDesc, const std::string& defaultVal,
                                 bool required) const {
    std::stringstream ss;
    
    // Format option name
    if (arg.length() == 1) {
        ss << "  -" << arg << ", --" << arg;
    } else {
        ss << "      --" << arg;
    }
    
    // Add default value if not empty
    if (!defaultVal.empty()) {
        ss << " (default: " << defaultVal << ")";
    }
    
    ss << "\n";
    
    // Format description
    ss << "        " << desc;
    
    // Add long description if available
    if (!longDesc.empty()) {
        ss << "\n        " << longDesc;
    }
    
    return ss.str();
}

std::string CLIParser::generateHelp() const {
    std::stringstream ss;
    
    ss << "Usage: " << programName << " [OPTIONS]" << std::endl << std::endl;
    
    if (!description.empty()) {
        ss << description << std::endl << std::endl;
    }
    
    if (!argumentDescriptions.empty()) {
        ss << "Arguments:" << std::endl;
        for (const auto& pair : argumentDescriptions) {
            const std::string& name = pair.first;
            const std::string& desc = pair.second;
            const std::string& longDesc = argumentLongDescriptions.count(name) ? argumentLongDescriptions.at(name) : "";
            const std::string& defaultVal = argumentDefaults.count(name) ? argumentDefaults.at(name) : "";
            bool required = argumentRequired.count(name) ? argumentRequired.at(name) : false;
            
            ss << formatHelp(name, desc, longDesc, defaultVal, required) << std::endl;
        }
        ss << std::endl;
    }
    
    if (!flagDescriptions.empty()) {
        ss << "Flags:" << std::endl;
        for (const auto& pair : flagDescriptions) {
            const std::string& name = pair.first;
            const std::string& desc = pair.second;
            const std::string& longDesc = flagLongDescriptions.count(name) ? flagLongDescriptions.at(name) : "";
            
            ss << "      --" << name;
            
            if (!longDesc.empty()) {
                ss << "\n        " << longDesc;
            }
            ss << std::endl;
        }
        ss << std::endl;
    }
    
    ss << "Other options:" << std::endl;
    ss << formatHelp("h", "Show help message", "Display this help message and exit", "", false) << std::endl;
    ss << formatHelp("version", "Show version information", "Display version information and exit", "", false) << std::endl;
    
    return ss.str();
}

void CLIParser::printHelp() const {
    std::cout << generateHelp() << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << programName << " " << versionInfo << std::endl;
}

// NLMCommonOptions parsing functions

NLMCommonOptions parseCommonOptions(int argc, char* argv[], const std::string& programName) {
    NLMCommonOptions options;
    
    CLIParser parser(programName);
    parser.setDescription("Neural Learning Machine - Experimental artificial developmental brain");
    parser.setVersion("0.1.0");
    
    // Common arguments
    parser.addArgument("config", "Configuration file", "Load configuration from specified file", "configs/default.cfg");
    parser.addArgument("output", "Output file", "Save results to specified file", "");
    parser.addArgument("log-level", "Logging level", "Set logging verbosity (trace, debug, info, warn, error, critical)", "info");
    parser.addArgument("random-seed", "Random seed", "Set random seed for reproducible simulations", "");
    parser.addArgument("timestep", "Simulation timestep", "Set simulation timestep in seconds", "0.001");
    parser.addArgument("num-episodes", "Number of episodes", "Number of episodes to run", "1");
    parser.addArgument("steps-per-episode", "Steps per episode", "Number of steps per episode", "1000");
    
    // Common flags
    parser.addFlag("verbose", "Verbose output", "Enable verbose output for detailed information");
    parser.addFlag("quiet", "Quiet mode", "Suppress informational messages, only show errors and warnings");
    parser.addFlag("help", "Show help", "Display help information and exit");
    parser.addFlag("version", "Show version", "Display version information and exit");
    parser.addFlag("plasticity", "Enable plasticity", "Enable synaptic plasticity mechanisms");
    parser.addFlag("development", "Enable development", "Enable developmental processes");
    parser.addFlag("curiosity", "Enable curiosity", "Enable curiosity-driven exploration");
    parser.addFlag("reward", "Enable reward modulation", "Enable reward-based learning");
    parser.addFlag("checkpoint", "Enable checkpointing", "Enable checkpoint saving and loading");
    parser.addFlag("replay", "Enable replay", "Enable memory replay and consolidation");
    
    CLIResult result = parser.parse(argc, argv);
    
    if (!result.success) {
        if (result.error.find("help") != std::string::npos || result.error.find("version") != std::string::npos) {
            // Help or version was requested, parser already handled it
            std::exit(0);
        } else {
            std::cerr << "Error: " << result.error << std::endl;
            std::exit(1);
        }
    }
    
    // Populate options from parser
    options.configFile = parser.getValue("config");
    options.outputFile = parser.getValue("output");
    options.logLevel = parser.getValue("log-level");
    options.verbose = parser.getFlag("verbose");
    options.quiet = parser.getFlag("quiet");
    options.help = parser.getFlag("help");
    options.version = parser.getFlag("version");
    options.numEpisodes = std::stoi(parser.getValue("num-episodes"));
    options.stepsPerEpisode = std::stoi(parser.getValue("steps-per-episode"));
    options.timestep = std::stod(parser.getValue("timestep"));
    options.randomSeed = parser.getValue("random-seed");
    options.enablePlasticity = parser.getFlag("plasticity");
    options.enableDevelopment = parser.getFlag("development");
    options.enableCuriosity = parser.getFlag("curiosity");
    options.enableRewardModulation = parser.getFlag("reward");
    options.enableCheckpointing = parser.getFlag("checkpoint");
    options.enableReplay = parser.getFlag("replay");
    
    // Set default checkpoint file if checkpointing is enabled
    if (options.enableCheckpointing && options.checkpointFile.empty()) {
        options.checkpointFile = "/tmp/nlm_checkpoint.bin";
    }
    
    return options;
}

void printCommonOptionsHelp(const std::string& programName) {
    CLIParser parser(programName);
    parser.printHelp();
}

void setupCommonLogging(const std::string& logLevel, bool verbose, bool quiet) {
    // Configure logging based on options
    // This would integrate with the existing Logger class
    if (quiet) {
        // Set log level to error only
        // Logger::getGlobal().setLevel(LogLevel::Error);
    } else if (verbose) {
        // Set log level to debug
        // Logger::getGlobal().setLevel(LogLevel::Debug);
    } else {
        // Set log level based on logLevel parameter
        // if (logLevel == "trace") Logger::getGlobal().setLevel(LogLevel::Trace);
        // else if (logLevel == "debug") Logger::getGlobal().setLevel(LogLevel::Debug);
        // else if (logLevel == "info") Logger::getGlobal().setLevel(LogLevel::Info);
        // else if (logLevel == "warn") Logger::getGlobal().setLevel(LogLevel::Warn);
        // else if (logLevel == "error") Logger::getGlobal().setLevel(LogLevel::Error);
        // else Logger::getGlobal().setLevel(LogLevel::Info);  // default
    }
}

bool loadCommonConfig(const std::string& configFile, std::map<std::string, std::string>& configValues) {
    // Load configuration from file
    // This would integrate with the existing Config class
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse key=value format
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Skip comments
            if (key.empty() || key[0] == '#') {
                continue;
            }
            
            configValues[key] = value;
        }
    }
    
    file.close();
    return true;
}

std::string formatValue(double value, const std::string& unit) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        ss << " " << unit;
    }
    return ss.str();
}

std::string formatTime(double seconds) {
    if (seconds >= 60.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remainingSeconds = seconds - (minutes * 60.0);
        if (remainingSeconds >= 1.0) {
            return std::to_string(minutes) + "m " + formatValue(remainingSeconds, "s");
        } else {
            return std::to_string(minutes) + "m";
        }
    } else {
        return formatValue(seconds, "s");
    }
}

SimulationMetrics runSimulationLoop(std::function<void(uint64_t, double)> stepFunction,
                                   uint64_t numSteps,
                                   double timestep,
                                   std::function<void(double, const SimulationMetrics&)> progressCallback) {
    SimulationMetrics metrics;
    metrics.startTime = 0.0; // Would get actual time with std::chrono
    metrics.totalSteps = numSteps;
    metrics.totalSpikes = 0;
    metrics.averageFiringRate = 0.0;
    metrics.energyConsumed = 0.0;
    metrics.rewardReceived = 0.0;
    metrics.explorationActions = 0;
    metrics.exploitationActions = 0;
    metrics.totalActions = 0;
    metrics.developmentProgress = 0.0;
    metrics.memoryUtilization = 0.0;
    
    for (uint64_t step = 0; step < numSteps; ++step) {
        stepFunction(step, timestep * step);
        
        // Update metrics (simplified)
        metrics.totalSpikes += 100; // Would get actual spike count
        metrics.totalActions++;
        
        // Call progress callback if provided
        if (progressCallback) {
            double elapsedTime = timestep * (step + 1);
            SimulationMetrics currentMetrics = metrics;
            // Update currentMetrics with actual values for this step
            progressCallback(elapsedTime, currentMetrics);
        }
    }
    
    metrics.endTime = metrics.startTime + (numSteps * timestep);
    metrics.averageFiringRate = static_cast<double>(metrics.totalSpikes) / numSteps;
    
    return metrics;
}

bool exportMetrics(const std::string& filename, const SimulationMetrics& metrics,
                   const std::map<std::string, double>& additionalMetrics) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Simulation Metrics" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Timing:" << std::endl;
    file << "  Duration: " << formatTime(metrics.endTime - metrics.startTime) << std::endl;
    file << "  Total Steps: " << metrics.totalSteps << std::endl;
    file << "  Timestep: " << formatValue(0.001, "s") << std::endl;
    file << std::endl;
    
    file << "Activity:" << std::endl;
    file << "  Total Spikes: " << metrics.totalSpikes << std::endl;
    file << "  Average Firing Rate: " << formatValue(metrics.averageFiringRate, "Hz") << std::endl;
    file << std::endl;
    
    file << "Learning:" << std::endl;
    file << "  Reward Received: " << formatValue(metrics.rewardReceived, "units") << std::endl;
    file << "  Exploration Actions: " << metrics.explorationActions << std::endl;
    file << "  Exploitation Actions: " << metrics.exploitationActions << std::endl;
    file << "  Development Progress: " << formatValue(metrics.developmentProgress, "%") << std::endl;
    file << std::endl;
    
    file << "Resources:" << std::endl;
    file << "  Energy Consumed: " << formatValue(metrics.energyConsumed, "J") << std::endl;
    file << "  Memory Utilization: " << formatValue(metrics.memoryUtilization, "%") << std::endl;
    file << std::endl;
    
    if (!additionalMetrics.empty()) {
        file << "Additional Metrics:" << std::endl;
        for (const auto& pair : additionalMetrics) {
            file << "  " << pair.first << ": " << formatValue(pair.second) << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
    return true;
}

} // namespace nlm