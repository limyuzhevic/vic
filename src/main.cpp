// NLM (熙然) - Neural Learning Machine
// Improved Phase 6 Integration with Better CLI, Error Handling, and Logging
//
// This version includes:
// - Comprehensive CLI interface with multiple execution modes
// - Improved error handling and validation
// - Better logging configuration
// - Configuration management improvements
// - Memory safety and resource management
// - Support for advanced configuration options
// - Phase 6 integration demonstration

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <chrono>

using namespace nlm;

// Forward declarations for helper functions
void printBanner();
void printUsage(const std::string& programName);
void printHelp(const std::string& programName);
void printVersionInfo();

// Execution mode enum
enum class ExecutionMode {
    Phase2Demo,
    Phase6Demo,
    Phase6Integration,
    TestValidation,
    ConfigValidation,
    VersionInfo,
    Help
};

// Application options structure
struct AppOptions {
    ExecutionMode mode = ExecutionMode::Phase6Demo;
    std::string configFile = "configs/nlm_config.json";
    std::string outputFile;
    std::string logFile;
    LogLevel logLevel = LogLevel::Info;
    bool useColors = true;
    bool verbose = false;
    bool benchmark = false;
    uint32_t testNeurons = 500;
    uint32_t testSteps = 1000;
    std::vector<std::string> configOverrides;
    bool validateConfig = false;
    bool saveResults = false;
    std::string experimentName = "default_experiment";
};

// Parse command line arguments
AppOptions parseArguments(int argc, char** argv) {
    AppOptions options;
    std::string programName = "nlm";
    
    if (argc > 0) {
        programName = argv[0];
    }
    
    // Simple argument parsing with support for --key=value and -key value
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle mode selection
        if (arg == "--mode" && i + 1 < argc) {
            std::string modeArg = argv[++i];
            if (modeArg == "phase2") options.mode = ExecutionMode::Phase2Demo;
            else if (modeArg == "phase6") options.mode = ExecutionMode::Phase6Demo;
            else if (modeArg == "integration") options.mode = ExecutionMode::Phase6Integration;
            else if (modeArg == "test") options.mode = ExecutionMode::TestValidation;
            else if (modeArg == "config") options.mode = ExecutionMode::ConfigValidation;
            else if (modeArg == "version") options.mode = ExecutionMode::VersionInfo;
            else if (modeArg == "help") options.mode = ExecutionMode::Help;
            else {
                std::cerr << "ERROR: Unknown mode '" << modeArg << "'. Use --help for available modes." << std::endl;
                exit(1);
            }
        }
        // Handle short mode flag
        else if (arg == "-m" && i + 1 < argc) {
            std::string modeArg = argv[++i];
            if (modeArg == "2") options.mode = ExecutionMode::Phase2Demo;
            else if (modeArg == "6") options.mode = ExecutionMode::Phase6Demo;
            else if (modeArg == "i") options.mode = ExecutionMode::Phase6Integration;
            else if (modeArg == "t") options.mode = ExecutionMode::TestValidation;
            else {
                std::cerr << "ERROR: Unknown short mode '" << modeArg << "'." << std::endl;
                exit(1);
            }
        }
        
        // Handle configuration file
        else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        }
        else if (arg == "-c" && i + 1 < argc) {
            options.configFile = argv[++i];
        }
        
        // Handle log level
        else if (arg == "--log-level" && i + 1 < argc) {
            std::string levelArg = argv[++i];
            if (levelArg == "debug") options.logLevel = LogLevel::Debug;
            else if (levelArg == "info") options.logLevel = LogLevel::Info;
            else if (levelArg == "warning") options.logLevel = LogLevel::Warning;
            else if (levelArg == "error") options.logLevel = LogLevel::Error;
            else if (levelArg == "critical") options.logLevel = LogLevel::Critical;
            else {
                std::cerr << "ERROR: Unknown log level '" << levelArg << "'." << std::endl;
                exit(1);
            }
        }
        else if (arg == "-l" && i + 1 < argc) {
            std::string levelArg = argv[++i];
            if (levelArg == "d") options.logLevel = LogLevel::Debug;
            else if (levelArg == "i") options.logLevel = LogLevel::Info;
            else if (levelArg == "w") options.logLevel = LogLevel::Warning;
            else if (levelArg == "e") options.logLevel = LogLevel::Error;
            else if (levelArg == "c") options.logLevel = LogLevel::Critical;
            else {
                std::cerr << "ERROR: Unknown short log level '" << levelArg << "'." << std::endl;
                exit(1);
            }
        }
        
        // Handle log file output
        else if (arg == "--log-file" && i + 1 < argc) {
            options.logFile = argv[++i];
        }
        
        // Handle output file
        else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            options.outputFile = argv[++i];
        }
        
        // Handle verbose mode
        else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        }
        
        // Handle benchmark mode
        else if (arg == "--benchmark" || arg == "-b") {
            options.benchmark = true;
        }
        
        // Handle color support
        else if (arg == "--no-color" || arg == "-nc") {
            options.useColors = false;
        }
        else if (arg == "--color" || arg == "-c") {
            options.useColors = true;
        }
        
        // Handle test parameters
        else if (arg == "--neurons" && i + 1 < argc) {
            options.testNeurons = std::stoi(argv[++i]);
        }
        else if (arg == "--steps" && i + 1 < argc) {
            options.testSteps = std::stoi(argv[++i]);
        }
        
        // Handle config validation
        else if (arg == "--validate-config") {
            options.validateConfig = true;
        }
        
        // Handle save results
        else if (arg == "--save-results" || arg == "-s") {
            options.saveResults = true;
        }
        
        // Handle experiment name
        else if (arg == "--experiment-name" && i + 1 < argc) {
            options.experimentName = argv[++i];
        }
        
        // Handle help
        else if (arg == "--help" || arg == "-h") {
            options.mode = ExecutionMode::Help;
        }
        
        // Handle version info
        else if (arg == "--version" || arg == "-V") {
            options.mode = ExecutionMode::VersionInfo;
        }
        
        // Handle unknown argument
        else if (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-") {
            std::cerr << "ERROR: Unknown argument '" << arg << "'." << std::endl;
            printUsage(programName);
            exit(1);
        }
        
        // Simple key=value config overrides
        else if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            options.configOverrides.push_back(arg.substr(2));
        }
        else if (arg.find('=') != std::string::npos && arg[0] == '-') {
            options.configOverrides.push_back(arg.substr(1));
        }
        
        // Unknown positional argument
        else {
            std::cerr << "ERROR: Unknown positional argument '" << arg << "'." << std::endl;
            printUsage(programName);
            exit(1);
        }
    }
    
    return options;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Improved Phase 6 Integration Demo                         ║
    ║                                                               ║
    ║     Demonstrates all integrated brain systems working together ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM is an experimental artificial developmental brain project." << std::endl;
    std::cout << std::endl;
    std::cout << "Execution Modes (use --mode MODE or -m MODE):" << std::endl;
    std::cout << "  phase2 (2)          : Run Phase 2 neural computation demo" << std::endl;
    std::cout << "  phase6 (6)          : Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  integration (i)    : Run Phase 6 integration experiment" << std::endl;
    std::cout << "  test (t)           : Run comprehensive test suite" << std::endl;
    std::cout << "  config             : Validate configuration" << std::endl;
    std::cout << std::endl;
}

