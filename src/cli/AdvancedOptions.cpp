// Advanced options implementation

#include "AdvancedOptions.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <ctime>

namespace nlm {
namespace cli {

// AdvancedOptions implementation

bool AdvancedOptions::isValid() const {
    if (neuronCount <= 0) return false;
    if (maxSteps <= 0) return false;
    if (timestep <= 0) return false;
    if (memoryLimit < 0) return false;
    if (parallelThreads <= 0) return false;
    
    if (outputFormat != "text" && outputFormat != "json" && outputFormat != "csv") {
        return false;
    }
    
    if (debugLevel != "error" && debugLevel != "warning" && debugLevel != "info" && debugLevel != "debug") {
        return false;
    }
    
    return true;
}

void AdvancedOptions::printOptions() const {
    std::cout << "Advanced Options Summary:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Max Steps: " << maxSteps << std::endl;
    std::cout << "  Timestep: " << timestep << "s" << std::endl;
    std::cout << "  Output File: " << (outputFile.empty() ? "(none)" : outputFile) << std::endl;
    std::cout << "  Output Format: " << outputFormat << std::endl;
    std::cout << "  Parallel Threads: " << parallelThreads << std::endl;
    std::cout << "  Memory Limit: " << memoryLimit << " MB" << std::endl;
    std::cout << "  Debug Level: " << debugLevel << std::endl;
    
    if (!pluginPaths.empty()) {
        std::cout << "  Plugin Paths:" << std::endl;
        for (const auto& path : pluginPaths) {
            std::cout << "    " << path << std::endl;
        }
    }
}

std::string AdvancedOptions::toString() const {
    std::stringstream ss;
    ss << "AdvancedOptions{" << std::endl;
    ss << "  neuronCount: " << neuronCount << "," << std::endl;
    ss << "  maxSteps: " << maxSteps << "," << std::endl;
    ss << "  timestep: " << timestep << "," << std::endl;
    ss << "  outputFile: \"" << outputFile << "\"," << std::endl;
    ss << "  outputFormat: \"" << outputFormat << "\"," << std::endl;
    ss << "  quietMode: " << (quietMode ? "true" : "false") << "," << std::endl;
    ss << "  verboseMode: " << (verboseMode ? "true" : "false") << "," << std::endl;
    ss << "  enableProgress: " << (enableProgress ? "true" : "false") << "," << std::endl;
    ss << "  parallelThreads: " << parallelThreads << "," << std::endl;
    ss << "}";
    return ss.str();
}

AdvancedOptions AdvancedOptions::fromEnvironment() {
    AdvancedOptions options;
    
    // Load from environment variables
    std::string neuronCount = Environment::getVar("NLM_NEURON_COUNT", "500");
    std::string maxSteps = Environment::getVar("NLM_MAX_STEPS", "1000");
    std::string timestep = Environment::getVar("NLM_TIMESTEP", "0.001");
    std::string outputFormat = Environment::getVar("NLM_OUTPUT_FORMAT", "text");
    std::string parallelThreads = Environment::getVar("NLM_THREADS", "1");
    
    try {
        options.neuronCount = std::stoi(neuronCount);
        options.maxSteps = std::stoi(maxSteps);
        options.timestep = std::stod(timestep);
        options.outputFormat = outputFormat;
        options.parallelThreads = std::stoi(parallelThreads);
    } catch (...) {
        // Use defaults if parsing fails
    }
    
    return options;
}

AdvancedOptions AdvancedOptions::fromConfigFile(const std::string& filename) {
    AdvancedOptions options;
    options.loadFromFile(filename);
    return options;
}

bool AdvancedOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--neuron-count" && i + 1 < args.size()) {
            try { neuronCount = std::stoi(args[++i]); }
            catch (...) { return false; }
        } else if (arg == "--max-steps" && i + 1 < args.size()) {
            try { maxSteps = std::stoi(args[++i]); }
            catch (...) { return false; }
        } else if (arg == "--timestep" && i + 1 < args.size()) {
            try { timestep = std::stod(args[++i]); }
            catch (...) { return false; }
        } else if (arg == "--output-file" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--output-format" && i + 1 < args.size()) {
            outputFormat = args[++i];
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg == "--enable-progress") {
            enableProgress = true;
        } else if (arg == "--parallel" && i + 1 < args.size()) {
            try { parallelThreads = std::stoi(args[++i]); }
            catch (...) { return false; }
        } else if (arg == "--memory-limit" && i + 1 < args.size()) {
            try { memoryLimit = std::stoi(args[++i]); }
            catch (...) { return false; }
        } else if (arg == "--plugin-path" && i + 1 < args.size()) {
            pluginPaths.push_back(args[++i]);
        } else if (arg == "--enable-profiling") {
            enableProfiling = true;
        } else if (arg == "--enable-benchmarking") {
            enableBenchmarking = true;
        } else if (arg == "--debug-level" && i + 1 < args.size()) {
            debugLevel = args[++i];
        } else if (arg == "--enable-validation") {
            enableValidation = true;
        } else if (arg == "--enable-logging") {
            enableLogging = true;
        } else if (arg == "--config-file" && i + 1 < args.size()) {
            configFile = args[++i];
        }
    }
    
    return true;
}

