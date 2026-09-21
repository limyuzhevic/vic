// =============================================================================
// ENHANCED COMMAND LINE INTERFACE AND UTILITY FUNCTIONS
// =============================================================================
//
// This module provides enhanced command-line parsing, validation, and utility functions
// for the NLM system. It includes:
// - Command line argument parsing with validation
// - Configuration validation and schema checking
// - Enhanced logging and status reporting
// - Progress tracking and progress bar utilities
// - Performance monitoring utilities
// - Configuration file utilities
// - Validation utilities for configuration parameters
//
// All features are designed to be optional and backward compatible.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <functional>
#include <iomanip>
#include <memory>
#include <mutex>
#include <cmath>
#include <variant>
#include <limits>
#include <filesystem>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

// =============================================================================
// ENHANCED COMMAND LINE PARSER
// =============================================================================

class AdvancedCLI {
public:
    struct ParsedCommand {
        std::string command;
        std::map<std::string, std::variant<int, double, std::string, bool>> arguments;
        std::vector<std::string> flags;
        std::string description;
        std::chrono::system_clock::time_point timestamp;
        
        ParsedCommand() : timestamp(std::chrono::system_clock::now()) {}
    };
    
    class ValidationError : public std::runtime_error {
    public:
        ValidationError(const std::string& message, const std::string& context = "")
            : std::runtime_error(message), context_(context) {}
        
        const std::string& getContext() const { return context_; }
    
    private:
        std::string context_;
    };
    
    AdvancedCLI() = default;
    
    // Parse command line arguments with type inference
    static std::vector<ParsedCommand> parse(int argc, char** argv) {
        std::vector<ParsedCommand> commands;
        
        for (int i = 1; i < argc; ++i) {
            ParsedCommand cmd;
            cmd.command = argv[i];
            
            // Parse flags
            while (i + 1 < argc && argv[i + 1][0] == '-') {
                i++;
                cmd.flags.push_back(argv[i]);
            }
            
            // Parse key=value arguments with type inference
            while (i + 1 < argc && argv[i + 1].find('=') != std::string::npos) {
                i++;
                std::string arg = argv[i];
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(0, pos);
                    std::string value = arg.substr(pos + 1);
                    
                    // Type inference
                    cmd.arguments[key] = inferType(value);
                }
            }
            
            commands.push_back(cmd);
        }
        
