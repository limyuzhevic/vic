// NLM Command Line Interface Implementation
// Unified CLI for all NLM executables with advanced features

#include "NLMCLI.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace nlm {

NLMCLI::NLMCLI() {
    // Initialize validation rules for configuration
    initializeValidationRules();
}

NLMCLI::~NLMCLI() {
    // Cleanup if needed
}

void NLMCLI::initializeValidationRules() {
    validationRules = {
        {"random_seed", "int", "range:0:1000000", true, "42"},
        {"simulation_timestep", "float", "range:0.0001:0.1", true, "0.001"},
        {"neuron_count", "int", "range:10:10000", true, "1000"},
        {"region_count", "int", "range:1:100", true, "2"},
        {"connection_probability", "float", "range:0.0:1.0", true, "0.1"},
        {"environment_width", "int", "range:10:1000", true, "20"},
        {"environment_height", "int", "range:10:1000", true, "20"},
        {"plasticity.stdp.enable", "bool", "enum:true,false", false, "true"},
        {"neuromod.dopamine.scale", "float", "range:0.0:10.0", false, "1.0"},
        {"benchmark", "bool", "enum:true,false", false, "false"},
        {"verbose", "bool", "enum:true,false", false, "false"},
        {"quiet", "bool", "enum:true,false", false, "false"}
    };
}

bool NLMCLI::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            displayHelp();
            return false;
        } else if (arg == "--version") {
            displayVersion();
            return false;
        } else if (arg == "--advanced-help") {
            displayAdvancedHelp();
            return false;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg.substr(0, 9) == "--config=") {
            options.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.substr(0, 4) == "--log" && arg.find("=") != std::string::npos) {
            options.logLevel = arg.substr(arg.find("=") + 1);
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg.substr(0, 16) == "--benchmark-mode" && arg.find("=") != std::string::npos) {
            options.benchmarkMode = arg.substr(arg.find("=") + 1);
        } else if (arg == "--benchmark-mode" && i + 1 < argc) {
            options.benchmarkMode = argv[++i];
        } else if (arg.substr(0, 11) == "--num-steps" && arg.find("=") != std::string::npos) {
            double value;
            if (parseNumericValue(arg.substr(arg.find("=") + 1), value)) {
                options.numSteps = static_cast<int>(value);
            }
        } else if (arg == "--num-steps" && i + 1 < argc) {
            double value;
            if (parseNumericValue(argv[++i], value)) {
                options.numSteps = static_cast<int>(value);
            }
        } else if (arg.substr(0, 11) == "--timestep" && arg.find("=") != std::string::npos) {
            double value;
            if (parseNumericValue(arg.substr(arg.find("=") + 1), value)) {
                options.timestep = value;
            }
        } else if (arg == "--timestep" && i + 1 < argc) {
            double value;
            if (parseNumericValue(argv[++i], value)) {
                options.timestep = value;
            }
        } else if (arg.substr(0, 13) == "--random-seed" && arg.find("=") != std::string::npos) {
            double value;
            if (parseNumericValue(arg.substr(arg.find("=") + 1), value)) {
                options.randomSeed = static_cast<int>(value);
            }
        } else if (arg == "--random-seed" && i + 1 < argc) {
            double value;
            if (parseNumericValue(argv[++i], value)) {
                options.randomSeed = static_cast<int>(value);
            }
        } else if (arg.substr(0, 9) == "--output=") {
            options.outputFile = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg.substr(0, 17) == "--experiment-type" && arg.find("=") != std::string::npos) {
            int value;
            if (parseNumericValue(arg.substr(arg.find("=") + 1), value)) {
                options.experimentType = value;
            }
        } else if (arg == "--experiment-type" && i + 1 < argc) {
            int value;
            if (parseNumericValue(argv[++i], value)) {
                options.experimentType = value;
            }
        } else if (arg == "--run-all-phases") {
            options.runAllPhases = true;
        } else if (arg == "--validate-config") {
            options.validateConfig = true;
        } else if (arg == "--validate-connections") {
            options.validateConnections = true;
        } else if (arg == "--profile-performance") {
            options.profilePerformance = true;
        } else if (arg.substr(0, 18) == "--profile-output" && arg.find("=") != std::string::npos) {
            options.profileOutput = arg.substr(arg.find("=") + 1);
        } else if (arg == "--profile-output" && i + 1 < argc) {
            options.profileOutput = argv[++i];
        } else if (arg == "--enable-checkpointing") {
            options.enableCheckpointing = true;
        } else if (arg == "--enable-replay") {
            options.enableReplay = true;
        } else if (arg == "--enable-development") {
            options.enableDevelopment = true;
        } else if (arg.substr(0, 20) == "--max-memory-mb" && arg.find("=") != std::string::npos) {
            double value;
            if (parseNumericValue(arg.substr(arg.find("=") + 1), value)) {
                options.maxMemoryMB = static_cast<int>(value);
            }
        } else if (arg == "--max-memory-mb" && i + 1 < argc) {
            double value;
            if (parseNumericValue(argv[++i], value)) {
                options.maxMemoryMB = static_cast<int>(value);
            }
        } else if (arg == "--threads" && i + 1 < argc) {
            options.threads = std::stoi(argv[++i]);
        } else if (arg.substr(0, 17) == "--export-format" && arg.find("=") != std::string::npos) {
            options.exportFormat = arg.substr(arg.find("=") + 1);
        } else if (arg == "--export-format" && i + 1 < argc) {
            options.exportFormat = argv[++i];
        } else if (arg == "--export-neural-data") {
            options.exportNeuralData = true;
        } else if (arg == "--export-memory-data") {
            options.exportMemoryData = true;
        } else if (arg == "--export-configuration") {
            options.exportConfiguration = true;
        } else {
            // Unknown argument, add to experiment args
            options.experimentArgs.push_back(arg);
        }
    }
    
    return true;
}