bool AdvancedOptions::parseOption(const std::string& arg, size_t& currentPos, const std::vector<std::string>& args) {
    if (arg == "--neuron-count" && currentPos + 1 < args.size()) {
        try { neuronCount = std::stoi(args[++currentPos]); }
        catch (...) { return false; }
        return true;
    } else if (arg == "--max-steps" && currentPos + 1 < args.size()) {
        try { maxSteps = std::stoi(args[++currentPos]); }
        catch (...) { return false; }
        return true;
    } else if (arg == "--timestep" && currentPos + 1 < args.size()) {
        try { timestep = std::stod(args[++currentPos]); }
        catch (...) { return false; }
        return true;
    } else if (arg == "--output-file" && currentPos + 1 < args.size()) {
        outputFile = args[++currentPos];
        return true;
    } else if (arg == "--output-format" && currentPos + 1 < args.size()) {
        outputFormat = args[++currentPos];
        return true;
    } else if (arg == "--quiet" || arg == "-q") {
        quietMode = true;
        return true;
    } else if (arg == "--verbose" || arg == "-v") {
        verboseMode = true;
        return true;
    } else if (arg == "--enable-progress") {
        enableProgress = true;
        return true;
    } else if (arg == "--parallel" && currentPos + 1 < args.size()) {
        try { parallelThreads = std::stoi(args[++currentPos]); }
        catch (...) { return false; }
        return true;
    } else if (arg == "--memory-limit" && currentPos + 1 < args.size()) {
        try { memoryLimit = std::stoi(args[++currentPos]); }
        catch (...) { return false; }
        return true;
    } else if (arg == "--plugin-path" && currentPos + 1 < args.size()) {
        pluginPaths.push_back(args[++currentPos]);
        return true;
    } else if (arg == "--enable-profiling") {
        enableProfiling = true;
        return true;
    } else if (arg == "--enable-benchmarking") {
        enableBenchmarking = true;
        return true;
    } else if (arg == "--debug-level" && currentPos + 1 < args.size()) {
        debugLevel = args[++currentPos];
        return true;
    } else if (arg == "--enable-validation") {
        enableValidation = true;
        return true;
    } else if (arg == "--enable-logging") {
        enableLogging = true;
        return true;
    } else if (arg == "--config-file" && currentPos + 1 < args.size()) {
        configFile = args[++currentPos];
        return true;
    }
    
    return false;
}

std::string AdvancedOptions::getOptionValue(const std::string& optionName) const {
    if (optionName == "neuron_count") return std::to_string(neuronCount);
    if (optionName == "max_steps") return std::to_string(maxSteps);
    if (optionName == "timestep") return std::to_string(timestep);
    if (optionName == "output_file") return outputFile;
    if (optionName == "output_format") return outputFormat;
    if (optionName == "parallel_threads") return std::to_string(parallelThreads);
    if (optionName == "memory_limit") return std::to_string(memoryLimit);
    if (optionName == "config_file") return configFile;
    
    return "";
}

bool AdvancedOptions::hasOption(const std::string& optionName) const {
    if (optionName == "neuron_count") return neuronCount > 0;
    if (optionName == "max_steps") return maxSteps > 0;
    if (optionName == "timestep") return timestep > 0;
    if (optionName == "output_file") return !outputFile.empty();
    if (optionName == "output_format") return !outputFormat.empty();
    if (optionName == "parallel_threads") return parallelThreads > 0;
    if (optionName == "memory_limit") return memoryLimit >= 0;
    if (optionName == "config_file") return !configFile.empty();
    
    return false;
}

