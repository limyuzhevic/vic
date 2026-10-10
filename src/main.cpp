// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - Demonstration Application
//
// This application demonstrates the NLM (Neural Learning Machine) brain
// with configuration constants and improved error handling.
// The actual unit tests are available in tests/test_integration.cpp

#include "core/Config/Config.hpp"
#include "core/config/Constants.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
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
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
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
    
    // Load configuration with improved error handling
    auto config = std::make_shared<Config>();
    
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
    
    // Load config from file (now with better error handling)
    if (!config->loadFromFile(configFile)) {
        NLM_LOG_WARNING("Config file not found or could not be loaded: " + configFile);
        NLM_LOG_INFO("Using default configuration.");
    } else {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 using constants from config namespace
    config->set("random_seed", static_cast<int64_t>(config::DEFAULT_RANDOM_SEED), ConfigSource::Default);
    config->set("simulation_timestep", config::SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(config::DEFAULT_NEURON_COUNT), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(config::DEFAULT_REGION_COUNT), ConfigSource::Default);
    config->set("connection_probability", config::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", config::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", config::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", config::DEFAULT_STDP_TAU, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", config::SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", config::PRUNING_RATE, ConfigSource::Default);
    
    // Log configuration summary using constants
    double timestep = config->getOr<double>("simulation_timestep", config::SIMULATION_TIMESTEP);
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration (using constants from config namespace):");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", config::DEFAULT_RANDOM_SEED)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(timestep) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", config::DEFAULT_NEURON_COUNT)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", config::DEFAULT_REGION_COUNT)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", config::DEFAULT_CONNECTION_PROBABILITY)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
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
    
    // Note: The integration tests have been moved to tests/test_integration.cpp
    // For demonstration, just run a short simulation to show the brain is working
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running demonstration simulation (10 steps)...");
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 5 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
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
    NLM_LOG_INFO("  ✓ Configuration constants provide stable parameters");
    NLM_LOG_INFO("  ✓ Error handling improved for robustness");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Run 'make test' to execute the complete integration test suite:");
    NLM_LOG_INFO("  - Unit tests for individual components");
    NLM_LOG_INFO("  - Integration tests for system behavior");
    NLM_LOG_INFO("  - Tests use configuration constants for consistency");
    
    return 0;
}