bool NLMCLI::parseKeyValue(const std::string& arg, std::string& key, std::string& value) {
    size_t equalsPos = arg.find('=');
    if (equalsPos != std::string::npos) {
        key = arg.substr(0, equalsPos);
        value = arg.substr(equalsPos + 1);
        return true;
    }
    return false;
}

bool NLMCLI::parseNumericValue(const std::string& value, double& result) {
    try {
        result = std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

void NLMCLI::displayHelp() const {
    std::cout << "NLM — Neural Learning Machine — Command Line Interface" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS] [EXPERIMENT_ARGS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "BASIC OPTIONS:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --quiet                 Suppress all output except errors" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config=<file>         Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --log=<level>           Set log level (debug, info, warning, error)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "SIMULATION CONTROL:" << std::endl;
    std::cout << "  --num-steps=<n>         Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --timestep=<s>          Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --random-seed=<n>       Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPERIMENT SELECTION:" << std::endl;
    std::cout << "  --experiment-type=<n>   Experiment type: 0=Basic, 1=Phase3, 2=Phase4, 3=Phase6" << std::endl;
    std::cout << "  --run-all-phases        Run all phases sequentially" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PERFORMANCE OPTIONS:" << std::endl;
    std::cout << "  --benchmark             Enable benchmarking mode" << std::endl;
    std::cout << "  --benchmark-mode=<mode> Benchmark mode: quick, thorough, memory, cpu" << std::endl;
    std::cout << "  --max-memory-mb=<n>    Maximum memory usage in MB (default: 512)" << std::endl;
    std::cout << "  --threads=<n>           Number of threads to use (default: 1)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "VALIDATION AND DEBUGGING:" << std::endl;
    std::cout << "  --validate-config       Validate configuration file" << std::endl;
    std::cout << "  --validate-connections  Validate neural connections" << std::endl;
    std::cout << "  --profile-performance   Enable performance profiling" << std::endl;
    std::cout << "  --profile-output=<file> Output profile data to file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --enable-checkpointing  Enable checkpoint saving" << std::endl;
    std::cout << "  --enable-replay         Enable memory replay" << std::endl;
    std::cout << "  --enable-development    Enable developmental processes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPORT OPTIONS:" << std::endl;
    std::cout << "  --export-format=<fmt>   Export format: json, text, binary (default: json)" << std::endl;
    std::cout << "  --export-neural-data    Export neural state data" << std::endl;
    std::cout << "  --export-memory-data    Export memory state data" << std::endl;
    std::cout << "  --export-configuration   Export configuration data" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OUTPUT CONTROL:" << std::endl;
    std::cout << "  --output=<file>         Write output to file instead of stdout" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPERIMENT ARGUMENTS:" << std::endl;
    std::cout << "  Additional arguments are passed to the selected experiment" << std::endl;
    std::cout << std::endl;
    
    std::cout << "For more detailed help, use --advanced-help" << std::endl;
}

void NLMCLI::displayVersion() const {
    std::cout << "NLM v0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "C++20 Implementation with Python Bindings" << std::endl;
}

void NLMCLI::displayAdvancedHelp() const {
    displayHelp();
    std::cout << std::endl;
    std::cout << "ADVANCED USAGE EXAMPLES:" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 1: Run Phase 3 world interaction with custom settings:" << std::endl;
    std::cout << "  nlm --experiment-type=1 --num-steps=500 --timestep=0.01 \\" << std::endl;
    std::cout << "       --random-seed=12345 --benchmark --output=phase3_results.json" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 2: Run complete Phase 6 integration test with validation:" << std::endl;
    std::cout << "  nlm --experiment-type=3 --validate-config --validate-connections \\" << std::endl;
    std::cout << "       --profile-performance --enable-checkpointing --enable-replay" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 3: Run all phases in sequence with benchmarking:" << std::endl;
    std::cout << "  nlm --run-all-phases --benchmark --benchmark-mode=thorough \\" << std::endl;
    std::cout << "       --max-memory-mb=1024 --threads=4 --export-neural-data" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example 4: Quick validation and export configuration:" << std::endl;
    std::cout << "  nlm --validate-config --validate-connections --export-configuration \\" << std::endl;
    std::cout << "       --export-format=json --quiet" << std::endl;
    std::cout << std::endl;
}

bool NLMCLI::validateOptions() const {
    // Check configuration file exists
    if (!fileExists(options.configFile)) {
        std::cerr << "ERROR: Configuration file not found: " << options.configFile << std::endl;
        return false;
    }
    
    // Validate log level
    std::string lowerLogLevel = toLowerCase(options.logLevel);
    if (lowerLogLevel != "debug" && lowerLogLevel != "info" && 
        lowerLogLevel != "warning" && lowerLogLevel != "error") {
        std::cerr << "ERROR: Invalid log level: " << options.logLevel << std::endl;
        return false;
    }
    
    // Validate benchmark mode
    if (options.benchmark) {
        std::string lowerBenchmarkMode = toLowerCase(options.benchmarkMode);
        if (lowerBenchmarkMode != "quick" && lowerBenchmarkMode != "thorough" && 
            lowerBenchmarkMode != "memory" && lowerBenchmarkMode != "cpu") {
            std::cerr << "ERROR: Invalid benchmark mode: " << options.benchmarkMode << std::endl;
            return false;
        }
    }
    
    // Validate experiment type
    if (options.experimentType < 0 || options.experimentType > 3) {
        std::cerr << "ERROR: Invalid experiment type: " << options.experimentType << std::endl;
        return false;
    }
    
    // Validate timestep
    if (options.timestep <= 0.0) {
        std::cerr << "ERROR: Invalid timestep: must be positive" << std::endl;
        return false;
    }
    
    // Validate num steps
    if (options.numSteps <= 0) {
        std::cerr << "ERROR: Invalid number of steps: must be positive" << std::endl;
        return false;
    }
    
    // Validate memory limit
    if (options.maxMemoryMB <= 0) {
        std::cerr << "ERROR: Invalid max memory: must be positive" << std::endl;
        return false;
    }
    
    // Validate threads
    if (options.threads <= 0) {
        std::cerr << "ERROR: Invalid thread count: must be positive" << std::endl;
        return false;
    }
    
    // Validate export format
    std::string lowerExportFormat = toLowerCase(options.exportFormat);
    if (lowerExportFormat != "json" && lowerExportFormat != "text" && 
        lowerExportFormat != "binary") {
        std::cerr << "ERROR: Invalid export format: " << options.exportFormat << std::endl;
        return false;
    }
    
    return true;
}

bool NLMCLI::processOptions() {
    // Load and configure logging
    auto logger = std::make_shared<Logger>();
    if (options.logLevel == "debug") {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Debug));
    } else if (options.logLevel == "info") {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    } else if (options.logLevel == "warning") {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Warning));
    } else if (options.logLevel == "error") {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Error));
    }
    Logger::setGlobal(logger);
    
    // Load configuration
    if (!loadConfiguration()) {
        return false;
    }
    
    // Apply command line overrides
    if (options.randomSeed != 42) {
        currentConfig->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::CommandLine);
    }
    
    if (options.timestep != 0.001) {
        currentConfig->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    }
    
    // Start performance monitoring if requested
    if (options.profilePerformance) {
        startPerformanceMonitoring();
    }
    
    return true;
}

