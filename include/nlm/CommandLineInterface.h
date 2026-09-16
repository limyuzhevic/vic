// NLM Command Line Interface
// This file provides a flexible command-line interface for NLM Phase 2
// Supports configurable test execution and parameter customization

#pragma once

#include "main.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cstdlib>

namespace nlm {

// Command line interface for NLM
class CommandLineInterface {
public:
    CommandLineInterface(int argc, char** argv) : argc_(argc), argv_(argv) {
        parseArguments();
    }
    
    ~CommandLineInterface() = default;
    
    // Check if a specific argument is present
    bool hasArgument(const std::string& arg) const {
        return std::find(arguments_.begin(), arguments_.end(), arg) != arguments_.end();
    }
    
    // Get the value of a specific argument
    std::string getArgument(const std::string& arg) const {
        auto it = argumentValues_.find(arg);
        if (it != argumentValues_.end()) {
            return it->second;
        }
        return "";
    }
    
    // Get boolean flag value
    bool getFlag(const std::string& flag) const {
        return flags_.find(flag) != flags_.end();
    }
    
    // Execute the appropriate action based on parsed arguments
    int execute() {
        if (showHelp_) {
            printHelp();
            return 0;
        }
        
        if (showVersion_) {
            printVersion();
            return 0;
        }
        
        if (saveConfig_) {
            if (saveConfigPath_.empty()) {
                std::cerr << "Error: --save-config requires a file path" << std::endl;
                return 1;
            }
            // Save configuration to file
            // Implementation depends on config system
            return 0;
        }
        
        if (!configFile_.empty()) {
            // Load configuration from file
            if (!config_->loadFromFile(configFile_)) {
                std::cerr << "Error: Failed to load configuration from: " << configFile_ << std::endl;
                return 1;
            }
        }
        
        // Parse test selection
        std::vector<std::string> selectedTests;
        if (!selectedTests_.empty()) {
            selectedTests = parseTestSelection(selectedTests_);
        }
        
        // Initialize brain with configuration
        auto config = std::make_shared<Config>();
        config->loadFromArgs(argc, argv);
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Run selected tests
        if (selectedTests.empty()) {
            // Run all tests
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== RUNNING ALL TESTS ===");
            runAllTests(brain);
        } else {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== RUNNING SELECTED TESTS ===");
            for (const auto& testName : selectedTests) {
                NLM_LOG_INFO("Running test: " << testName << "...");
                runTestByName(brain, testName);
            }
        }
        
        // Final status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Execution Complete ===");
        
        return 0;
    }
    
private:
    int argc_;
    char** argv_;
    std::vector<std::string> arguments_;
    std::map<std::string, std::string> argumentValues_;
    std::map<std::string, bool> flags_;
    std::shared_ptr<Config> config_;
    