bool AdvancedOptions::validate() const {
    if (!isValid()) {
        return false;
    }
    
    // Validate plugin paths
    for (const auto& path : pluginPaths) {
        if (path.empty()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> AdvancedOptions::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (neuronCount <= 0) {
        errors.push_back("Neuron count must be positive");
    }
    
    if (maxSteps <= 0) {
        errors.push_back("Max steps must be positive");
    }
    
    if (timestep <= 0) {
        errors.push_back("Timestep must be positive");
    }
    
    if (memoryLimit < 0) {
        errors.push_back("Memory limit must be non-negative");
    }
    
    if (parallelThreads <= 0) {
        errors.push_back("Parallel threads must be positive");
    }
    
    if (outputFormat != "text" && outputFormat != "json" && outputFormat != "csv") {
        errors.push_back("Invalid output format. Must be 'text', 'json', or 'csv'");
    }
    
    if (debugLevel != "error" && debugLevel != "warning" && debugLevel != "info" && debugLevel != "debug") {
        errors.push_back("Invalid debug level. Must be 'error', 'warning', 'info', or 'debug'");
    }
    
    for (const auto& path : pluginPaths) {
        if (path.empty()) {
            errors.push_back("Plugin path cannot be empty");
        }
    }
    
    return errors;
}

bool AdvancedOptions::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Set values based on key
        if (key == "neuron_count") {
            try { neuronCount = std::stoi(value); }
            catch (...) {}
        } else if (key == "max_steps") {
            try { maxSteps = std::stoi(value); }
            catch (...) {}
        } else if (key == "timestep") {
            try { timestep = std::stod(value); }
            catch (...) {}
        } else if (key == "output_file") {
            outputFile = value;
        } else if (key == "output_format") {
            outputFormat = value;
        } else if (key == "parallel_threads") {
            try { parallelThreads = std::stoi(value); }
            catch (...) {}
        } else if (key == "memory_limit") {
            try { memoryLimit = std::stoi(value); }
            catch (...) {}
        } else if (key == "config_file") {
            configFile = value;
        }
    }
    
    file.close();
    return true;
}

bool AdvancedOptions::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "neuron_count = " << neuronCount << std::endl;
    file << "max_steps = " << maxSteps << std::endl;
    file << "timestep = " << timestep << std::endl;
    file << "output_file = \"" << outputFile << "\"" << std::endl;
    file << "output_format = \"" << outputFormat << "\"" << std::endl;
    file << "parallel_threads = " << parallelThreads << std::endl;
    file << "memory_limit = " << memoryLimit << std::endl;
    file << "config_file = \"" << configFile << "\"" << std::endl;
    
    file.close();
    return true;
}

bool AdvancedOptions::loadFromEnvironment() {
    std::string neuronCount = Environment::getVar("NLM_NEURON_COUNT", "500");
    std::string maxSteps = Environment::getVar("NLM_MAX_STEPS", "1000");
    std::string timestep = Environment::getVar("NLM_TIMESTEP", "0.001");
    std::string outputFormat = Environment::getVar("NLM_OUTPUT_FORMAT", "text");
    std::string parallelThreads = Environment::getVar("NLM_THREADS", "1");
    std::string configFile = Environment::getVar("NLM_CONFIG_FILE", "configs/default.cfg");
    
    try {
        neuronCount = neuronCount.empty() ? "500" : neuronCount;
        maxSteps = maxSteps.empty() ? "1000" : maxSteps;
        timestep = timestep.empty() ? "0.001" : timestep;
        outputFormat = outputFormat.empty() ? "text" : outputFormat;
        parallelThreads = parallelThreads.empty() ? "1" : parallelThreads;
        
        this->neuronCount = std::stoi(neuronCount);
        this->maxSteps = std::stoi(maxSteps);
        this->timestep = std::stod(timestep);
        this->outputFormat = outputFormat;
        this->parallelThreads = std::stoi(parallelThreads);
        this->configFile = configFile;
    } catch (...) {
        // Use defaults if parsing fails
    }
    
    return true;
}

void AdvancedOptions::mergeWithCommandLine(const std::vector<std::string>& args) {
    parse(args);
}

// CommandOptions implementation

// SimulationCommandOptions implementation

void SimulationCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--neuron-count" && i + 1 < args.size()) {
            try { neuronCount = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--max-steps" && i + 1 < args.size()) {
            try { maxSteps = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--timestep" && i + 1 < args.size()) {
            try { timestep = std::stod(args[++i]); }
            catch (...) {}
        } else if (arg == "--config-file" && i + 1 < args.size()) {
            configFile = args[++i];
        } else if (arg == "--output-file" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--output-format" && i + 1 < args.size()) {
            outputFormat = args[++i];
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--enable-progress") {
            enableProgress = true;
        } else if (arg == "--parallel" && i + 1 < args.size()) {
            try { parallelThreads = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--memory-limit" && i + 1 < args.size()) {
            try { memoryLimit = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--plugin-path" && i + 1 < args.size()) {
            pluginPaths.push_back(args[++i]);
        } else if (arg == "--enable-profiling") {
            enableProfiling = true;
        } else if (arg == "--enable-benchmarking") {
            enableBenchmarking = true;
        } else if (arg == "--debug-level" && i + 1 < args.size()) {
            debugLevel = args[++i];
        } else if (arg == "--enable-validation") {
            enableValidation = true;
        } else if (arg == "--enable-logging") {
            enableLogging = true;
        }
    }
}

bool SimulationCommandOptions::validate() const {
    if (neuronCount <= 0) return false;
    if (maxSteps <= 0) return false;
    if (timestep <= 0) return false;
    if (memoryLimit < 0) return false;
    if (parallelThreads <= 0) return false;
    
    if (outputFormat != "text" && outputFormat != "json" && outputFormat != "csv") {
        return false;
    }
    
    if (debugLevel != "error" && debugLevel != "warning" && debugLevel != "info" && debugLevel != "debug") {
        return false;
    }
    
    return true;
}

// ConfigCommandOptions implementation

void ConfigCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--list") {
            list = true;
        } else if (arg == "--config-file" && i + 1 < args.size()) {
            configFile = args[++i];
        } else if (arg == "--output-file" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--validate") {
            validate = true;
        } else if (arg == "--export-config") {
            exportConfig = true;
        }
    }
}

bool ConfigCommandOptions::validate() const {
    return true;
}

// DebugCommandOptions implementation

void DebugCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--attach") {
            attach = true;
        } else if (arg == "--profile" || arg == "-p") {
            profile = true;
        } else if (arg == "--validate" || arg == "-v") {
            validate = true;
        } else if (arg == "--memory") {
            memory = true;
        } else if (arg == "--threads") {
            threads = true;
        } else if (arg == "--log-level" && i + 1 < args.size()) {
            logLevel = args[++i];
        }
    }
}

bool DebugCommandOptions::validate() const {
    if (logLevel != "error" && logLevel != "warning" && logLevel != "info" && logLevel != "debug") {
        return false;
    }
    
    return true;
}

// ValidateCommandOptions implementation

void ValidateCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--memory") {
            checkMemory = true;
        } else if (arg == "--threads") {
            checkThreads = true;
        } else if (arg == "--config") {
            checkConfig = true;
        } else if (arg == "--plugins") {
            checkPlugins = true;
        }
    }
}

bool ValidateCommandOptions::validate() const {
    return true;
}

// PluginCommandOptions implementation

void PluginCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--list") {
            list = true;
        } else if (arg == "--load" && i + 1 < args.size()) {
            loadPath = args[++i];
        } else if (arg == "--unload" && i + 1 < args.size()) {
            unloadName = args[++i];
        } else if (arg == "--enable" && i + 1 < args.size()) {
            enableName = args[++i];
        } else if (arg == "--disable" && i + 1 < args.size()) {
            disableName = args[++i];
        } else if (arg == "--info" && i + 1 < args.size()) {
            infoName = args[++i];
        }
    }
}

bool PluginCommandOptions::validate() const {
    return true;
}

// ProfileCommandOptions implementation

void ProfileCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--benchmark" || arg == "-b") {
            benchmark = true;
        } else if (arg == "--profile" || arg == "-p") {
            profile = true;
        } else if (arg == "--analyze" || arg == "-a") {
            analyze = true;
        } else if (arg == "--output-file" && i + 1 < args.size()) {
            outputFile = args[++i];
        }
    }
}

bool ProfileCommandOptions::validate() const {
    return true;
}

// BenchmarkCommandOptions implementation

void BenchmarkCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--neuron-count" && i + 1 < args.size()) {
            try { neuronCount = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--steps" && i + 1 < args.size()) {
            try { steps = std::stoi(args[++i]); }
            catch (...) {}
        } else if (arg == "--threads" && i + 1 < args.size()) {
            try { threads = std::stoi(args[++i]); }
            catch (...) {}
        }
    }
}

bool BenchmarkCommandOptions::validate() const {
    if (neuronCount <= 0) return false;
    if (steps <= 0) return false;
    if (threads <= 0) return false;
    
    return true;
}

// AnalyzeCommandOptions implementation

void AnalyzeCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--type" && i + 1 < args.size()) {
            analysisType = args[++i];
        }
    }
}

bool AnalyzeCommandOptions::validate() const {
    if (analysisType != "performance" && analysisType != "memory" && 
        analysisType != "network" && analysisType != "system") {
        return false;
    }
    
    return true;
}

// UtilityCommandOptions implementation

void UtilityCommandOptions::parse(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--convert") {
            convert = true;
        } else if (arg == "--validate") {
            validate = true;
        } else if (arg == "--format-info") {
            formatInfo = true;
        }
    }
}

bool UtilityCommandOptions::validate() const {
    return true;
}

} // namespace cli
} // namespace nlm
