#include "CommandLineInterface.hpp"
#include "../core/Config/ConfigConstants.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace nlm {

CommandLineInterface::CommandLineInterface()
    : config_(std::make_shared<Config>())
    , verbose_(false)
    , debug_(false)
    , profiling_(false)
    , helpRequested_(false)
    , versionRequested_(false)
    , benchmarkRequested_(false)
    , stepsCompleted_(0)
    , neuronsFired_(0) {
    
    registerStandardOptions();
    registerDebugOptions();
    registerPerformanceOptions();
    registerOutputOptions();
    
    startTime_ = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
}

CommandLineInterface::~CommandLineInterface() = default;

bool CommandLineInterface::parseArguments(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments
    }
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested_ = true;
            continue;
        }
        
        if (arg == "--version") {
            versionRequested_ = true;
            continue;
        }
        
        if (arg == "--benchmark") {
            benchmarkRequested_ = true;
            continue;
        }
        
        if (arg == "--verbose" || arg == "-v") {
            verbose_ = true;
            continue;
        }
        
        if (arg == "--debug" || arg == "-d") {
            debug_ = true;
            continue;
        }
        
        if (arg == "--profile" || arg == "-p") {
            profiling_ = true;
            continue;
        }
        
        // Handle --config=filename
        if (arg.find("--config=") == 0) {
            configFile_ = arg.substr(9);
            if (!loadConfig(configFile_)) {
                printError("Failed to load config file: " + configFile_);
                success = false;
            }
            continue;
        }
        
        // Handle --output=filename
        if (arg.find("--output=") == 0) {
            outputFile_ = arg.substr(9);
            continue;
        }
        
        // Handle --key=value format
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            parsedOptions_[key] = value;
            config_->set(key, value);
            continue;
        }
        
        // Handle -key value format
        if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            std::string key = arg.substr(1);
            
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                // Next argument is the value
                std::string value = argv[++i];
                parsedOptions_[key] = value;
                config_->set(key, value);
            } else {
                printError("Missing value for option: " + arg);
                success = false;
            }
            continue;
        }
        
        // Positional arguments
        positionalArgs_.push_back(arg);
    }
    
    if (!validateArguments()) {
        success = false;
    }
    
    if (!validateConfig()) {
        printWarning("Configuration validation failed, using defaults");
    }
    
    if (verbose_) {
        NLM_LOG_INFO("Parsed " + std::to_string(parsedOptions_.size()) + " options");
        NLM_LOG_INFO("Parsed " + std::to_string(parsedFlags_.size()) + " flags");
        NLM_LOG_INFO("Parsed " + std::to_string(positionalArgs_.size()) + " positional arguments");
    }
    
    return success;
}

bool CommandLineInterface::hasOption(const std::string& option) const {
    return parsedOptions_.find(option) != parsedOptions_.end();
}

bool CommandLineInterface::isFlagSet(const std::string& flag) const {
    return parsedFlags_.find(flag) != parsedFlags_.end() && parsedFlags_.at(flag);
}

std::string CommandLineInterface::getStringOption(const std::string& option, const std::string& defaultValue) const {
    auto it = parsedOptions_.find(option);
    return it != parsedOptions_.end() ? it->second : defaultValue;
}

template<typename T>
std::optional<T> CommandLineInterface::getOption(const std::string& option) const {
    auto it = parsedOptions_.find(option);
    if (it == parsedOptions_.end()) {
        return std::nullopt;
    }
    
    const std::string& value = it->second;
    
    // Try to convert based on type T
    if constexpr (std::is_same_v<T, int>) {
        try {
            return std::stoi(value);
        } catch (...) {
            return std::nullopt;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        try {
            return std::stod(value);
        } catch (...) {
            return std::nullopt;
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes");
    } else if constexpr (std::is_same_v<T, std::string>) {
        return value;
    }
    
    return std::nullopt;
}

bool CommandLineInterface::getFlag(const std::string& flag, bool defaultValue) const {
    auto it = parsedFlags_.find(flag);
    return it != parsedFlags_.end() ? it->second : defaultValue;
}

void CommandLineInterface::printHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS] [POSITIONAL_ARGS]..." << std::endl;
    std::cout << std::endl;
    
    std::cout << "STANDARD OPTIONS:" << std::endl;
    std::cout << "  --help, -h              Show this help message and exit" << std::endl;
    std::cout << "  --version               Show version information and exit" << std::endl;
    std::cout << "  --benchmark             Run performance benchmark and exit" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << "  --debug, -d             Enable debug output" << std::endl;
    std::cout << "  --profile, -p           Enable performance profiling" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --config=FILE           Load configuration from FILE" << std::endl;
    std::cout << "  --output=FILE           Save configuration to FILE" << std::endl;
    std::cout << "  --key=value             Set configuration key to value" << std::endl;
    std::cout << "  -key value              Set configuration key to value (short form)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "COMMON CONFIGURATION KEYS:" << std::endl;
    std::cout << "  brain.neuron_count       Number of neurons in brain" << std::endl;
    std::cout << "  brain.synapse_density    Synapse density (0.0-1.0)" << std::endl;
    std::cout << "  world.width             World width" << std::endl;
    std::cout << "  world.height            World height" << std::endl;
    std::cout << "  world.vision_width      Vision sensor width" << std::endl;
    std::cout << "  world.vision_height     Vision sensor height" << std::endl;
    std::cout << "  performance.target_fps  Target frames per second" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PERFORMANCE OPTIONS:" << std::endl;
    std::cout << "  --benchmark             Run performance benchmark" << std::endl;
    std::cout << "  --profile               Enable performance profiling" << std::endl;
    std::cout << std::endl;
    
    std::cout << "DEBUG OPTIONS:" << std::endl;
    std::cout << "  --debug, -d             Enable debug output" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm --help                    Show help" << std::endl;
    std::cout << "  nlm --config=nlm_config.json  Load custom config" << std::endl;
    std::cout << "  nlm brain.neuron_count=2000    Set neuron count" << std::endl;
    std::cout << "  nlm --debug --verbose          Enable debugging" << std::endl;
    std::cout << "  nlm --benchmark               Run benchmark" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 6.0.0 (Phase 6 - Final Integration)" << std::endl;
    std::cout << "Author: Research Project" << std::endl;
    std::cout << "License: MIT" << std::endl;
    std::cout << std::endl;
    std::cout << "Built with C++20, using Leaky Integrate-and-Fire (LIF) neurons." << std::endl;
    std::cout << "Phase 6: Complete integration of all brain systems." << std::endl;
}

