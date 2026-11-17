// NLM Main Entry Point
// Phase 2: Real Neural Computation
// This file provides a modular test runner with command-line interface

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace nlm;

// Forward declarations for test functions
namespace test_modules {
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
}

// Command-line argument parsing
struct CommandLineArgs {
    bool help = false;
    bool version = false;
    bool verbose = false;
    std::string configFile = "configs/default.cfg";
    std::vector<std::string> selectedTests;
    bool listTests = false;
    bool runAll = false;
};

// Test module information structure
struct TestModuleInfo {
    std::string name;
    std::string description;
    std::string module;
    bool (*func)(std::shared_ptr<Brain>);
    bool available;
};

// Available test modules
const std::vector<TestModuleInfo> AVAILABLE_TESTS = {
    {"basic_connectivity", "Basic Neural Connectivity Test", "connectivity", 
     [](std::shared_ptr<Brain> brain) { test_modules::runBasicConnectivityTest(brain); return true; }, true},
    {"plasticity", "Plasticity Learning Experiment", "plasticity",
     [](std::shared_ptr<Brain> brain) { test_modules::runPlasticityExperiment(brain); return true; }, true},
    {"stdp", "STDP Verification Test", "stdp",
     [](std::shared_ptr<Brain> brain) { test_modules::runStdpVerification(brain); return true; }, true}
};

// Parse command-line arguments
CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version" || arg == "-v") {
            args.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            args.verbose = true;
        } else if (arg == "--list-tests") {
            args.listTests = true;
        } else if (arg == "--all" || arg == "-a") {
            args.runAll = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.find('--') == 0 && arg.find('=') != std::string::npos) {
            // Handle --key=value format
            std::string key = arg.substr(2, arg.find('=') - 2);
            std::string value = arg.substr(arg.find('=') + 1);
            
            if (key == "test") {
                args.selectedTests.push_back(value);
            }
        } else if (arg.find('-') == 0) {
            // Handle -key format
            std::string key = arg.substr(1);
            
            if (key == "test") {
                if (i + 1 < argc) {
                    args.selectedTests.push_back(argv[++i]);
                }
            }
        } else {
            // Positional argument - could be a test name
            if (std::find(args.selectedTests.begin(), args.selectedTests.end(), arg) == args.selectedTests.end()) {
                args.selectedTests.push_back(arg);
            }
        }
    }
    
    return args;
}

// Print help information
void printHelp() {
    std::cout << R"(
NLM Phase 2 Test Runner
=======================

Usage: nlm [options] [test1 test2 ...]

Options:
  -h, --help              Show this help message
  -v, --version           Show version information
  -V, --verbose          Enable verbose output
  --config file          Specify configuration file (default: configs/default.cfg)
  --list-tests           List available tests
  -a, --all              Run all available tests

Tests (can be specified as positional arguments or with --test=NAME):
  basic_connectivity    Basic Neural Connectivity Test
  plasticity            Plasticity Learning Experiment  
  stdp                  STDP Verification Test

Examples:
  nlm                           Run all tests
  nlm --test=plasticity          Run only plasticity test
  nlm basic_connectivity stdp    Run two specific tests
  nlm --config myconfig.cfg      Use custom config file
  nlm --list-tests               Show available tests

For more information, visit: https://github.com/nlm-project/nlm

NLM - Neural Learning Machine (熙然)
An experimental artificial developmental brain.
)" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM Phase 2 Test Runner" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: " << __cplusplus << std::endl;
}

// List available tests
void listTests() {
    std::cout << "Available Tests:" << std::endl;
    std::cout << "=============== " << std::endl;
    
    for (const auto& test : AVAILABLE_TESTS) {
        std::cout << "  " << test.name << "" << std::endl;
        std::cout << "    " << test.description << "" << std::endl;
        std::cout << "    Module: " << test.module << "" << std::endl;
        std::cout << "" << std::endl;
    }
}

// Initialize logging system
void initializeLogging(bool verbose) {
    auto logger = std::make_shared<Logger>();
    
    if (verbose) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
    } else {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("NLM Phase 2 Test Runner initialized");
    if (verbose) {
        NLM_LOG_DEBUG("Verbose logging enabled");
    }
}

// Initialize configuration
std::shared_ptr<Config> initializeConfig(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARNING("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    return config;
}

// Run all tests
void runAllTests(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running All Tests ===");
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run tests in sequence
    for (const auto& test : AVAILABLE_TESTS) {
        if (test.available) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Running: " + test.name + " ===");
            
            try {
                // Reset and reinitialize brain for each test
                brain->reset();
                brain->initialize();
                
                test.func(brain);
                
                NLM_LOG_INFO("=== Test Completed: " + test.name + " ===");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Test " + test.name + " failed with exception: " + std::string(e.what()));
            }
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Tests Completed ===");
}

// Run selected tests
void runSelectedTests(std::shared_ptr<Config> config, const std::vector<std::string>& selectedTests) {
    NLM_LOG_INFO("=== Running Selected Tests ===");
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    int testCount = 0;
    int passedCount = 0;
    
    for (const auto& testName : selectedTests) {
        bool found = false;
        for (const auto& test : AVAILABLE_TESTS) {
            if (test.name == testName && test.available) {
                found = true;
                testCount++;
                
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Running Test " + std::to_string(testCount) + ": " + test.name + " ===");
                
                try {
                    // Reset and reinitialize brain for each test
                    brain->reset();
                    brain->initialize();
                    
                    if (test.func(brain)) {
                        passedCount++;
                        NLM_LOG_INFO("=== Test Passed: " + test.name + " ===");
                    } else {
                        NLM_LOG_ERROR("=== Test Failed: " + test.name + " ===");
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Test " + test.name + " failed with exception: " + std::string(e.what()));
                }
                
                break;
            }
        }
        
        if (!found) {
            NLM_LOG_WARNING("Test not found: " + testName);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Summary: " + std::to_string(passedCount) + "/" + std::to_string(testCount) + " passed ===");
}

// Main entry point
int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Handle help/version/list options
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    if (args.listTests) {
        listTests();
        return 0;
    }
    
    // Initialize logging
    initializeLogging(args.verbose);
    
    NLM_LOG_INFO("Starting NLM Phase 2 Neural Computation...");
    
    // Initialize configuration
    auto config = initializeConfig(args.configFile);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    if (args.runAll || args.selectedTests.empty()) {
        // Run all tests
        runAllTests(config);
    } else {
        // Run selected tests
        runSelectedTests(config, args.selectedTests);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 2 Complete ===");
    NLM_LOG_INFO("");
    
    return 0;
}