bool NLMCLI::loadConfiguration() {
    currentConfig = std::make_shared<Config>();
    
    // Try to load from file (ignore if not found)
    if (fileExists(options.configFile)) {
        if (currentConfig->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Using default configuration (failed to load file)");
        }
    } else {
        NLM_LOG_WARNING("Configuration file not found: " + options.configFile);
        NLM_LOG_WARNING("Using default configuration");
    }
    
    return true;
}

bool NLMCLI::validateConfiguration() const {
    if (!options.validateConfig) {
        return true; // Skip validation if not requested
    }
    
    bool allValid = true;
    
    // Check required validation rules
    for (const auto& rule : validationRules) {
        if (!rule.required) continue;
        
        // Check if key exists in config
        if (!currentConfig->has(rule.key)) {
            std::cerr << "ERROR: Missing required configuration key: " << rule.key << std::endl;
            allValid = false;
            continue;
        }
        
        // Apply validation rule
        if (!applyValidationRule(rule)) {
            std::cerr << "ERROR: Configuration validation failed for key: " << rule.key << std::endl;
            allValid = false;
        }
    }
    
    // Validate connections if requested
    if (options.validateConnections && currentConfig) {
        if (!validateNeuralConnections()) {
            std::cerr << "ERROR: Neural connection validation failed" << std::endl;
            allValid = false;
        }
    }
    
    return allValid;
}