void CommandLineInterface::printConfigSummary() const {
    std::cout << "CURRENT CONFIGURATION:" << std::endl;
    std::cout << config_->summary() << std::endl;
}

bool CommandLineInterface::loadConfig(const std::string& filename) {
    if (filename.empty()) {
        printError("Invalid filename: empty string");
        return false;
    }
    
    return config_->loadFromFile(filename);
}

bool CommandLineInterface::saveConfig(const std::string& filename) const {
    if (filename.empty()) {
        printError("Invalid filename: empty string");
        return false;
    }
    
    return config_->saveToFile(filename);
}

std::string CommandLineInterface::getPerformanceStats() const {
    auto endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    double elapsedTime = endTime - startTime_;
    
    std::ostringstream oss;
    oss << "Performance Statistics:" << std::endl;
    oss << "  Elapsed Time: " << std::fixed << std::setprecision(3) << elapsedTime << " seconds" << std::endl;
    oss << "  Steps Completed: " << stepsCompleted_ << std::endl;
    oss << "  Neurons Fired: " << neuronsFired_ << std::endl;
    
    if (stepsCompleted_ > 0) {
        oss << "  Steps per Second: " << (stepsCompleted_ / elapsedTime) << std::endl;
        oss << "  Firing Rate: " << (neuronsFired_ / (double)stepsCompleted_) << " firings per step" << std::endl;
    }
    
    if (profiling_) {
        oss << "  Profiling: Enabled" << std::endl;
        // Add more detailed profiling information if needed
    }
    
    return oss.str();
}

int CommandLineInterface::executeSimulation() const {
    if (debug_) {
        NLM_LOG_DEBUG("Starting simulation with current configuration");
    }
    
    // Update end time
    endTime_ = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    
    std::cout << "Simulation not implemented in this CLI version." << std::endl;
    std::cout << "This is a placeholder implementation." << std::endl;
    std::cout << getPerformanceStats() << std::endl;
    
    return 0;
}

void CommandLineInterface::registerStandardOptions() {
    // Standard options are registered in parseArguments
}

void CommandLineInterface::registerDebugOptions() {
    // Debug options are registered in parseArguments
}

void CommandLineInterface::registerPerformanceOptions() {
    // Performance options are registered in parseArguments
}

void CommandLineInterface::registerOutputOptions() {
    // Output options are registered in parseArguments
}

void CommandLineInterface::printError(const std::string& message) const {
    std::cerr << "ERROR: " << message << std::endl;
}

void CommandLineInterface::printWarning(const std::string& message) const {
    std::cerr << "WARNING: " << message << std::endl;
}

bool CommandLineInterface::validateArguments() const {
    bool valid = true;
    
    // Check for conflicting options
    if (helpRequested_ && (versionRequested_ || benchmarkRequested_)) {
        printWarning("Help requested, but version/benchmark also specified");
    }
    
    // Validate option values
    if (hasOption("brain.neuron_count")) {
        auto neuronCount = getOption<int64_t>("brain.neuron_count");
        if (neuronCount && *neuronCount <= 0) {
            printError("brain.neuron_count must be positive");
            valid = false;
        }
    }
    
    if (hasOption("brain.synapse_density")) {
        auto density = getOption<double>("brain.synapse_density");
        if (density && (*density < 0.0 || *density > 1.0)) {
            printError("brain.synapse_density must be between 0.0 and 1.0");
            valid = false;
        }
    }
    
    return valid;
}

bool CommandLineInterface::validateConfig() const {
    // Basic validation
    if (!config_->has("brain.neuron_count")) {
        NLM_LOG_WARN("brain.neuron_count not set, using default");
    }
    
    if (!config_->has("brain.synapse_density")) {
        NLM_LOG_WARN("brain.synapse_density not set, using default");
    }
    
    if (!config_->has("world.width") || !config_->has("world.height")) {
        NLM_LOG_WARN("World dimensions not set, using defaults");
    }
    
    return true;
}

} // namespace nlm