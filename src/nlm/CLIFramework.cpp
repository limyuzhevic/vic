#include "nlm/CLIFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <limits>
#include <cstring>

namespace nlm {

namespace {
    // Helper functions
    bool stringEndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        while (start < end && std::isspace(str[start])) start++;
        while (end > start && std::isspace(str[end - 1])) end--;
        
        return str.substr(start, end - start);
    }
    
    bool stringToBool(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        return lower == "true" || lower == "1" || lower == "yes" || 
               lower == "on" || lower == "enable" || lower == "enabled";
    }
}

NLMCliOptions parseCommandLine(int argc, char** argv) {
    NLMCliOptions options;
    
    // Check for help or version first
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
            continue;
        }
        if (arg == "--version" || arg == "-v") {
            options.showVersion = true;
            continue;
        }
    }
    
    // Parse other options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") continue;
        if (arg == "--version" || arg == "-v") continue;
        
        if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            size_t equalsPos = key.find('=');
            
            if (equalsPos != std::string::npos) {
                std::string value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
                
                // Parse key-value pair
                if (key == "config") {
                    options.configFile = value;
                } else if (key == "output-dir") {
                    options.outputDir = value;
                } else if (key == "log-level") {
                    options.logLevel = value;
                } else if (key == "verbose" || key == "v") {
                    options.verbose = stringToBool(value);
                } else if (key == "debug") {
                    options.debug = stringToBool(value);
                } else if (key == "mode") {
                    options.mode = value;
                } else if (key == "save-results" || key == "save") {
                    options.saveResults = stringToBool(value);
                } else if (key == "max-steps") {
                    try {
                        options.maxSteps = parseInt(value, 1, 1000000);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --max-steps: " + std::string(e.what()));
                    }
                } else if (key == "checkpoint-path") {
                    options.checkpointPath = value;
                } else if (key == "enable-checkpointing" || key == "checkpoint") {
                    options.enableCheckpointing = stringToBool(value);
                } else if (key == "enable-replay" || key == "replay") {
                    options.enableReplay = stringToBool(value);
                } else if (key == "enable-development" || key == "development") {
                    options.enableDevelopment = stringToBool(value);
                } else if (key == "experiment-mode") {
                    options.experimentMode = value;
                } else if (key == "neuron-count") {
                    try {
                        options.neuronCount = parseInt(value, 10, 1000000);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --neuron-count: " + std::string(e.what()));
                    }
                } else if (key == "region-count") {
                    try {
                        options.regionCount = parseInt(value, 1, 100);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --region-count: " + std::string(e.what()));
                    }
                } else if (key == "connection-probability") {
                    try {
                        options.connectionProbability = parseFloat(value, 0.0f, 1.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --connection-probability: " + std::string(e.what()));
                    }
                } else if (key == "random-seed") {
                    options.randomSeed = value;
                } else if (key == "stdp-ltp-weight") {
                    try {
                        options.stdpLTPWeight = parseFloat(value, 0.0f, 10.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --stdp-ltp-weight: " + std::string(e.what()));
                    }
                } else if (key == "stdp-ltd-weight") {
                    try {
                        options.stdpLTDWeight = parseFloat(value, 0.0f, 10.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --stdp-ltd-weight: " + std::string(e.what()));
                    }
                } else if (key == "stdp-tau") {
                    try {
                        options.stdpTau = parseFloat(value, 0.0f, 1000.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --stdp-tau: " + std::string(e.what()));
                    }
                } else if (key == "synaptogenesis-rate") {
                    try {
                        options.synaptogenesisRate = parseFloat(value, 0.0f, 1.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --synaptogenesis-rate: " + std::string(e.what()));
                    }
                } else if (key == "pruning-rate") {
                    try {
                        options.pruningRate = parseFloat(value, 0.0f, 1.0f);
                    } catch (const CLIException& e) {
                        throw CLIException("Invalid value for --pruning-rate: " + std::string(e.what()));
                    }
                } else {
                    throw CLIException("Unknown option: " + key);
                }
            } else {
                // Boolean flag
                if (key == "verbose" || key == "v") {
                    options.verbose = true;
                } else if (key == "debug") {
                    options.debug = true;
                } else if (key == "save-results" || key == "save") {
                    options.saveResults = true;
                } else if (key == "enable-checkpointing" || key == "checkpoint") {
                    options.enableCheckpointing = true;
                } else if (key == "enable-replay" || key == "replay") {
                    options.enableReplay = true;
                } else if (key == "enable-development" || key == "development") {
                    options.enableDevelopment = true;
                } else {
                    throw CLIException("Unknown flag: " + key);
                }
            }
        } else if (arg.substr(0, 2) == "-" && arg.size() > 2) {
            // Short option with value (e.g., -n500)
            std::string shortOpt = arg.substr(1, 1);
            std::string value = arg.substr(2);
            
            if (shortOpt == "n" && !value.empty()) {
                try {
                    options.neuronCount = parseInt(value, 10, 1000000);
                } catch (const CLIException& e) {
                    throw CLIException("Invalid value for -n: " + std::string(e.what()));
                }
            } else if (shortOpt == "r" && !value.empty()) {
                try {
                    options.regionCount = parseInt(value, 1, 100);
                } catch (const CLIException& e) {
                    throw CLIException("Invalid value for -r: " + std::string(e.what()));
                }
            } else if (shortOpt == "c" && !value.empty()) {
                try {
                    options.connectionProbability = parseFloat(value, 0.0f, 1.0f);
                } catch (const CLIException& e) {
                    throw CLIException("Invalid value for -c: " + std::string(e.what()));
                }
            } else {
                throw CLIException("Unknown short option: -" + shortOpt);
            }
        } else {
            // Positional argument (for demo-specific arguments)
            // For now, treat as unknown
            throw CLIException("Unknown positional argument: " + arg);
        }
    }
    
    return options;
}

void printHelp(const std::string& programName) {
    std::cout << "===========================================" << std::endl;
    std::cout << "NLM - Neural Learning Machine CLI Tool" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "  Run NLM brain simulations with configurable options." << std::endl;
    std::cout << "  Supports different modes: basic, phase3, phase4, phase6." << std::endl;
    std::cout << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --config FILE                 Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output-dir DIR              Output directory for results" << std::endl;
    std::cout << "  --log-level LEVEL             Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "  --debug                       Enable debug mode" << std::endl;
    std::cout << "  --mode MODE                   Simulation mode (basic, phase3, phase4, phase6)" << std::endl;
    std::cout << "  --save-results, --save        Save simulation results" << std::endl;
    std::cout << "  --max-steps N                 Maximum simulation steps" << std::endl;
    std::cout << "  --checkpoint-path PATH         Checkpoint file path" << std::endl;
    std::cout << "  --enable-checkpointing, --checkpoint    Enable checkpointing" << std::endl;
    std::cout << "  --enable-replay, --replay     Enable replay mode" << std::endl;
    std::cout << "  --enable-development, --development    Enable development mode" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 3 OPTIONS:" << std::endl;
    std::cout << "  --num-episodes N              Number of episodes (default: 5)" << std::endl;
    std::cout << "  --steps-per-episode N          Steps per episode (default: 200)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 4 OPTIONS:" << std::endl;
    std::cout << "  --num-trials N                Number of trials (default: 50)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 6 OPTIONS:" << std::endl;
    std::cout << "  --demo-steps N                Number of steps for demo (default: 2000)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --neuron-count N              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region-count N               Number of regions (default: 2)" << std::endl;
    std::cout << "  --connection-probability P     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --random-seed SEED             Random seed (default: 42)" << std::endl;
    std::cout << "  --stdp-ltp-weight W            STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight W            STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau T                   STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --synaptogenesis-rate R        Synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate R               Pruning rate (default: 0.00001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "SHORT OPTIONS:" << std::endl;
    std::cout << "  -n N                          Neuron count" << std::endl;
    std::cout << "  -r N                          Region count" << std::endl;
    std::cout << "  -c P                          Connection probability" << std::endl;
    std::cout << "  -s SEED                        Random seed" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  " << programName << " --mode phase3 --num-episodes 10 --steps-per-episode 500" << std::endl;
    std::cout << "  " << programName << " --mode phase4 --num-trials 100 --neuron-count 2000" << std::endl;
    std::cout << "  " << programName << " --mode phase6 --enable-checkpointing --checkpoint-path demo.bin" << std::endl;
    std::cout << "  " << programName << " --neuron-count 1000 --region-count 4 --connection-probability 0.05" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: Phase 6 Integration" << std::endl;
    std::cout << "Author: Research Project" << std::endl;
    std::cout << "License: MIT" << std::endl;
}

void validateOptions(const NLMCliOptions& options) {
    if (options.neuronCount < 10) {
        throw CLIException("Neuron count too low (min: 10)");
    }
    if (options.neuronCount > 1000000) {
        throw CLIException("Neuron count too high (max: 1,000,000)");
    }
    
    if (options.regionCount < 1) {
        throw CLIException("Region count must be at least 1");
    }
    
    if (options.regionCount > options.neuronCount / 10) {
        throw CLIException("Region count too high relative to neuron count");
    }
    
    if (options.connectionProbability < 0.0f || options.connectionProbability > 1.0f) {
        throw CLIException("Connection probability must be between 0.0 and 1.0");
    }
    
    if (options.maxSteps < 1) {
        throw CLIException("Max steps must be at least 1");
    }
    
    if (options.maxSteps > 1000000) {
        throw CLIException("Max steps too high (max: 1,000,000)");
    }
    
    if (options.stdpLTPWeight < 0.0f) {
        throw CLIException("STDP LTP weight cannot be negative");
    }
    
    if (options.stdpLTDWeight < 0.0f) {
        throw CLIException("STDP LTD weight cannot be negative");
    }
    
    if (options.stdpTau <= 0.0f) {
        throw CLIException("STDP tau must be positive");
    }
    
    if (options.synaptogenesisRate < 0.0f || options.synaptogenesisRate > 1.0f) {
        throw CLIException("Synaptogenesis rate must be between 0.0 and 1.0");
    }
    
    if (options.pruningRate < 0.0f || options.pruningRate > 1.0f) {
        throw CLIException("Pruning rate must be between 0.0 and 1.0");
    }
    
    if (options.mode != "basic" && options.mode != "phase3" && 
        options.mode != "phase4" && options.mode != "phase6") {
        throw CLIException("Invalid mode. Must be one of: basic, phase3, phase4, phase6");
    }
}

int parseInt(const std::string& str, int min, int max) {
    try {
        size_t pos;
        int value = std::stoi(str, &pos);
        
        if (pos != str.size()) {
            throw CLIException("Invalid integer format");
        }
        
        if (value < min || value > max) {
            throw CLIException("Value " + std::to_string(value) + " out of range [" + 
                             std::to_string(min) + ", " + std::to_string(max) + "]");
        }
        
        return value;
    } catch (const std::invalid_argument&) {
        throw CLIException("Invalid integer format");
    } catch (const std::out_of_range&) {
        throw CLIException("Integer value out of range");
    }
}

float parseFloat(const std::string& str, float min, float max) {
    try {
        size_t pos;
        float value = std::stof(str, &pos);
        
        if (pos != str.size()) {
            throw CLIException("Invalid float format");
        }
        
        if (value < min || value > max) {
            throw CLIException("Value " + std::to_string(value) + " out of range [" + 
                             std::to_string(min) + ", " + std::to_string(max) + "]");
        }
        
        return value;
    } catch (const std::invalid_argument&) {
        throw CLIException("Invalid float format");
    } catch (const std::out_of_range&) {
        throw CLIException("Float value out of range");
    }
}

bool parseBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "true" || lower == "1" || lower == "yes" || 
        lower == "on" || lower == "enable" || lower == "enabled") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || 
               lower == "off" || lower == "disable" || lower == "disabled") {
        return false;
    } else {
        throw CLIException("Invalid boolean value: " + str);
    }
}

