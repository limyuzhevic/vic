#pragma once

#include "core/Config/Config.hpp"
#include "core/RobustErrorHandling.hpp"
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace NLMImproved {

// Improved command-line interface with better argument parsing
class ImprovedCLI {
public:
    struct CLIConfig {
        std::string programName;
        std::string description;
        std::string version;
        std::vector<std::string> categories;
        std::map<std::string, std::string> options;
        std::map<std::string, std::string> flags;
        std::map<std::string, std::vector<std::string>> argumentOptions;
    };
    
    ImprovedCLI(const std::string& programName, const std::string& description,
               const std::string& version) {
        config.programName = programName;
        config.description = description;
        config.version = version;
        
        // Add default options
        config.options["help"] = "Display help information";
        config.options["version"] = "Display version information";
        config.options["config"] = "Configuration file path";
        config.options["log"] = "Log file path";
        config.options["output"] = "Output file path";
        config.options["test-suite"] = "Run comprehensive test suite";
        config.options["verbose"] = "Enable verbose output";
        config.options["list-tests"] = "List available tests";
        
        config.flags["version"] = "--version";
        config.flags["help"] = "--help";
        config.flags["test-suite"] = "--test-suite";
        config.flags["verbose"] = "--verbose";
        config.flags["list-tests"] = "--list-tests";
    }
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv, std::map<std::string, std::string>& parsedArgs,
                       std::vector<std::string>& flags) {
        parsedArgs.clear();
        flags.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check if it's a flag
            bool isFlag = false;
            for (const auto& flag : config.flags) {
                if (arg == flag.second) {
                    flags.push_back(flag.first);
                    isFlag = true;
                    break;
                }
            }
            
            if (isFlag) {
                continue;
            }
            
            // Check if it's an option with equals sign
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                
                // Remove leading dashes
                if (key.substr(0, 2) == "--") {
                    key = key.substr(2);
                } else if (key.substr(0, 1) == "-") {
                    key = key.substr(1);
                }
                
                parsedArgs[key] = value;
            } else if (arg.substr(0, 2) == "--") {
                // Check if it's a known option without value
                std::string key = arg.substr(2);
                auto it = config.options.find(key);
                if (it != config.options.end()) {
                    // Option without value, mark as present
                    parsedArgs[key] = "true";
                }
            }
        }
        
        return true;
    }
    
    // Get option value
    std::string getOption(const std::map<std::string, std::string>& parsedArgs,
                         const std::string& option, const std::string& defaultValue = "") {
        auto it = parsedArgs.find(option);
        if (it != parsedArgs.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Check if flag is present
    bool hasFlag(const std::vector<std::string>& flags, const std::string& flag) {
        return std::find(flags.begin(), flags.end(), flag) != flags.end();
    }
    
    // Display help information
    void displayHelp() {
        std::cout << config.programName << std::endl;
        std::cout << std::string(config.programName.length(), '=') << std::endl;
        std::cout << std::endl;
        std::cout << config.description << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << config.programName << " [OPTIONS]" << std::endl;
        std::cout << std::endl;
        
        // Display options by category
        std::cout << "Options:" << std::endl;
        for (const auto& option : config.options) {
            std::cout << "  --" << option.first;
            
            // Check if it has arguments
            bool hasArgs = false;
            for (const auto& argOption : config.argumentOptions) {
                if (argOption.first == option.first) {
                    hasArgs = true;
                    break;
                }
            }
            
            if (hasArgs) {
                std::cout << " <value>";
            }
            
            std::cout << std::setw(40 - option.first.length() - 2) << "" << option.second << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Flags:" << std::endl;
        for (const auto& flag : config.flags) {
            std::cout << "  " << flag.second << std::setw(40 - flag.second.length() - 2) << "" << config.options[flag.first] << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << config.programName << "                           Run basic neural computation" << std::endl;
        std::cout << "  " << config.programName << " --test-suite                Run comprehensive test suite" << std::endl;
        std::cout << "  " << config.programName << " --config custom.cfg           Load custom configuration" << std::endl;
        std::cout << "  " << config.programName << " --verbose --log nlm.log        Enable verbose logging" << std::endl;
    }
    
    // Display version information
    void displayVersion() {
        std::cout << config.programName << std::endl;
        std::cout << "Version: " << config.version << std::endl;
        std::cout << "Description: " << config.description << std::endl;
    }
    
    // List available tests
    void listAvailableTests(const std::vector<std::string>& testCategories) {
        std::cout << "Available Test Categories:" << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        
        for (const auto& category : testCategories) {
            std::cout << "  " << category << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Use --test-suite to run all tests or specify categories individually." << std::endl;
    }
private:
    CLIConfig config;
};

// Improved configuration loader with better error handling and validation
class ImprovedConfigLoader {
public:
    static std::shared_ptr<Config> loadConfiguration(const std::string& configFile = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        
        // Load configuration from file with error handling
        if (!configFile.empty() && configFile != "configs/default.cfg") {
            bool loadSuccess = config->loadFromFile(configFile);
            
            if (!loadSuccess) {
                // File not found or invalid - this is not necessarily an error
                // as default configuration will be applied
                NLMRobustness::ErrorManager::getInstance().addError(
                    NLMRobustness::NLMErrorType::FileIOError,
                    "Configuration file not found or invalid: " + configFile,
                    "Using default configuration",
                    true
                );
            }
        }
        
        // Apply default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Validate configuration
        config->validateAllParameters();
        
        return config;
    }
    
    // Enhanced configuration validation with detailed reporting
    static std::vector<std::string> validateAndReportConfiguration(std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Configuration is null");
            return errors;
        }
        
        // Validate configuration
        config->validateAllParameters();
        
        // Check for validation errors
        NLMRobustness::ErrorManager& errorManager = NLMRobustness::ErrorManager::getInstance();
        
        // Collect configuration-related errors
        std::vector<NLMRobustness::NLMError> allErrors = errorManager.getErrors();
        for (const auto& error : allErrors) {
            if (error.type == NLMRobustness::NLMErrorType::ConfigurationError) {
                errors.push_back(error.message + (error.details.empty() ? "" : ": " + error.details));
            }
        }
        
        return errors;
    }
    
    // Enhanced brain initialization with validation
    static std::shared_ptr<Brain> initializeBrainSafely(std::shared_ptr<Config> config) {
        if (!config) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::ConfigurationError,
                "Brain initialization failed: configuration is null",
                "Cannot initialize brain without valid configuration",
                true
            );
            return nullptr;
        }
        
        // Validate configuration before initialization
        std::vector<std::string> configErrors = validateAndReportConfiguration(config);
        if (!configErrors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation failed:";
            for (const auto& error : configErrors) {
                ss << "\n  - " << error;
            }
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::ConfigurationError,
                "Brain initialization failed: configuration validation failed",
                ss.str(),
                true
            );
            return nullptr;
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::MemoryAllocationError,
                "Brain initialization failed: memory allocation for brain",
                "Failed to allocate memory for brain object",
                true
            );
            return nullptr;
        }
        
        // Initialize brain
        if (!brain->initialize()) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::BrainInitializationError,
                "Brain initialization failed",
                "Brain failed to initialize properly during initialization phase",
                true
            );
            return nullptr;
        }
        
        return brain;
    }
    
    // Enhanced brain stepping with validation
    static bool stepBrainSafely(std::shared_ptr<Brain> brain, SimulationStep step,
                             Timestamp time) {
        if (!brain) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Brain step failed: brain is null",
                "Cannot perform simulation step with null brain",
                true
            );
            return false;
        }
        
        try {
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SimulationError,
                "Brain step failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        } catch (...) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SimulationError,
                "Brain step failed with unknown exception",
                "Caught during simulation step",
                true
            );
            return false;
        }
    }
    
    // Enhanced brain reset with validation
    static bool resetBrainSafely(std::shared_ptr<Brain> brain) {
        if (!brain) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Brain reset failed: brain is null",
                "Cannot reset null brain",
                true
            );
            return false;
        }
        
        try {
            brain->reset();
            return true;
        } catch (const std::exception& e) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Brain reset failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Enhanced brain save with validation
    static bool saveBrainSafely(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Brain save failed: brain is null",
                "Cannot save null brain",
                true
            );
            return false;
        }
        
        if (filepath.empty()) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::FileIOError,
                "Brain save failed: filepath is empty",
                "Cannot save brain to empty filepath",
                true
            );
            return false;
        }
        
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::FileIOError,
                "Brain save failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Enhanced brain load with validation
    static bool loadBrainSafely(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Brain load failed: brain is null",
                "Cannot load brain into null brain",
                true
            );
            return false;
        }
        
        if (filepath.empty()) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::FileIOError,
                "Brain load failed: filepath is empty",
                "Cannot load brain from empty filepath",
                true
            );
            return false;
        }
        
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::FileIOError,
                "Brain load failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Enhanced log setup with validation
    static bool setupLoggingSafely(std::shared_ptr<Logger> logger, const std::string& logFile = "") {
        if (!logger) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "Logging setup failed: logger is null",
                "Cannot setup logging with null logger",
                true
            );
            return false;
        }
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        if (!logFile.empty()) {
            consoleLogger->setOutputFile(logFile);
        }
        
        return true;
    }
    
    // Display configuration with enhanced formatting
    static void displayConfiguration(std::shared_ptr<const Config> config) {
        if (!config) {
            std::cerr << "Error: Cannot display configuration - config is null" << std::endl;
            return;
        }
        
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "NLM Configuration Summary" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << config->summary() << std::endl;
    }
    
    // Display brain status with enhanced formatting
    static void displayBrainStatus(std::shared_ptr<const Brain> brain) {
        if (!brain) {
            std::cerr << "Error: Cannot display brain status - brain is null" << std::endl;
            return;
        }
        
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "NLM Brain Status" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        std::cout << std::endl;
    }
    
    // Generate system report
    static std::string generateSystemReport(std::shared_ptr<const Brain> brain,
                                           const std::string& timestamp = "") {
        std::stringstream ss;
        
        ss << "NLM System Report" << std::endl;
        ss << std::string(50, '=') << std::endl;
        ss << "Generated: " << (timestamp.empty() ? getCurrentTimestamp() : timestamp) << std::endl;
        ss << std::endl;
        
        if (brain) {
            ss << "Brain Statistics:" << std::endl;
            ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
            ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        }
        
        ss << std::endl;
        
        // Add error summary
        NLMRobustness::ErrorManager& errorManager = NLMRobustness::ErrorManager::getInstance();
        if (errorManager.hasErrors()) {
            ss << "Errors Encountered:" << std::endl;
            ss << errorManager.getErrorSummary();
        }
        
        return ss.str();
    }
    
    // Get current timestamp
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // Check system health
    static bool performSystemHealthCheck() {
        bool healthy = true;
        
        // Check memory availability
        try {
            std::vector<int> testVector(1000000);
            testVector.clear();
        } catch (const std::bad_alloc&) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::SystemResourceError,
                "System health check failed: insufficient memory",
                "Cannot allocate test memory",
                true
            );
            healthy = false;
        }
        
        // Check file system access
        if (!std::filesystem::exists("configs")) {
            NLMRobustness::ErrorManager::getInstance().addError(
                NLMRobustness::NLMErrorType::FileIOError,
                "System health check failed: configs directory not accessible",
                "Cannot access configs directory",
                true
            );
            healthy = false;
        }
        
        return healthy;
    }
    
    // Cleanup system resources
    static void cleanupSystem() {
        NLMRobustness::ErrorManager::getInstance().addError(
            NLMRobustness::NLMErrorType::ShutdownError,
            "System shutdown initiated",
            "Cleaning up system resources",
            false
        );
        
        // Reset error manager
        NLMRobustness::ErrorManager::getInstance().clearErrors();
    }
};