        return commands;
    }
    
    // Type inference for configuration values
    static std::variant<int, double, std::string, bool> inferType(const std::string& value) {
        // Try bool first
        if (value == "true" || value == "false") {
            return value == "true";
        }
        
        // Try int
        try {
            size_t pos;
            long long intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                return intValue;
            }
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            long double doubleValue = std::stold(value, &pos);
            if (pos == value.size()) {
                return static_cast<double>(doubleValue);
            }
        } catch (...) {}
        
        // Return as string
        return value;
    }
    
    // Get help text for command line options
    static std::string getHelpText() {
        return R"(
Enhanced NLM Command Line Options:

  General Options:
    --help, -h              Show this help message
    --version               Show version information
    --verbose, -v           Enable verbose output
    --quiet, -q             Suppress informational messages
    --log-level LEVEL       Set logging level (DEBUG, INFO, WARNING, ERROR)
    --log-file FILE          Log to file instead of console

  Configuration Options:
    --config FILE           Load configuration from file (JSON, YAML, or .cfg)
    --set KEY=VALUE         Set configuration value
    --validate              Validate configuration
    --schema FILE           Load configuration schema

  Execution Options:
    --benchmark NEURONS,STEPS  Run benchmark with specified neurons and steps
    --run-mode MODE          Execution mode (BENCHMARK, EXPERIMENT, DEMO)
    --max-steps STEPS       Maximum number of simulation steps
    --timestep TIME         Simulation timestep (default: 0.001)

  Advanced Options:
    --checkpoint INTERVAL    Save checkpoints every INTERVAL steps
    --compression LEVEL      Compression level for checkpoints (0-9)
    --memory-pool          Enable memory pool optimization
    --gpu                  Use GPU acceleration if available
    --threads N             Number of worker threads
    --progress              Show progress indicators
    --profile               Enable profiling mode
    --dry-run              Parse and validate without executing
    --list-benchmarks       List available benchmarks
    --get-config            Print current configuration

  Development Options:
    --debug                 Enable debug mode
    --trace                 Enable detailed tracing
    --watch FILE            Watch file for changes and reload
    --interactive          Start interactive console

  Example Usage:
    ./nlm --benchmark 1000,1000 --timestep 0.001 --progress
    ./nlm --config config.json --set neuron_count=2000 --validate
    ./nlm --run-mode EXPERIMENT --max-steps 5000 --checkpoint 1000
    ./nlm --help
    ./nlm --version
    ./nlm --dry-run --benchmark 500,500
    ./nlm --list-benchmarks
    ./nlm --get-config

  Benchmarks:
    Available benchmarks (neurons, steps):
    - 100,1000      : Small network test
    - 500,1000      : Medium network test
    - 1000,500      : Large network test
    - 2000,200      : Very large network test
    - 5000,100      : Extra large network test
    - 10000,50      : Massive network test
    - 20000,20      : Extremely large network test
    - 50000,10      : Ultra-large network test

  Configuration Keys:
    - neuron_count (int): Number of neurons in the network
    - region_count (int): Number of brain regions
    - connection_probability (float): Synapse formation probability
    - simulation_timestep (double): Time step size in seconds
    - random_seed (int64): Random number generator seed
    - stdp_ltp_weight (float): STDP long-term potentiation weight
    - stdp_ltd_weight (float): STDP long-term depression weight
    - synaptogenesis_rate (float): Rate of new synapse formation
    - pruning_rate (float): Rate of synapse removal
    - replay_interval (int): Memory replay interval
    - consolidation_interval (int): Memory consolidation interval
    - checkpoint_dir (string): Directory for checkpoint files
    - checkpoint_interval (int): Checkpoint saving interval
    - max_memory_mb (int): Maximum memory usage in MB
    - log_to_file (bool): Enable file logging
    - enable_validation (bool): Enable configuration validation
    - development_enabled (bool): Enable developmental processes
)";
    }
};

// =============================================================================
// CONFIGURATION VALIDATION AND SCHEMA
// =============================================================================

class ConfigurationValidator {
public:
    struct ValidationRule {
        std::string key;
        std::string type;
        std::variant<std::monostate, int, double, std::string> min;
        std::variant<std::monostate, int, double, std::string> max;
        std::string description;
        bool required;
    };
    
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> violations;
    };
    
private:
    std::vector<ValidationRule> rules_;
    std::map<std::string, std::string> customSchema_;
    
