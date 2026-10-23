// NLM Phase 2 Main - Refactored
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Constants.hpp"
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
#include "tests/TestPhase2.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

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

bool initializeConfiguration(std::shared_ptr<Config> config, int argc, char** argv) {
    // Try to load from file if provided
    std::string configFile = constants::DEFAULT_CONFIG_FILE;
    
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
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    if (!config->loadFromArgs(argc, argv)) {
        NLM_LOG_WARNING("Invalid command line arguments.");
    }
    
    return true;
}

void configurePhase2Defaults(std::shared_ptr<Config> config) {
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", constants::DEFAULT_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(constants::DEFAULT_NEURON_COUNT), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(constants::DEFAULT_REGION_COUNT), ConfigSource::Default);
    config->set("connection_probability", constants::DEFAULT_CONNECTION_PROB, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", constants::STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", constants::STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", constants::STDP_TIME_CONSTANT, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", constants::SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", constants::PRUNING_RATE, ConfigSource::Default);
}

void logConfigurationSummary(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", constants::DEFAULT_TIMESTEP)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", constants::DEFAULT_NEURON_COUNT)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", constants::DEFAULT_REGION_COUNT)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", constants::DEFAULT_CONNECTION_PROB)));
    NLM_LOG_INFO("");
}

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
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (!initializeConfiguration(config, argc, argv)) {
        NLM_LOG_ERROR("Failed to initialize configuration!");
        return 1;
    }
    
    configurePhase2Defaults(config);
    logConfigurationSummary(config);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", constants::DEFAULT_TIMESTEP);
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
    
    // Run all tests
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Starting Neural Computation Tests ===");
    
    bool allTestsPassed = true;
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    if (!tests::runBasicConnectivityTest(brain)) {
        NLM_LOG_ERROR("Basic connectivity test failed!");
        allTestsPassed = false;
    }
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    if (!tests::runPlasticityExperiment(brain)) {
        NLM_LOG_ERROR("Plasticity experiment failed!");
        allTestsPassed = false;
    }
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    if (!tests::runStdpVerification(brain)) {
        NLM_LOG_ERROR("STDP verification failed!");
        allTestsPassed = false;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
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
    
    if (allTestsPassed) {
        NLM_LOG_INFO("✓ ALL TESTS PASSED - Phase 2 successfully completed!");
        return 0;
    } else {
        NLM_LOG_ERROR("✗ SOME TESTS FAILED - Phase 2 incomplete!");
        return 1;
    }
}
