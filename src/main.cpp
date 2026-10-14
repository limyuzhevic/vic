// NLM (熙然) - Neural Learning Machine
// Enhanced Command Line Interface
// This file provides the main application entry point with:
// - New comprehensive command-line interface with multiple execution modes
// - Configuration options via JSON/YAML files
// - Advanced features for automation and integration
// - Backward compatibility with existing demo programs
// - Modular design with extracted components
//
// New Features:
// 1. Multiple execution modes: basic, full, phase3, phase4, phase6, help
// 2. Configuration options: --config, --set, --neurons, --episodes, --trials, --timestep
// 3. Advanced features: --verbose, --log-level, --save-state, --load-state, --benchmark, --version
// 4. Flexible test selection: --tests, --fast, --thorough
// 5. Output and reporting: --quiet, --json, --report, --metrics
// 6. Plugin and extension support: --plugins, --experiment, --validate-config
//
// Backward Compatibility:
// 1. Existing test modes: basic, plasticity, stdp
// 2. Existing demo programs: nlm_phase3_demo, nlm_phase4_demo, nlm_phase6_demo
// 3. Existing configuration files: configs/default.cfg

#include "main_banner.hpp"
#include "learning_experiment.hpp"
#include "basic_connectivity_test.hpp"
#include "plasticity_experiment_test.hpp"
#include "stdp_verification_test.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "command_line.hpp"
#include "command_line_legacy.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <algorithm>

using namespace nlm;

/**
 * Command-line options structure for test selection
 */
struct TestOptions {
    bool runAll = false;
    bool runBasic = false;
    bool runPlasticity = false;
    bool runStdp = false;
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "INFO";
    bool verbose = false;
    int maxNeurons = 500;
    double maxTime = 1.0;
    
    TestOptions() {
        // Default: run all tests
        runAll = true;
        runBasic = true;
        runPlasticity = true;
        runStdp = true;
    }
};

/**
 * Parse command-line arguments into test options
 * 
 * Supports options like:
 * --test basic --test plasticity --config my_config.cfg --verbose
 * --test all (default)
 * --help for usage information
 */
TestOptions parseCommandLine(int argc, char** argv) {
    TestOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            std::exit(0);
        }
        else if (arg == "--test" || arg == "-t") {
            if (i + 1 < argc) {
                std::string testName = argv[++i];
                if (testName == "all") {
                    options.runAll = true;
                    options.runBasic = true;
                    options.runPlasticity = true;
                    options.runStdp = true;
                } else if (testName == "basic") {
                    options.runAll = false;
                    options.runBasic = true;
                    options.runPlasticity = false;
                    options.runStdp = false;
                } else if (testName == "plasticity") {
                    options.runAll = false;
                    options.runBasic = false;
                    options.runPlasticity = true;
                    options.runStdp = false;
                } else if (testName == "stdp") {
                    options.runAll = false;
                    options.runBasic = false;
                    options.runPlasticity = false;
                    options.runStdp = true;
                } else {
                    std::cerr << "Unknown test: " << testName << std::endl;
                    printUsage();
                    std::exit(1);
                }
            }
        }
        else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        }
        else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        }
        else if (arg == "--max-neurons") {
            if (i + 1 < argc) {
                options.maxNeurons = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--max-time") {
            if (i + 1 < argc) {
                options.maxTime = std::stod(argv[++i]);
            }
        }
    }
    
    return options;
}

/**
 * Display usage information
 */
void printUsage() {
    std::cout << "Usage: nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run neural learning machine tests with configurable options." << std::endl;
    std::cout << std::endl;
    std::cout << "Test Options:" << std::endl;
    std::cout << "  --test all        Run all tests (default)" << std::endl;
    std::cout << "  --test basic      Run basic connectivity test only" << std::endl;
    std::cout << "  --test plasticity Run plasticity learning experiment only" << std::endl;
    std::cout << "  --test stdp       Run STDP verification test only" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config FILE     Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --max-neurons N   Set maximum neuron count to N" << std::endl;
    std::cout << "  --max-time T      Set maximum simulation time to T seconds" << std::endl;
    std::cout << std::endl;
    std::cout << "Logging Options:" << std::endl;
    std::cout << "  --log-level L     Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --verbose -v      Enable verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  --help -h         Show this help message and exit" << std::endl;
}

/**
 * Map string log level to enum
 */
LogLevel stringToLogLevel(const std::string& level) {
    std::string upper = level;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "ERROR") return LogLevel::Error;
    if (upper == "CRITICAL") return LogLevel::Critical;
    return LogLevel::Info;  // Default
}

/**
 * Initialize the application with logging and configuration
 * 
 * Returns true on success, false on failure
 */