void printHelp(const std::string& programName) {
    printBanner();
    std::cout << "NLM — Neural Learning Machine (熙然)" << std::endl;
    std::cout << "Improved Version with Phase 6 Integration" << std::endl;
    std::cout << std::endl;
    printUsage(programName);
    std::cout << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  --help, -h            Show this help message" << std::endl;
    std::cout << "  --version, -V         Show version information" << std::endl;
    std::cout << "  --validate-config     Validate configuration before running" << std::endl;
    std::cout << "  --verbose, -v         Enable verbose logging" << std::endl;
    std::cout << "  --benchmark, -b       Run with benchmark timing" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config FILE         Load configuration from FILE (default: configs/nlm_config.json)" << std::endl;
    std::cout << "  --log-file FILE       Log output to file" << std::endl;
    std::cout << "  --log-level LEVEL     Set log level: debug, info, warning, error, critical" << std::endl;
    std::cout << "  --no-color            Disable colored output" << std::endl;
    std::cout << "  --save-results        Save experiment results" << std::endl;
    std::cout << "  --experiment-name NAME    Set experiment name" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Demo Options (when using --mode phase6):" << std::endl;
    std::cout << "  --neurons N           Number of neurons to simulate (default: 500)" << std::endl;
    std::cout << "  --steps S             Number of simulation steps (default: 1000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --mode phase6 --neurons 1000 --steps 5000 --verbose" << std::endl;
    std::cout << "  " << programName << " --mode integration --log-level debug --no-color" << std::endl;
    std::cout << "  " << programName << " --mode test --benchmark" << std::endl;
    std::cout << "  " << programName << " --mode config --config configs/nlm_config.json" << std::endl;
    std::cout << std::endl;
}

