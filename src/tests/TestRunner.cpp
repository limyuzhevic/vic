// TestRunner.cpp - Main orchestrator for NLM tests
//
// This file serves as the new main() function for the test suite, coordinating
// all test functions with command-line argument parsing, error handling, and
// logging. It maintains backward compatibility by preserving the original test
// sequence while adding modularity and configurability.

#include "TestRunner.hpp"
#include "BrainConnectivityTest.hpp"
#include "PlasticityTest.hpp"
#include "StdpTest.hpp"
#include "LearningExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {
namespace tests {

void TestRunner::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

TestRunner::TestRunner(int argc, char** argv) 
    : argc(argc), argv(argv), config_(nullptr), brain_(nullptr), clock_(nullptr) {
    
    // Parse command line arguments
    parseArgs();
    
    // Initialize logger
    initializeLogger();
    
    // Load configuration
    loadConfiguration();
    
    // Initialize simulation clock
    double timestep = config_->getOr<double>("simulation_timestep", 0.001);
    clock_ = std::make_shared<SimulationClock>(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Determine which tests to run
    determineTestsToRun();
}

void TestRunner::initializeLogger() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void TestRunner::loadConfiguration() {
    config_ = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config_->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config_->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config_->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config_->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config_->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config_->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config_->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
}

void TestRunner::determineTestsToRun() {
    // Parse command line arguments to determine which tests to run
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        else if (arg == "--version") {
            printVersion();
            exit(0);
        }
        else if (arg == "--all" || arg == "-a") {
            testsToRun = {TestType::Connectivity, TestType::Plasticity, TestType::Stdp};
        }
        else if (arg == "--connectivity" || arg == "-c") {
            testsToRun = {TestType::Connectivity};
        }
        else if (arg == "--plasticity" || arg == "-p") {
            testsToRun = {TestType::Plasticity};
        }
        else if (arg == "--stdp" || arg == "-s") {
            testsToRun = {TestType::Stdp};
        }
        else if (arg.substr(0, 7) == "--test=") {
            std::string testName = arg.substr(7);
            TestType test = stringToTestType(testName);
            if (test != TestType::Unknown) {
                testsToRun = {test};
            }
        }
    }
    
    // If no specific test was requested, run all tests
    if (testsToRun.empty()) {
        testsToRun = {TestType::Connectivity, TestType::Plasticity, TestType::Stdp};
    }
}

TestRunner::TestType TestRunner::stringToTestType(const std::string& str) {
    if (str == "connectivity" || str == "c") return TestType::Connectivity;
    if (str == "plasticity" || str == "p") return TestType::Plasticity;
    if (str == "stdp" || str == "s") return TestType::Stdp;
    return TestType::Unknown;
}

void TestRunner::printHelp() {
    std::cout << "NLM Test Runner - Usage:" << std::endl;
    std::cout << "  ./nlm_test [--help|-h] [--version] [--all|-a] [--connectivity|-c] [--plasticity|-p] [--stdp|-s] [--test=<type>]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h         Show this help message" << std::endl;
    std::cout << "  --version          Show version information" << std::endl;
    std::cout << "  --all, -a          Run all tests (default)" << std::endl;
    std::cout << "  --connectivity, -c  Run only basic connectivity test" << std::endl;
    std::cout << "  --plasticity, -p   Run only plasticity learning experiment" << std::endl;
    std::cout << "  --stdp, -s         Run only STDP verification test" << std::endl;
    std::cout << "  --test=<type>      Run specific test (connectivity, plasticity, stdp)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "The original test sequence is maintained for backward compatibility:" << std::endl;
    std::cout << "  1. Basic Connectivity Test" << std::endl;
    std::cout << "  2. Plasticity Learning Experiment" << std::endl;
    std::cout << "  3. STDP Verification Test" << std::endl;
}

void TestRunner::printVersion() {
    std::cout << "NLM Phase 2 Test Runner v0.1.0" << std::endl;
    std::cout << "Copyright (c) 2026 NLM Project" << std::endl;
}

void TestRunner::initializeBrain() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain_ = std::make_shared<Brain>(config_);
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw std::runtime_error("Brain initialization failed");
    }
    
    brain_->logStatus();
}

void TestRunner::runConnectivityTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Basic Connectivity Test ===");
    
    try {
        BrainConnectivityTest::run(brain_);
        NLM_LOG_INFO("✓ Basic Connectivity Test completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Basic Connectivity Test failed: " + std::string(e.what()));
        throw;
    }
}

void TestRunner::runPlasticityTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Plasticity Learning Experiment ===");
    
    try {
        // Reset brain for plasticity experiment
        brain_->reset();
        brain_->initialize();
        
        PlasticityTest::run(brain_);
        NLM_LOG_INFO("✓ Plasticity Learning Experiment completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Plasticity Learning Experiment failed: " + std::string(e.what()));
        throw;
    }
}

void TestRunner::runStdpTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running STDP Verification Test ===");
    
    try {
        // Reset and run STDP verification
        brain_->reset();
        brain_->initialize();
        
        StdpTest::run(brain_);
        NLM_LOG_INFO("✓ STDP Verification Test completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("STDP Verification Test failed: " + std::string(e.what()));
        throw;
    }
}

void TestRunner::runAllTests() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running All Tests (Backward Compatible Sequence) ===");
    
    // Run tests in original order for backward compatibility
    runConnectivityTest();
    runPlasticityTest();
    runStdpTest();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Tests Completed Successfully ===");
}

void TestRunner::printFinalBrainStatus() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain_->logStatus();
}

void TestRunner::printPhase2Complete() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
}

int TestRunner::run() {
    try {
        printBanner();
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
        
        initializeLogger();
        loadConfiguration();
        
        // Initialize brain
        initializeBrain();
        
        // Run selected tests
        if (testsToRun.size() == 1) {
            // Run only the specified test
            switch (testsToRun[0]) {
                case TestType::Connectivity:
                    runConnectivityTest();
                    break;
                case TestType::Plasticity:
                    runPlasticityTest();
                    break;
                case TestType::Stdp:
                    runStdpTest();
                    break;
            }
        } else {
            // Run all tests in original sequence
            runAllTests();
        }
        
        // Final brain status
        printFinalBrainStatus();
        
        // Phase 2 completion message
        printPhase2Complete();
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test execution failed: " + std::string(e.what()));
        return 1;
    }
}

} // namespace tests
} // namespace nlm