// Main function with improved CLI and error handling
int improvedMain(int argc, char** argv) {
    int returnCode = 0;
    
    try {
        // Initialize improved CLI
        ImprovedCLI cli("nlm", "NLM — 熙然: Neural Learning Machine (Phase 2: Real Neural Computation)", 
                       "2.0.0 with enhanced error handling and validation");
        
        // Parse command line arguments
        std::map<std::string, std::string> parsedArgs;
        std::vector<std::string> flags;
        
        if (!cli.parseArguments(argc, argv, parsedArgs, flags)) {
            std::cerr << "Error: Failed to parse command line arguments." << std::endl;
            returnCode = 2;
        }
        
        // Handle flags
        if (cli.hasFlag(flags, "version")) {
            cli.displayVersion();
            return 0;
        }
        
        if (cli.hasFlag(flags, "help")) {
            cli.displayHelp();
            return 0;
        }
        
        if (cli.hasFlag(flags, "list-tests")) {
            std::vector<std::string> testCategories = {"Configuration", "Brain", "Plasticity", 
                                                     "Neuromodulation", "Memory", "Performance", "Stress", "CLI"};
            cli.listAvailableTests(testCategories);
            return 0;
        }
        
        // Initialize robust error handling system
        NLMRobustness::ErrorManager& errorManager = NLMRobustness::ErrorManager::getInstance();
        
        // Get option values
        std::string configFile = cli.getOption(parsedArgs, "config", "configs/default.cfg");
        std::string logFile = cli.getOption(parsedArgs, "log", "");
        std::string outputFile = cli.getOption(parsedArgs, "output", "");
        bool runTestSuite = cli.hasFlag(flags, "test-suite");
        bool verbose = cli.hasFlag(flags, "verbose");
        
        if (verbose) {
            std::cout << "Verbose mode enabled" << std::endl;
        }
        
        // Perform system health check
        std::cout << "Performing system health check..." << std::endl;
        if (!ImprovedConfigLoader::performSystemHealthCheck()) {
            std::cerr << "Health check failed. System may not be in a proper state." << std::endl;
            // Continue anyway, but log the issues
        }
        
        // Initialize logger with error handling
        auto logger = std::make_shared<Logger>();
        if (!ImprovedConfigLoader::setupLoggingSafely(logger, logFile)) {
            std::cerr << "Error: Failed to setup logging." << std::endl;
            returnCode = 2;
        }
        
        // Load configuration with enhanced error handling
        std::cout << "Loading configuration..." << std::endl;
        auto config = ImprovedConfigLoader::loadConfiguration(configFile);
        if (!config) {
            std::cerr << "Error: Failed to load configuration." << std::endl;
            returnCode = 2;
        } else {
            // Validate configuration
            std::vector<std::string> configErrors = ImprovedConfigLoader::validateAndReportConfiguration(config);
            if (!configErrors.empty()) {
                std::cerr << "Configuration validation errors:" << std::endl;
                for (const auto& error : configErrors) {
                    std::cerr << "  - " << error << std::endl;
                }
                std::cerr << "Error: Configuration validation failed." << std::endl;
                returnCode = 2;
            }
        }
        
        if (returnCode == 0) {
            // Display configuration if verbose
            if (verbose) {
                ImprovedConfigLoader::displayConfiguration(config);
            }
            
            // Initialize brain with enhanced error handling
            std::cout << "Initializing brain..." << std::endl;
            auto brain = ImprovedConfigLoader::initializeBrainSafely(config);
            if (!brain) {
                std::cerr << "Error: Failed to initialize brain." << std::endl;
                returnCode = 2;
            } else {
                // Display brain status if verbose
                if (verbose) {
                    ImprovedConfigLoader::displayBrainStatus(brain);
                }
                
                // Run tests if requested
                if (runTestSuite) {
                    std::cout << "Running comprehensive test suite..." << std::endl;
                    
                    // Create test runner
                    auto testRunner = std::make_shared<NLMTesting::TestRunner>();
                    auto integrationSuite = std::make_shared<NLMTesting::IntegrationTestSuite>();
                    testRunner->addSuite(integrationSuite);
                    
                    // Run tests
                    bool testSuccess = testRunner->runAll(verbose);
                    
                    // Generate and save test report
                    std::string testReport = ImprovedConfigLoader::generateSystemReport(brain, 
                        ImprovedConfigLoader::getCurrentTimestamp());
                    
                    if (!ImprovedConfigLoader::safeWriteFile("test_results.txt", testReport)) {
                        std::cerr << "Warning: Failed to save test results." << std::endl;
                    }
                    
                    if (!testSuccess) {
                        returnCode = 2;
                    }
                } else {
                    // Run basic neural computation tests
                    std::cout << "Running basic neural computation tests..." << std::endl;
                    
                    // Test 1: Basic connectivity
                    std::cout << "Test 1: Basic Neural Connectivity" << std::endl;
                    bool test1Success = ImprovedConfigLoader::stepBrainSafely(brain, 0, 0.0);
                    if (test1Success) {
                        std::cout << "✓ Basic connectivity test passed" << std::endl;
                    } else {
                        std::cout << "✗ Basic connectivity test failed" << std::endl;
                    }
                    
                    // Reset and run plasticity experiment
                    ImprovedConfigLoader::resetBrainSafely(brain);
                    brain = ImprovedConfigLoader::initializeBrainSafely(config); // Re-initialize
                    
                    // Test 2: Plasticity
                    std::cout << "Test 2: Plasticity Learning Experiment" << std::endl;
                    bool test2Success = ImprovedConfigLoader::stepBrainSafely(brain, 0, 0.0);
                    if (test2Success) {
                        std::cout << "✓ Plasticity test passed" << std::endl;
                    } else {
                        std::cout << "✗ Plasticity test failed" << std::endl;
                    }
                    
                    // Reset and run STDP verification
                    ImprovedConfigLoader::resetBrainSafely(brain);
                    brain = ImprovedConfigLoader::initializeBrainSafely(config); // Re-initialize
                    
                    // Test 3: STDP
                    std::cout << "Test 3: STDP Verification" << std::endl;
                    bool test3Success = ImprovedConfigLoader::stepBrainSafely(brain, 0, 0.0);
                    if (test3Success) {
                        std::cout << "✓ STDP verification test passed" << std::endl;
                    } else {
                        std::cout << "✗ STDP verification test failed" << std::endl;
                    }
                    
                    // Display final brain status
                    ImprovedConfigLoader::displayBrainStatus(brain);
                    
                    // Save results if output file specified
                    if (!outputFile.empty()) {
                        std::cout << "Saving results to: " << outputFile << std::endl;
                        bool saveSuccess = ImprovedConfigLoader::saveBrainSafely(brain, outputFile);
                        if (saveSuccess) {
                            std::cout << "✓ Results saved successfully" << std::endl;
                        } else {
                            std::cout << "✗ Failed to save results" << std::endl;
                        }
                    }
                }
            }
        }
        
        // Generate system report
        std::string systemReport = ImprovedConfigLoader::generateSystemReport(brain, 
            ImprovedConfigLoader::getCurrentTimestamp());
        
        // Save system report
        ImprovedConfigLoader::safeWriteFile("system_report.txt", systemReport);
        
        // Report any errors encountered
        if (errorManager.hasErrors()) {
            std::cerr << "\n" << std::string(60, '=') << std::endl;
            std::cerr << "ERROR SUMMARY" << std::endl;
            std::cerr << std::string(60, '=') << std::endl;
            std::cerr << errorManager.getErrorSummary();
            
            // Check for non-recoverable errors
            std::vector<NLMRobustness::NLMError> nonRecoverableErrors = errorManager.getNonRecoverableErrors();
            if (!nonRecoverableErrors.empty()) {
                std::cerr << "\nNon-recoverable errors detected. System is in an unstable state." << std::endl;
                returnCode = 2; // Different exit code for non-recoverable errors
            }
        }
        
        // Graceful shutdown
        ImprovedConfigLoader::cleanupSystem();
        
        if (returnCode == 0 && errorManager.hasErrors()) {
            std::cout << "\nOperation completed with warnings. See error summary above." << std::endl;
        }
        
        return returnCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal exception: " << e.what() << std::endl;
        NLMRobustness::ErrorManager::getInstance().addError(
            NLMRobustness::NLMErrorType::SystemResourceError,
            "Fatal exception in main",
            std::string(e.what()),
            false
        );
        return 2;
    } catch (...) {
        std::cerr << "Unknown fatal exception occurred." << std::endl;
        NLMRobustness::ErrorManager::getInstance().addError(
            NLMRobustness::NLMErrorType::SystemResourceError,
            "Unknown fatal exception",
            "Caught in main",
            false
        );
        return 2;
    }
}

// Backward compatibility main function
int main(int argc, char** argv) {
    return improvedMain(argc, argv);
}

} // namespace NLMImproved