bool loadConfigFile(const std::string& configFile) {
    try {
        if (!std::filesystem::exists(configFile)) {
            return false;
        }
        
        // Try to load the config file
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + configFile + ": " + std::string(e.what()));
        return false;
    }
}

void logOptions(const NLMCliOptions& options) {
    NLM_LOG_INFO("CLI Options:");
    NLM_LOG_INFO(formatOption("  configFile", options.configFile));
    NLM_LOG_INFO(formatOption("  outputDir", options.outputDir));
    NLM_LOG_INFO(formatOption("  logLevel", options.logLevel));
    NLM_LOG_INFO(formatOption("  verbose", options.verbose));
    NLM_LOG_INFO(formatOption("  debug", options.debug));
    NLM_LOG_INFO(formatOption("  mode", options.mode));
    NLM_LOG_INFO(formatOption("  saveResults", options.saveResults));
    NLM_LOG_INFO(formatOption("  maxSteps", options.maxSteps));
    NLM_LOG_INFO(formatOption("  checkpointPath", options.checkpointPath));
    NLM_LOG_INFO(formatOption("  enableCheckpointing", options.enableCheckpointing));
    NLM_LOG_INFO(formatOption("  enableReplay", options.enableReplay));
    NLM_LOG_INFO(formatOption("  enableDevelopment", options.enableDevelopment));
    NLM_LOG_INFO(formatOption("  experimentMode", options.experimentMode));
    NLM_LOG_INFO(formatOption("  neuronCount", options.neuronCount));
    NLM_LOG_INFO(formatOption("  regionCount", options.regionCount));
    NLM_LOG_INFO(formatOption("  connectionProbability", options.connectionProbability));
    NLM_LOG_INFO(formatOption("  randomSeed", options.randomSeed));
    NLM_LOG_INFO(formatOption("  stdpLTPWeight", options.stdpLTPWeight));
    NLM_LOG_INFO(formatOption("  stdpLTDWeight", options.stdpLTDWeight));
    NLM_LOG_INFO(formatOption("  stdpTau", options.stdpTau));
    NLM_LOG_INFO(formatOption("  synaptogenesisRate", options.synaptogenesisRate));
    NLM_LOG_INFO(formatOption("  pruningRate", options.pruningRate));
}

bool ensureOutputDirectory(const std::string& path) {
    try {
        if (path.empty() || path == ".") {
            return true;
        }
        
        if (!std::filesystem::exists(path)) {
            if (!std::filesystem::create_directories(path)) {
                NLM_LOG_ERROR("Failed to create output directory: " + path);
                return false;
            }
            NLM_LOG_INFO("Created output directory: " + path);
        } else if (!std::filesystem::is_directory(path)) {
            NLM_LOG_ERROR("Output path exists but is not a directory: " + path);
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error ensuring output directory " + path + ": " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm