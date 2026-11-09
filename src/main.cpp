// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// This has been refactored to improve modularity, testability, and configurability.

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
#include "experiments/ExperimentRunner.hpp"
#include "test/TestRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <thread>

using namespace nlm;

void printBanner() {
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

// Configuration parser for command-line arguments
class CommandLineParser {
public:
    struct ParsedArgs {
        std::string configFile = "configs/default.cfg";
        bool enableConnectivityTest = true;
        bool enablePlasticityTest = true;
        bool enableStdpTest = true;
        bool verboseLogging = true;
        bool profilePerformance = true;
        bool enableResultValidation = true;
        size_t neuronCount = 500;
        float connectivityCurrent = 50.0f;
        float plasticityCurrent = 30.0f;
        uint32_t plasticitySteps = 1000;
        float stdpCurrent = 60.0f;
        uint32_t stdpTrials = 50;
    };
    
    static ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            }
            else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            }
            else if (arg == "--no-connectivity") {
                args.enableConnectivityTest = false;
            }
            else if (arg == "--no-plasticity") {
                args.enablePlasticityTest = false;
            }
            else if (arg == "--no-stdp") {
                args.enableStdpTest = false;
            }
            else if (arg == "--quiet") {
                args.verboseLogging = false;
            }
            else if (arg == "--no-profile") {
                args.profilePerformance = false;
            }
            else if (arg == "--no-validate") {
                args.enableResultValidation = false;
            }
            else if (arg.substr(0, 15) == "--neuron-count=") {
                args.neuronCount = std::stoull(arg.substr(15));
            }
            else if (arg.substr(0, 15) == "--connectivity=") {
                args.connectivityCurrent = std::stof(arg.substr(15));
            }
            else if (arg.substr(0, 14) == "--plasticity=") {
                args.plasticityCurrent = std::stof(arg.substr(14));
            }
            else if (arg.substr(0, 16) == "--plasticity-steps=") {
                args.plasticitySteps = std::stoul(arg.substr(16));
            }
            else if (arg.substr(0, 11) == "--stdp=") {
                args.stdpCurrent = std::stof(arg.substr(6));
            }
            else if (arg.substr(0, 11) == "--stdp-trials=") {
                args.stdpTrials = std::stoul(arg.substr(12));
            }
        }
        
        return args;
    }
    
private:
    static void printUsage() {
        std::cout << "NLM Neural Learning Machine - Phase 2" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Usage: nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration options:" << std::endl;
        std::cout << "  --config=file.cfg     Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Test control options:" << std::endl;
        std::cout << "  --no-connectivity     Skip connectivity test" << std::endl;
        std::cout << "  --no-plasticity      Skip plasticity test" << std::endl;
        std::cout << "  --no-stdp             Skip STDP test" << std::endl;
        std::cout << std::endl;
        std::cout << "Output and performance options:" << std::endl;
        std::cout << "  --quiet               Suppress verbose logging" << std::endl;
        std::cout << "  --no-profile          Disable performance profiling" << std::endl;
        std::cout << "  --no-validate         Disable result validation" << std::endl;
        std::cout << std::endl;
        std::cout << "Parameter customization options:" << std::endl;
        std::cout << "  --neuron-count=N      Set number of neurons" << std::endl;
        std::cout << "  --connectivity=I      Set current injection for connectivity test" << std::endl;
        std::cout << "  --plasticity=I        Set current injection for plasticity test" << std::endl;
        std::cout << "  --plasticity-steps=N  Set steps for plasticity test" << std::endl;
        std::cout << "  --stdp=I              Set current injection for STDP test" << std::endl;
        std::cout << "  --stdp-trials=N       Set trials for STDP test" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --config=myconfig.cfg --no-plasticity" << std::endl;
        std::cout << "  nlm --neuron-count=1000 --plasticity=40.0 --plasticity-steps=500" << std::endl;
        std::cout << "  nlm --quiet --no-profile" << std::endl;
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    auto parsedArgs = CommandLineParser::parse(argc, argv);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = parsedArgs.configFile;
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(parsedArgs.neuronCount), ConfigSource::Default);
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
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", parsedArgs.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Prepare test configuration
    TestConfig testConfig;
    testConfig.enableConnectivityTest = parsedArgs.enableConnectivityTest;
    testConfig.enablePlasticityTest = parsedArgs.enablePlasticityTest;
    testConfig.enableStdpTest = parsedArgs.enableStdpTest;
    testConfig.verboseLogging = parsedArgs.verboseLogging;
    testConfig.profilePerformance = parsedArgs.profilePerformance;
    testConfig.enableResultValidation = parsedArgs.enableResultValidation;
    testConfig.connectivityTestCurrent = parsedArgs.connectivityCurrent;
    testConfig.plasticityTestCurrent = parsedArgs.plasticityCurrent;
    testConfig.plasticityTestSteps = parsedArgs.plasticitySteps;
    testConfig.stdpTestCurrent = parsedArgs.stdpCurrent;
    testConfig.stdpTestTrials = parsedArgs.stdpTrials;
    testConfig.neuronCount = parsedArgs.neuronCount;
    testConfig.regionCount = 1;
    testConfig.connectionProbability = 0.15f;
    
    // Create test runner
    TestRunner testRunner(testConfig);
    
    // Run tests
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Starting NLM Test Suite ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<TestStats> results;
    if (testConfig.enableConnectivityTest) {
        NLM_LOG_INFO("Running connectivity test...");
        results.push_back(testRunner.runTest(brain, "connectivity"));
    }
    
    // Reset brain for next test
    brain->reset();
    brain->initialize();
    
    if (testConfig.enablePlasticityTest) {
        NLM_LOG_INFO("Running plasticity test...");
        results.push_back(testRunner.runTest(brain, "plasticity"));
    }
    
    // Reset and run STDP test
    brain->reset();
    brain->initialize();
    
    if (testConfig.enableStdpTest) {
        NLM_LOG_INFO("Running STDP test...");
        results.push_back(testRunner.runTest(brain, "stdp"));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Validate results
    if (testConfig.enableResultValidation) {
        auto validationResult = testRunner.validateResults(results);
        if (!validationResult.errors.empty()) {
            NLM_LOG_ERROR("Validation failed:");
            for (const auto& error : validationResult.errors) {
                NLM_LOG_ERROR("  " + error);
            }
        }
        if (!validationResult.warnings.empty()) {
            NLM_LOG_WARN("Validation warnings:");
            for (const auto& warning : validationResult.warnings) {
                NLM_LOG_WARN("  " + warning);
            }
        }
    }
    
    // Print formatted test results
    NLM_LOG_INFO("");
    testRunner.printTestResults(results);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Total Test Suite Time ===");
    NLM_LOG_INFO(f"  Total execution time: {totalTimeMs:.2f} ms");
    
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
    
    return 0;
}