bool NLMCLI::applyValidationRule(const ConfigValidationRule& rule) const {
    try {
        std::string valueStr;
        if (currentConfig->has(rule.key)) {
            if (rule.type == "int") {
                int64_t intVal = currentConfig->getOr<int64_t>(rule.key, 0);
                valueStr = std::to_string(intVal);
            } else if (rule.type == "float") {
                float floatVal = currentConfig->getOr<float>(rule.key, 0.0f);
                valueStr = std::to_string(floatVal);
            } else if (rule.type == "bool") {
                bool boolVal = currentConfig->getOr<bool>(rule.key, false);
                valueStr = boolVal ? "true" : "false";
            } else if (rule.type == "string") {
                valueStr = currentConfig->getOr<std::string>(rule.key, "");
            }
        } else {
            // Use default value
            valueStr = rule.defaultValue;
        }
        
        // Apply type-specific validation
        if (rule.validation.find("range:") == 0) {
            // Parse range:range:min:max
            size_t colon1 = rule.validation.find(':');
            size_t colon2 = rule.validation.find(':', colon1 + 1);
            if (colon1 != std::string::npos && colon2 != std::string::npos) {
                double min = std::stod(rule.validation.substr(colon1 + 1, colon2 - colon1 - 1));
                double max = std::stod(rule.validation.substr(colon2 + 1));
                double val = std::stod(valueStr);
                return (val >= min && val <= max);
            }
        } else if (rule.validation.find("enum:") == 0) {
            // Parse enum:enum:value1:value2:...
            std::string enumValues = rule.validation.substr(5);
            std::vector<std::string> enums = split(enumValues, ':');
            return std::find(enums.begin(), enums.end(), valueStr) != enums.end();
        }
        
        return true; // No validation applied or unrecognized validation format
    } catch (...) {
        return false;
    }
}

bool NLMCLI::validateNeuralConnections() const {
    // Placeholder for neural connection validation
    // In a real implementation, this would validate the brain's connectivity
    NLM_LOG_INFO("Validating neural connections...")
    
    // Check for isolated neurons, disconnected regions, etc.
    // For now, just return true as a placeholder
    return true;
}

void NLMCLI::startPerformanceMonitoring() {
    startTime = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Performance monitoring started");
}

void NLMCLI::stopPerformanceMonitoring() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    metrics.totalWallClockTime = duration.count();
    NLM_LOG_INFO("Performance monitoring stopped. Total time: " + std::to_string(metrics.totalWallClockTime) + "s");
}