    // Command-line parsing
    void parseArguments() {
        for (int i = 1; i < argc_; ++i) {
            std::string arg(argv_[i]);
            
            if (arg == "--help" || arg == "-h") {
                showHelp_ = true;
                continue;
            }
            
            if (arg == "--version") {
                showVersion_ = true;
                continue;
            }
            
            if (arg == "--save-config") {
                saveConfig_ = true;
                if (i + 1 < argc_) {
                    saveConfigPath_ = argv_[++i];
                }
                continue;
            }
            
            if (arg.substr(0, 8) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile_ = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc_) {
                    configFile_ = argv_[++i];
                }
                continue;
            }
            
            if (arg.substr(0, 10) == "--test=") {
                std::string testName = arg.substr(10);
                selectedTests_.push_back(testName);
                continue;
            }
            
            if (arg.substr(0, 6) == "--log=") {
                logLevel_ = arg.substr(6);
                continue;
            }
            
            if (arg.substr(0, 12) == "--neuron_count") {
                if (arg.find('=') != std::string::npos) {
                    neuronCount_ = std::stoi(arg.substr(arg.find('=') + 1));
                }
                continue;
            }
            
            if (arg.substr(0, 12) == "--region_count") {
                if (arg.find('=') != std::string::npos) {
                    regionCount_ = std::stoi(arg.substr(arg.find('=') + 1));
                }
                continue;
            }
            
            if (arg.substr(0, 20) == "--connection_probability") {
                if (arg.find('=') != std::string::npos) {
                    connectionProbability_ = std::stof(arg.substr(arg.find('=') + 1));
                }
                continue;
            }
            
            if (arg.substr(0, 17) == "--simulation_timestep") {
                if (arg.find('=') != std::string::npos) {
                    simulationTimestep_ = std::stod(arg.substr(arg.find('=') + 1));
                }
                continue;
            }
            
            // Handle boolean flags
            if (arg == "--reward_modulation_enabled" || arg == "--enable_reward_modulation") {
                flags_["reward_modulation_enabled"] = true;
                continue;
            }
            
            if (arg == "--structural_plasticity_enabled" || arg == "--enable_structural_plasticity") {
                flags_["structural_plasticity_enabled"] = true;
                continue;
            }
            
            if (arg == "--development_enabled" || arg == "--enable_development") {
                flags_["development_enabled"] = true;
                continue;
            }
            
            if (arg == "--curiosity_enabled" || arg == "--enable_curiosity") {
                flags_["curiosity_enabled"] = true;
                continue;
            }
            
            // Store other arguments
            arguments_.push_back(arg);
        }
    }
    
    // Parse test selection string
    std::vector<std::string> parseTestSelection(const std::string& testSelection) const {
        std::vector<std::string> tests;
        std::string test;
        std::istringstream iss(testSelection);
        
        while (std::getline(iss, test, ',')) {
            // Trim whitespace
            test.erase(0, test.find_first_not_of(" \t"));
            test.erase(test.find_last_not_of(" \t") + 1);
            
            if (!test.empty()) {
                tests.push_back(test);
            }
        }
        
        return tests;
    }
    
    // Print help information
    void printHelp() const {
        std::cout << "NLM Phase 2 Test Runner" << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./nlm [OPTIONS] [ARGUMENTS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message and exit" << std::endl;
        std::cout << "  --version                     Show version information and exit" << std::endl;
        std::cout << "  --config=FILE                Load configuration from FILE" << std::endl;
        std::cout << "  --save-config[=PATH]          Save configuration to PATH (default: config_backup.cfg)" << std::endl;
        std::cout << "  --test=TEST_NAME              Run specific test (can be repeated)" << std::endl;
        std::cout << "  --log=LEVEL                  Set log level (Debug, Info, Warning, Error, Critical)" << std::endl;
        std::cout << "  --neuron_count=N              Set number of neurons" << std::endl;
        std::cout << "  --region_count=N              Set number of brain regions" << std::endl;
        std::cout << "  --connection_probability=VAL   Set connection probability (0.0 - 1.0)" << std::endl;
        std::cout << "  --simulation_timestep=VAL      Set simulation timestep (seconds)" << std::endl;
        std::cout << "  --enable_reward_modulation     Enable reward modulation" << std::endl;
        std::cout << "  --enable_structural_plasticity Enable structural plasticity" << std::endl;
        std::cout << "  --enable_development           Enable developmental processes" << std::endl;
        std::cout << "  --enable_curiosity            Enable curiosity-driven exploration" << std::endl;
        std::cout << std::endl;
        std::cout << "Test Names:" << std::endl;
        auto allTests = getAllTests();
        for (const auto& test : allTests) {
            std::cout << "  " << test.name << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm --test=Basic%20Connectivity" << std::endl;
        std::cout << "  ./nlm --test=Plasticity%20Learning --test=STDP%20Verification" << std::endl;
        std::cout << "  ./nlm --test=Advanced%20Plasticity --test=Memory%20Integration" << std::endl;
        std::cout << "  ./nlm --run-all --config=/path/to/config.cfg" << std::endl;
        std::cout << "  ./nlm --neuron_count=200 --region_count=2 --log=Debug" << std::endl;
        std::cout << std::endl;
        std::cout << "Shortcut Options:" << std::endl;
        std::cout << "  -h                           Same as --help" << std::endl;
        std::cout << "  --run-all                    Run all available tests" << std::endl;
        std::cout << "  --debug                      Set log level to Debug" << std::endl;
    }
    
    // Print version information
    void printVersion() const {
        std::cout << "NLM Phase 2 Test Runner" << std::endl;
        std::cout << "Version: 2.0.0" << std::endl;
        std::cout << "Build: " << getCurrentTimestamp() << std::endl;
        std::cout << "Author: Neural Learning Machine Team" << std::endl;
        std::cout << "License: MIT" << std::endl;
        std::cout << std::endl;
        std::cout << "This is the official Phase 2 release implementing real neural computation:" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
        std::cout << "  - Event-driven spike propagation with delays" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "  - Learning experiments with measurable changes" << std::endl;
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Helper function to run all tests
    void runAllTests(std::shared_ptr<Brain> brain) {
        for (const auto& test : getAllTests()) {
            runTestByName(brain, test.name);
        }
    }
    
    // Helper function to run a single test by name
    void runTestByName(std::shared_ptr<Brain> brain, const std::string& testName) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Test: " << testName << " ===");
        
        for (const auto& test : getAllTests()) {
            if (test.name == testName) {
                test.testFunc(brain);
                break;
            }
        }
        
        NLM_LOG_INFO("=== Test Completed ===");
    }
    
public:
    // Public members for accessing parsed values
    std::string configFile_;
    std::vector<std::string> selectedTests_;
    std::string logLevel_;
    bool saveConfig_ = false;
    std::string saveConfigPath_ = "config_backup.cfg";
    bool showHelp_ = false;
    bool showVersion_ = false;
    
    // Configuration parameters
    int neuronCount_ = 500;
    int regionCount_ = 1;
    float connectionProbability_ = 0.15f;
    double simulationTimestep_ = 0.001;
    
    // Feature flags
    bool enableRewardModulation_ = true;
    bool enableStructuralPlasticity_ = true;
    bool enableDevelopment_ = true;
    bool enableCuriosity_ = true;
};

// Utility functions for NLM command line interface
namespace CLIUtils {

// Parse command line arguments and create CLI instance
inline std::unique_ptr<CommandLineInterface> parseCommandLine(int argc, char** argv) {
    return std::make_unique<CommandLineInterface>(argc, argv);
}

// Execute command line interface
inline int executeCommandLine(int argc, char** argv) {
    auto cli = parseCommandLine(argc, argv);
    return cli->execute();
}

// Check if running in quiet mode
inline bool isQuietMode(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quiet" || arg == "-q") {
            return true;
        }
    }
    return false;
}

// Check if running in verbose mode
inline bool isVerboseMode(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            return true;
        }
    }
    return false;
}

// Get test name from command line arguments
inline std::string getTestNameFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--test=") {
            return arg.substr(7);
        }
    }
    return "";
}

// Expand test name wildcards
inline std::vector<std::string> expandTestWildcards(const std::string& pattern) {
    std::vector<std::string> tests;
    auto allTests = getAllTests();
    
    for (const auto& test : allTests) {
        // Simple wildcard matching
        bool matches = true;
        if (pattern == "*") {
            matches = true;
        } else if (pattern == "basic") {
            matches = (test.name.find("Basic") != std::string::npos);
        } else if (pattern == "plasticity") {
            matches = (test.name.find("Plasticity") != std::string::npos);
        } else if (pattern == "stdp") {
            matches = (test.name.find("STDP") != std::string::npos);
        } else if (pattern == "memory") {
            matches = (test.name.find("Memory") != std::string::npos);
        } else if (pattern == "development") {
            matches = (test.name.find("Development") != std::string::npos);
        } else {
            matches = (test.name == pattern);
        }
        
        if (matches) {
            tests.push_back(test.name);
        }
    }
    
    return tests;
}