public:
    ConfigurationValidator() = default;
    
    // Add validation rule
    void addRule(const ValidationRule& rule) {
        rules_.push_back(rule);
    }
    
    // Add validation rule using type-safe methods
    void addIntRule(const std::string& key, int min = std::numeric_limits<int>::min(),
                   int max = std::numeric_limits<int>::max(),
                   const std::string& description = "", bool required = true) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "int";
        rule.min = min;
        rule.max = max;
        rule.description = description;
        rule.required = required;
        addRule(rule);
    }
    
    void addFloatRule(const std::string& key, double min = -std::numeric_limits<double>::max(),
                     double max = std::numeric_limits<double>::max(),
                     const std::string& description = "", bool required = true) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "float";
        rule.min = min;
        rule.max = max;
        rule.description = description;
        rule.required = required;
        addRule(rule);
    }
    
    void addStringRule(const std::string& key, size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                      const std::string& description = "", bool required = true) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "string";
        rule.min = static_cast<int>(minLength);
        rule.max = static_cast<int>(maxLength);
        rule.description = description;
        rule.required = required;
        addRule(rule);
    }
    
    void addBoolRule(const std::string& key, const std::string& description = "", bool required = true) {
        ValidationRule rule;
        rule.key = key;
        rule.type = "bool";
        rule.description = description;
        rule.required = required;
        addRule(rule);
    }
    
    // Validate configuration
    ValidationResult validate(const Config& config) const {
        ValidationResult result;
        result.valid = true;
        
        for (const auto& rule : rules_) {
            // Check if key exists
            if (rule.required && !config.has(rule.key)) {
                result.errors.push_back("Required key \"" + rule.key + "\" is missing");
                result.violations[rule.key] = "Missing required key";
                result.valid = false;
                continue;
            }
            
            if (!config.has(rule.key)) {
                continue; // Skip optional keys
            }
            
            // Validate type and value
            bool typeValid = false;
            bool rangeValid = true;
            
            try {
                if (rule.type == "int") {
                    int value = config.getOr<int>(rule.key, std::numeric_limits<int>::min());
                    typeValid = true;
                    
                    if (std::holds_alternative<int>(rule.min)) {
                        rangeValid = value >= std::get<int>(rule.min);
                    }
                    if (rangeValid && std::holds_alternative<int>(rule.max)) {
                        rangeValid = value <= std::get<int>(rule.max);
                    }
                } else if (rule.type == "float") {
                    float value = config.getOr<float>(rule.key, std::numeric_limits<float>::lowest());
                    typeValid = true;
                    
                    if (std::holds_alternative<double>(rule.min)) {
                        rangeValid = value >= static_cast<float>(std::get<double>(rule.min));
                    }
                    if (rangeValid && std::holds_alternative<double>(rule.max)) {
                        rangeValid = value <= static_cast<float>(std::get<double>(rule.max));
                    }
                } else if (rule.type == "string") {
                    std::string value = config.getOr<std::string>(rule.key, "");
                    typeValid = true;
                    
                    if (std::holds_alternative<int>(rule.min)) {
                        rangeValid = value.length() >= static_cast<size_t>(std::get<int>(rule.min));
                    }
                    if (rangeValid && std::holds_alternative<int>(rule.max)) {
                        rangeValid = value.length() <= static_cast<size_t>(std::get<int>(rule.max));
                    }
                } else if (rule.type == "bool") {
                    bool value = config.getOr<bool>(rule.key, false);
                    typeValid = true; // No range validation for bool
                }
                
                if (!typeValid) {
                    result.errors.push_back("Key \"" + rule.key + "\" has invalid type (expected " + rule.type + ")");
                    result.violations[rule.key] = "Invalid type for " + rule.key;
                    result.valid = false;
                } else if (!rangeValid) {
                    result.warnings.push_back("Key \"" + rule.key + "\" value is out of expected range");
                    result.violations[rule.key] = "Out of range for " + rule.key;
                }
                
            } catch (const std::bad_variant_access&) {
                result.errors.push_back("Key \"" + rule.key + "\" has invalid value for type " + rule.type);
                result.violations[rule.key] = "Invalid value for " + rule.key;
                result.valid = false;
            }
        }
        
        return result;
    }
    
    // Generate validation report
    std::string generateReport(const ValidationResult& result) const {
        std::stringstream ss;
        ss << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        ss << "║                         CONFIGURATION VALIDATION REPORT                            ║" << std::endl;
        ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        if (result.valid) {
            ss << "║ Status        : VALID                                             ║" << std::endl;
        } else {
            ss << "║ Status        : INVALID                                          ║" << std::endl;
        }
        
        if (!result.errors.empty()) {
            ss << "║ Errors        : " << result.errors.size() << " ║" << std::endl;
            ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
            for (const auto& error : result.errors) {
                ss << "║ " << error << "                                       ║" << std::endl;
            }
        } else {
            ss << "║ Errors        : None                                            ║" << std::endl;
        }
        
        if (!result.warnings.empty()) {
            ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
            ss << "║ Warnings      : " << result.warnings.size() << " ║" << std::endl;
            ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
            for (const auto& warning : result.warnings) {
                ss << "║ " << warning << "                                       ║" << std::endl;
            }
        } else {
            ss << "║ Warnings      : None                                            ║" << std::endl;
        }
        
        if (!result.violations.empty()) {
            ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
            ss << "║ Violations    : " << result.violations.size() << " ║" << std::endl;
            ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
            for (const auto& [key, violation] : result.violations) {
                ss << "║ " << key << ": " << violation << "                   ║" << std::endl;
            }
        } else {
            ss << "║ Violations    : None                                            ║" << std::endl;
        }
        
        ss << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        return ss.str();
    }
};