void NLMCLI::printPerformanceReport() const {
    std::cout << std::endl;
    std::cout << "=== PERFORMANCE REPORT ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Wall clock time:    " << metrics.totalWallClockTime << "s" << std::endl;
    std::cout << "CPU time:            " << metrics.cpuTime << "s" << std::endl;
    std::cout << "Peak memory:         " << (metrics.peakMemoryUsage / 1024 / 1024) << " MB" << std::endl;
    std::cout << "Total allocated:     " << (metrics.totalMemoryAllocated / 1024 / 1024) << " MB" << std::endl;
    
    if (!metrics.stepTimes.empty()) {
        double avgStepTime = 0.0;
        for (double stepTime : metrics.stepTimes) {
            avgStepTime += stepTime;
        }
        avgStepTime /= metrics.stepTimes.size();
        
        std::cout << "Average step time:  " << avgStepTime << "s" << std::endl;
        std::cout << "Steps per second:    " << (1.0 / avgStepTime) << " steps/s" << std::endl;
    }
    
    std::cout << std::endl;
}

void NLMCLI::recordStepTime(double stepTime) {
    metrics.stepTimes.push_back(stepTime);
    metrics.cpuTime += stepTime;
    
    // Update peak memory (simplified)
    size_t currentMemory = metrics.totalMemoryAllocated + (stepTime * 1024 * 1024); // Placeholder
    if (currentMemory > metrics.peakMemoryUsage) {
        metrics.peakMemoryUsage = currentMemory;
    }
}

void NLMCLI::printProgress() const {
    if (options.quiet) return;
    
    int progress = (options.numSteps > 0) ? (static_cast<int>(metrics.stepTimes.size()) * 100 / options.numSteps) : 0;
    std::cout << "\rProgress: [" << std::string(progress / 10, '=') << std::string(10 - progress / 10, ' ') 
              << "] " << progress << "% (" << metrics.stepTimes.size() << "/" << options.numSteps 
              << " steps)" << std::flush;
    
    if (metrics.stepTimes.size() >= static_cast<size_t>(options.numSteps)) {
        std::cout << std::endl;
    }
}

bool NLMCLI::exportResults() const {
    std::string exportFormat = toLowerCase(options.exportFormat);
    
    if (exportFormat == "json") {
        return exportToJSON(options.outputFile.empty() ? "nlm_results.json" : options.outputFile);
    } else if (exportFormat == "text") {
        return exportToText(options.outputFile.empty() ? "nlm_results.txt" : options.outputFile);
    } else if (exportFormat == "binary") {
        return exportToBinary(options.outputFile.empty() ? "nlm_results.bin" : options.outputFile);
    }
    
    return false;
}

bool NLMCLI::exportNeuralState() const {
    std::cout << "Exporting neural state data..." << std::endl;
    // Implementation for exporting neural state
    return true;
}

bool NLMCLI::exportMemoryState() const {
    std::cout << "Exporting memory state data..." << std::endl;
    // Implementation for exporting memory state
    return true;
}

bool NLMCLI::exportConfigurationState() const {
    std::cout << "Exporting configuration data..." << std::endl;
    // Implementation for exporting configuration
    return true;
}

bool NLMCLI::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open output file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "{
    \"nlm_version\": \"0.1.0\",
    \"timestamp\": \"" << getCurrentTimestamp() << "\",
    \"configuration\": {},
    \"performance_metrics\": {},
    \"neural_state\": {},
    \"memory_state\": {}
}";
    
    file.close();
    return true;
}

bool NLMCLI::exportToText(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open output file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "NLM Simulation Results" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    file << "Version: NLM v0.1.0" << std::endl;
    file << "Timestamp: " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    // Write configuration
    file << "Configuration:" << std::endl;
    if (currentConfig) {
        // Write all configuration values
        file << "(Configuration data not available in text export)" << std::endl;
    }
    file << std::endl;
    
    // Write performance metrics
    file << "Performance Metrics:" << std::endl;
    file << "  Wall clock time: " << metrics.totalWallClockTime << "s" << std::endl;
    file << "  CPU time: " << metrics.cpuTime << "s" << std::endl;
    file << "  Peak memory: " << (metrics.peakMemoryUsage / 1024 / 1024) << " MB" << std::endl;
    file << "  Steps per second: " << (metrics.stepTimes.empty() ? 0 : (1.0 / metrics.avgStepTime())) << " steps/s" << std::endl;
    file << std::endl;
    
    file.close();
    return true;
}

bool NLMCLI::exportToBinary(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "ERROR: Cannot open output file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write binary data (placeholder)
    // In a real implementation, this would write serialized brain state, memory state, etc.
    file << "Binary NLM results placeholder";
    
    file.close();
    return true;
}

std::string NLMCLI::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string NLMCLI::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool NLMCLI::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::vector<std::string> NLMCLI::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

std::string NLMCLI::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