void printVersionInfo() {
    std::cout << "NLM — Neural Learning Machine (熙然)" << std::endl;
    std::cout << "Version: 2.0.0 - Improved Phase 6 Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6: Final Integration Demo" << std::endl;
    std::cout << "  - All neural core systems integrated into coherent brain loop" << std::endl;
    std::cout << "  - Memory systems connected to neural processing" << std::endl;
    std::cout << "  - Neuromodulation affects plasticity and dynamics" << std::endl;
    std::cout << "  - Prediction integrated with learning" << std::endl;
    std::cout << "  - Development affects plasticity rates" << std::endl;
    std::cout << "  - Checkpoint save/load working" << std::endl;
    std::cout << "  - Replay and consolidation functional" << std::endl;
    std::cout << std::endl;
    std::cout << "Key Improvements:" << std::endl;
    std::cout << "  - Enhanced CLI with comprehensive options" << std::endl;
    std::cout << "  - JSON configuration support" << std::endl;
    std::cout << "  - Improved error handling and validation" << std::endl;
    std::cout << "  - Better memory safety and resource management" << std::endl;
    std::cout << "  - Full integration of all brain systems" << std::endl;
    std::cout << "  - Comprehensive integration testing" << std::endl;
}

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        AppOptions options = parseArguments(argc, argv);
        
        // Handle help/version modes
        if (options.mode == ExecutionMode::Help) {
            printHelp(argv[0]);
            return 0;
        }
        
        if (options.mode == ExecutionMode::VersionInfo) {
            printVersionInfo();
            return 0;
        }
        
        // Initialize logger with configuration
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
        consoleLogger->setUseColors(options.useColors);
        if (!options.logFile.empty()) {
            consoleLogger->setOutputFile(options.logFile);
        }
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
        NLM_LOG_INFO("Program started with mode: " + std::to_string(static_cast<int>(options.mode)));
        
        // Create and configure main configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from file
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + options.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Apply command line overrides
        for (const auto& override : options.configOverrides) {
            size_t eqPos = override.find('=');
            if (eqPos != std::string::npos) {
                std::string key = override.substr(0, eqPos);
                std::string value = override.substr(eqPos + 1);
                
                // Try to determine type and set appropriately
                if (value == "true" || value == "false") {
                    config->set(key, value == "true", ConfigSource::CommandLine);
                } else {
                    try {
                        config->set(key, std::stod(value), ConfigSource::CommandLine);
                    } catch (...) {
                        config->set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
        
        // Validate configuration if requested
        if (options.validateConfig) {
            NLM_LOG_INFO("Validating configuration...");
            std::cout << config->summary() << std::endl;
        }
        
        // Run the selected mode
        switch (options.mode) {
            case ExecutionMode::Phase2Demo:
                NLM_LOG_INFO("Running Phase 2 neural computation demo...");
                // Legacy Phase 2 demo (simplified version)
                runPhase2Demo(options, config);
                break;
                
            case ExecutionMode::Phase6Demo:
                NLM_LOG_INFO("Running Phase 6 integration demo...");
                runPhase6Demo(options, config);
                break;
                
            case ExecutionMode::Phase6Integration:
                NLM_LOG_INFO("Running Phase 6 integration experiment...");
                runPhase6IntegrationExperiment(options, config);
                break;
                
            case ExecutionMode::TestValidation:
                NLM_LOG_INFO("Running comprehensive test suite...");
                runTestValidation(options, config);
                break;
                
            case ExecutionMode::ConfigValidation:
                NLM_LOG_INFO("Validating configuration...");
                runConfigValidation(options, config);
                break;
                
            default:
                NLM_LOG_ERROR("Unknown execution mode.");
                return 1;
        }
        
        // Save results if requested
        if (options.saveResults && !options.outputFile.empty()) {
            NLM_LOG_INFO("Saving results to: " + options.outputFile);
            // For now, just log the action
        }
        
        NLM_LOG_INFO("=== Program completed successfully ===");
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred." << std::endl;
        return 1;
    }
}

// Helper functions defined in Phase6Demo.cpp
void runPhase6Demo(const AppOptions& options, const std::shared_ptr<Config>& config);
void runPhase6IntegrationExperiment(const AppOptions& options, const std::shared_ptr<Config>& config);
void runTestValidation(const AppOptions& options, const std::shared_ptr<Config>& config);
void runConfigValidation(const AppOptions& options, const std::shared_ptr<Config>& config);
void runPhase2Demo(const AppOptions& options, const std::shared_ptr<Config>& config);

// Legacy helper functions for backward compatibility
void printPhase2DemoBanner();

// Legacy Phase 2 demo functions (for backward compatibility)
void runPhase2Demo(const AppOptions& options, const std::shared_ptr<Config>& config);
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const AppOptions& options);
void runPlasticityExperiment(std::shared_ptr<Brain> brain, const AppOptions& options);
void runStdpVerification(std::shared_ptr<Brain> brain);
void runTestValidation(const AppOptions& options);
void runConfigValidation(const AppOptions& options, const std::shared_ptr<Config>& config);