// =============================================================================
// PROGRESS TRACKING AND DISPLAY UTILITIES
// =============================================================================

class ProgressTracker {
public:
    struct ProgressInfo {
        double completionPercentage;  // 0-100
        std::string currentPhase;     // Current phase description
        std::string estimatedTimeRemaining; // Human readable time remaining
        std::map<std::string, double> metrics;  // Custom metrics
        size_t stepsCompleted;       // Number of completed steps
        size_t totalSteps;           // Total number of steps
        double processingSpeed;      // Steps per second
        std::string statusMessage;   // Current status message
        std::chrono::steady_clock::time_point startTime;  // Session start time
    };
    
private:
    ProgressInfo progress_;
    mutable std::mutex progressMutex_;
    bool enabled_ = false;
    std::queue<std::string> messageQueue_;
    
public:
    ProgressTracker() {
        progress_.completionPercentage = 0.0;
        progress_.stepsCompleted = 0;
        progress_.totalSteps = 0;
        progress_.processingSpeed = 0.0;
        progress_.startTime = std::chrono::steady_clock::now();
    }
    
    // Enable progress tracking
    void enable(bool enable = true) {
        enabled_ = enable;
        if (enable) {
            progress_.startTime = std::chrono::steady_clock::now();
        }
    }
    
    // Update progress
    void update(double percentage, const std::string& phase = "", 
                size_t stepsCompleted = 0, size_t totalSteps = 0) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(progressMutex_);
        
        progress_.completionPercentage = percentage;
        if (!phase.empty()) {
            progress_.currentPhase = phase;
        }
        
        if (stepsCompleted > 0) {
            progress_.stepsCompleted = stepsCompleted;
        }
        
        if (totalSteps > 0) {
            progress_.totalSteps = totalSteps;
            
            // Calculate processing speed
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - progress_.startTime);
            if (elapsed.count() > 0) {
                progress_.processingSpeed = progress_.stepsCompleted / elapsed.count();
            }
            
            // Calculate estimated time remaining
            if (progress_.processingSpeed > 0) {
                double remainingSteps = progress_.totalSteps - progress_.stepsCompleted;
                double remainingSeconds = remainingSteps / progress_.processingSpeed;
                
                int hours = static_cast<int>(remainingSeconds / 3600);
                int minutes = static_cast<int>((remainingSeconds - hours * 3600) / 60);
                int seconds = static_cast<int>(remainingSeconds - hours * 3600 - minutes * 60);
                
                std::stringstream ss;
                if (hours > 0) ss << hours << "h ";
                if (minutes > 0 || hours > 0) ss << minutes << "m ";
                ss << seconds << "s";
                progress_.estimatedTimeRemaining = ss.str();
            }
        }
        
        // Add to message queue
        if (progress_.statusMessage.empty()) {
            messageQueue_.push("Progress updated: " + std::to_string(percentage) + "%");
        }
    }
    
    // Add status message
    void addMessage(const std::string& message) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(progressMutex_);
        progress_.statusMessage = message;
        messageQueue_.push(message);
    }
    
    // Get progress info
    ProgressInfo getProgress() const {
        std::lock_guard<std::mutex> lock(progressMutex_);
        return progress_;
    }
    
    // Generate progress report
    std::string generateReport() const {
        if (!enabled_) {
            return "Progress tracking is disabled.";
        }
        
        std::stringstream ss;
        ss << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        ss << "║                          PROGRESS TRACKING REPORT                                ║" << std::endl;
        ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        auto progress = getProgress();
        
        ss << "║ Completion    : " << std::left << std::setw(50) 
           << progress.completionPercentage << "% ║" << std::endl;
        ss << "║ Phase         : " << std::left << std::setw(50) 
           << progress.currentPhase << " ║" << std::endl;
        ss << "║ Steps         : " << std::left << std::setw(50) 
           << progress.stepsCompleted << "/" << progress.totalSteps << " ║" << std::endl;
        ss << "║ Speed         : " << std::left << std::setw(50) 
           << progress.processingSpeed << " steps/sec ║" << std::endl;
        ss << "║ Time Remaining: " << std::left << std::setw(50) 
           << progress.estimatedTimeRemaining << " ║" << std::endl;
        ss << "║ Status        : " << std::left << std::setw(50) 
           << progress.statusMessage << " ║" << std::endl;
        
        ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
        ss << "║ Custom Metrics                                              ║" << std::endl;
        ss << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        for (const auto& [name, value] : progress.metrics) {
            ss << "║ " << std::left << std::setw(27) << name 
               << ": " << std::left << std::setw(45) << value << "║" << std::endl;
        }
        
        if (progress.metrics.empty()) {
            ss << "║ No custom metrics collected                                   ║" << std::endl;
        }
        
        ss << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        return ss.str();
    }
    
    // Get next message from queue
    std::string getNextMessage() {
        std::lock_guard<std::mutex> lock(progressMutex_);
        if (messageQueue_.empty()) {
            return "";
        }
        
        std::string message = messageQueue_.front();
        messageQueue_.pop();
        return message;
    }
};