// Generate report from command line execution
inline std::string generateExecutionReport(int exitCode, const std::vector<std::string>& selectedTests) {
    std::stringstream ss;
    ss << "NLM Phase 2 Execution Report" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Exit Code: " << exitCode << std::endl;
    ss << "Selected Tests: " << selectedTests.size() << std::endl;
    for (const auto& test : selectedTests) {
        ss << "  - " << test << std::endl;
    }
    ss << "Timestamp: " << getCurrentTimestamp() << std::endl;
    ss << "Status: " << (exitCode == 0 ? "SUCCESS" : "FAILED") << std::endl;
    
    return ss.str();
}

// Check if argument is a flag
inline bool isFlag(const std::string& arg) {
    return arg == "--help" || arg == "-h" || arg == "--version" ||
           arg == "--run-all" || arg == "--debug" || arg == "--verbose" ||
           arg == "--quiet" || arg == "-v" || arg == "-q";
}

// Parse boolean arguments
inline bool parseBoolArgument(const std::string& arg, bool defaultValue = false) {
    if (arg == "--enable" || arg == "--on" || arg == "--true") {
        return true;
    } else if (arg == "--disable" || arg == "--off" || arg == "--false") {
        return false;
    }
    return defaultValue;
}

// Get current timestamp
inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Validate configuration file
inline bool validateConfigFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file has content
    std::string line;
    bool hasContent = false;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '#' && line[0] != '/') {
            hasContent = true;
            break;
        }
    }
    
    file.close();
    return hasContent;
}

// Create backup of configuration file
inline bool backupConfigFile(const std::string& filepath) {
    std::string backupPath = filepath + ".backup." + getCurrentTimestamp();
    backupPath.erase(std::remove(backupPath.begin(), backupPath.end(), ':'), backupPath.end());
    backupPath.erase(std::remove(backupPath.begin(), backupPath.end(), ' '), backupPath.end());
    
    std::ifstream src(filepath, std::ios::binary);
    std::ofstream dst(backupPath, std::ios::binary);
    
    if (!src || !dst) {
        return false;
    }
    
    dst << src.rdbuf();
    
    return true;
}

// List available tests
inline void listAvailableTests() {
    std::cout << "Available Tests:" << std::endl;
    std::cout << "===============" << std::endl;
    auto allTests = getAllTests();
    
    for (size_t i = 0; i < allTests.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << allTests[i].name << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Test Groups:" << std::endl;
    std::cout << "  basic      - Basic connectivity and spike propagation" << std::endl;
    std::cout << "  plasticity - Plasticity learning experiments" << std::endl;
    std::cout << "  stdp       - STDP timing rules verification" << std::endl;
    std::cout << "  memory     - Memory system integration" << std::endl;
    std::cout << "  development - Development and maturation processes" << std::endl;
}

// Execute specific test command
inline int executeTestCommand(const std::string& testName) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    runTestByName(brain, testName);
    return 0;
}

// Execute multiple test commands
inline int executeMultipleTests(const std::vector<std::string>& testNames) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    for (const auto& testName : testNames) {
        NLM_LOG_INFO("Running test: " << testName);
        runTestByName(brain, testName);
        
        // Reset brain between tests
        brain->reset();
        brain->initialize();
    }
    
    return 0;
}

} // namespace CLIUtils

} // namespace nlm

// Main function for command line execution
int main(int argc, char** argv) {
    // Initialize NLM
    NLM_LOG_INFO("Starting NLM Phase 2...")
    
    // Parse command line arguments
    auto cli = std::make_unique<nlm::CommandLineInterface>(argc, argv);
    
    // Execute the interface
    int exitCode = cli->execute();
    
    // Shutdown NLM
    NLM_LOG_INFO("NLM Phase 2 execution completed with exit code: " << exitCode);
    
    return exitCode;
}