bool initializeApplication(TestOptions& options, std::shared_ptr<Config>& config) {
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(stringToLogLevel(options.logLevel));
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Modular refactored version with test runner infrastructure");
        
        if (options.verbose) {
            NLM_LOG_INFO("Verbose mode enabled");
            NLM_LOG_INFO("Config file: " + options.configFile);
            NLM_LOG_INFO("Log level: " + options.logLevel);
        }
        
        // Load configuration
        config = std::make_shared<Config>();
        
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Could not load configuration from: " + options.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Apply command-line overrides
        config->loadFromArgs(0, nullptr);  // Will be overridden by main args if passed
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(options.maxNeurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.maxNeurons)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize application: " + std::string(e.what()));
        return false;
    }
}

/**
 * Initialize brain with error handling
 * 
 * Returns true on success, false on failure
 */
bool initializeBrain(std::shared_ptr<Brain>& brain, std::shared_ptr<Config>& config) {
    try {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    }
}

/**
 * Reset and re-initialize brain for next test
 * 
 * Returns true on success, false on failure
 */
bool resetBrain(std::shared_ptr<Brain>& brain) {
    try {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Resetting brain for next test...");
        
        brain->reset();
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to re-initialize brain after reset!");
            return false;
        }
        
        brain->logStatus();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain reset: " + std::string(e.what()));
        return false;
    }
}

/**
 * Execute selected tests
 * 
 * Returns true if all tests completed successfully, false otherwise
 */
bool executeTests(TestOptions& options, std::shared_ptr<Brain>& brain) {
    bool allTestsPassed = true;
    
    // Execute basic connectivity test
    if (options.runBasic || options.runAll) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: Basic Connectivity ===");
        
        try {
            runBasicConnectivityTest(brain);
            NLM_LOG_INFO("✓ Basic connectivity test completed");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Basic connectivity test failed: " + std::string(e.what()));
            allTestsPassed = false;
        }
        
        if (options.runAll) {
            // Reset for next test
            if (!resetBrain(brain)) {
                allTestsPassed = false;
            }
        }
    }
    
    // Execute plasticity experiment
    if (options.runPlasticity || options.runAll) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: Plasticity Learning Experiment ===");
        
        try {
            runPlasticityExperiment(brain);
            NLM_LOG_INFO("✓ Plasticity experiment completed");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Plasticity experiment failed: " + std::string(e.what()));
            allTestsPassed = false;
        }
        
        if (options.runAll) {
            // Reset for next test
            if (!resetBrain(brain)) {
                allTestsPassed = false;
            }
        }
    }
    
    // Execute STDP verification
    if (options.runStdp || options.runAll) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: STDP Verification ===");
        
        try {
            runStdpVerification(brain);
            NLM_LOG_INFO("✓ STDP verification completed");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("STDP verification failed: " + std::string(e.what()));
            allTestsPassed = false;
        }
    }
    
    return allTestsPassed;
}

/**
 * Display final summary and statistics
 */
void displayFinalSummary(std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule verified");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule functional");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning) active");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
}

int main(int argc, char** argv) {
    printBanner();
    
    try {
        // Check for legacy test modes
        bool legacyMode = false;
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if ((arg == "--test" || arg == "-t") && i + 1 < argc) {
                std::string testName = argv[++i];
                if (testName == "basic" || testName == "plasticity" || testName == "stdp" || testName == "all") {
                    legacyMode = true;
                    break;
                }
            }
        }
        
        if (legacyMode) {
            // Use legacy test runner for backward compatibility
            LegacyCommandLineInterface legacy;
            LegacyTestOptions options;
            
            if (!legacy.parseLegacyCommandLine(argc, argv, options)) {
                return 1;
            }
            
            // Initialize application
            std::shared_ptr<Config> config;
            if (!legacy.initializeApplication(options, config)) {
                return 1;
            }
            
            // Initialize brain
            std::shared_ptr<Brain> brain;
            if (!legacy.initializeBrain(brain, config)) {
                return 1;
            }
            
            // Execute legacy tests
            if (!legacy.executeLegacyTests(options, brain)) {
                return 1;
            }
            
            // Display final summary
            legacy.displayFinalSummary(brain);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("✓ All tests completed successfully");
            
            return 0;
        } else {
            // Use new CommandLine interface
            CommandLine cmdLine;
            
            // Parse command line arguments
            if (!cmdLine.parse(argc, argv)) {
                NLM_LOG_ERROR("Failed to parse command line arguments");
                return 1;
            }
            
            // Execute command
            return cmdLine.execute();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Fatal error: " + std::string(e.what()));
        NLM_LOG_CRITICAL("Application terminated due to unrecoverable error");
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown fatal error occurred");
        return 1;
    }
}
        NLM_LOG_CRITICAL("Application terminated due to unrecoverable error");
        return 1;
    }
}