// =============================================================================
// CONFIGURATION FILE UTILITIES
// =============================================================================

class ConfigFileUtils {
public:
    // Get default configuration file paths
    static std::vector<std::string> getDefaultConfigPaths() {
        return {
            "./config.json",
            "./config.yaml",
            "./configs/default.cfg",
            "~/.nlm/config.json",
            "~/.nlm/config.yaml",
            "/etc/nlm/config.cfg"
        };
    }
    
    // Find first available configuration file
    static std::string findConfigFile(const std::vector<std::string>& searchPaths = {}) {
        std::vector<std::string> paths;
        
        if (searchPaths.empty()) {
            paths = getDefaultConfigPaths();
        } else {
            paths = searchPaths;
        }
        
        for (const auto& path : paths) {
            // Expand ~ to home directory
            std::string expandedPath = path;
            if (expandedPath.substr(0, 2) == "~/") {
                const char* home = std::getenv("HOME");
                if (home != nullptr) {
                    expandedPath = std::string(home) + expandedPath.substr(1);
                }
            }
            
            std::ifstream file(expandedPath);
            if (file.good()) {
                file.close();
                return expandedPath;
            }
        }
        
        return "";
    }
    
    // Create example configuration file
    static bool createExampleConfig(const std::string& filepath = "config.json") {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write example configuration
            file << "{\n  \\\"neuron_count\\\": 1000,\n  \\\"region_count\\\": 1,\n  \\\"connection_probability\\\": 0.1,\n  \\\"simulation_timestep\\\": 0.001,\n  \\\"random_seed\\\": 42,\n  \\\"stdp_ltp_weight\\\": 0.02,\n  \\\"stdp_ltd_weight\\\": 0.015,\n  \\\"synaptogenesis_rate\\\": 0.0001,\n  \\\"pruning_rate\\\": 0.00001,\n  \\\"replay_interval\\\": 100,\n  \\\"consolidation_interval\\\": 1000,\n  \\\"checkpoint_dir\\\": \\\"./checkpoints\\\",\n  \\\"checkpoint_interval\\\": 1000,\n  \\\"max_memory_mb\\\": 2048,\n  \\\"log_to_file\\\": false,\n  \\\"enable_validation\\\": true,\n  \\\"development_enabled\\\": true\n}\n";
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating example config: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Create example YAML configuration file
    static bool createExampleYamlConfig(const std::string& filepath = "config.yaml") {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write example YAML configuration
            file << "neuron_count: 1000\n";
            file << "region_count: 1\n";
            file << "connection_probability: 0.1\n";
            file << "simulation_timestep: 0.001\n";
            file << "random_seed: 42\n";
            file << "stdp_ltp_weight: 0.02\n";
            file << "stdp_ltd_weight: 0.015\n";
            file << "synaptogenesis_rate: 0.0001\n";
            file << "pruning_rate: 0.00001\n";
            file << "replay_interval: 100\n";
            file << "consolidation_interval: 1000\n";
            file << "checkpoint_dir: ./checkpoints\n";
            file << "checkpoint_interval: 1000\n";
            file << "max_memory_mb: 2048\n";
            file << "log_to_file: false\n";
            file << "enable_validation: true\n";
            file << "development_enabled: true\n";
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating example YAML config: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Create example INI configuration file
    static bool createExampleIniConfig(const std::string& filepath = "configs/default.cfg") {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write example INI configuration
            file << "# NLM Configuration File\n";
            file << "# Format: key = value\n\n";
            file << "neuron_count = 1000\n";
            file << "region_count = 1\n";
            file << "connection_probability = 0.1\n";
            file << "simulation_timestep = 0.001\n";
            file << "random_seed = 42\n";
            file << "stdp_ltp_weight = 0.02\n";
            file << "stdp_ltd_weight = 0.015\n";
            file << "synaptogenesis_rate = 0.0001\n";
            file << "pruning_rate = 0.00001\n";
            file << "replay_interval = 100\n";
            file << "consolidation_interval = 1000\n";
            file << "checkpoint_dir = ./checkpoints\n";
            file << "checkpoint_interval = 1000\n";
            file << "max_memory_mb = 2048\n";
            file << "log_to_file = false\n";
            file << "enable_validation = true\n";
            file << "development_enabled = true\n";
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating example INI config: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Validate configuration file exists
    static bool validateConfigFile(const std::string& filepath) {
        std::ifstream file(filepath);
        return file.good();
    }
    
    // Backup configuration file
    static bool backupConfigFile(const std::string& filepath, int backupNumber = 1) {
        std::string backupPath = filepath + ".backup";
        if (backupNumber > 1) {
            backupPath = filepath + ".backup." + std::to_string(backupNumber);
        }
        
        std::ifstream src(filepath, std::ios::binary);
        std::ofstream dst(backupPath, std::ios::binary);
        
        if (!src || !dst) {
            return false;
        }
        
        dst << src.rdbuf();
        return true;
    }
};

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

class UtilityFunctions {
public:
    // Format elapsed time as human-readable string
    static std::string formatElapsedTime(double seconds) {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
        
        std::stringstream ss;
        if (hours > 0) {
            ss << hours << "h ";
        }
        if (minutes > 0 || hours > 0) {
            ss << minutes << "m ";
        }
        ss << secs << "s";
        
        return ss.str();
    }
    
    // Format number with thousands separators
    static std::string formatNumber(size_t number) {
        std::stringstream ss;
        ss << number;
        std::string str = ss.str();
        
        size_t pos = str.length();
        while (pos > 3) {
            pos -= 3;
            str.insert(pos, ",");
        }
        
        return str;
    }
    
    // Get current timestamp as string
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    // Create directory if it doesn't exist
    static bool ensureDirectoryExists(const std::string& path) {
        try {
            std::filesystem::create_directories(path);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error creating directory " << path << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Clean up temporary files
    static void cleanupTemporaryFiles(const std::string& pattern = "tmp_*") {
        try {
            std::filesystem::path searchPath(pattern);
            for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                if (entry.is_regular_file()) {
                    std::filesystem::remove(entry.path());
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error cleaning up temporary files: " << e.what() << std::endl;
        }
    }
    
    // Generate random string
    static std::string generateRandomString(size_t length) {
        const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, characters.size() - 1);
        
        std::string result;
        for (size_t i = 0; i < length; ++i) {
            result += characters[distribution(generator)];
        }
        
        return result;
    }
    
    // Calculate hash of string
    static std::string calculateHash(const std::string& input) {
        std::hash<std::string> hasher;
        size_t hash = hasher(input);
        
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << hash;
        
        return ss.str();
    }
    
    // Truncate string to maximum length
    static std::string truncateString(const std::string& input, size_t maxLength) {
        if (input.length() <= maxLength) {
            return input;
        }
        
        return input.substr(0, maxLength - 3) + "...";
    }
    
    // Convert string to lowercase
    static std::string toLowercase(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Convert string to uppercase
    static std::string toUppercase(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
};