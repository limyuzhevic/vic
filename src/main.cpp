// NLM (熙然) - Neural Learning Machine
// Phase 1: Skeleton and Architecture
//
// This is a placeholder implementation. Real neural computation will be
// implemented in Phase 2 and beyond.

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 1: Architecture Skeleton                            ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This is NOT a working neural network yet.                  ║
    ║     Real neural computation coming in Phase 2+.                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 1 Skeleton...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 1 Skeleton ===");
    NLM_LOG_INFO("This is a placeholder implementation.");
    NLM_LOG_INFO("Real neural computation will be implemented in Phase 2+.");
    NLM_LOG_INFO("");
    
    // Load configuration
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
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)));
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
    NLM_LOG_INFO("");
    
    // Initialize random generator
    uint64_t seed = config->getOr<int64_t>("random_seed", 42);
    RandomGenerator rng(seed);
    NLM_LOG_INFO("Random generator initialized with seed: " + std::to_string(seed));
    
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
    
    // Initialize environment (placeholder grid world)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing Environment...");
    auto environment = std::make_shared<GridWorldEnvironment>(10, 10);
    NLM_LOG_INFO("Environment: " + std::string(environment->getName()));
    NLM_LOG_INFO("Action space size: " + std::to_string(environment->getActionSpaceSize()));
    NLM_LOG_INFO("Observation space size: " + std::to_string(environment->getObservationSpaceSize()));
    
    // Run a few skeleton simulation steps
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running skeleton simulation steps...");
    
    SimulationStep maxSteps = 10;
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        clock.advance();
        
        // Brain step
        brain->step(step, clock.getTime());
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        auto observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Log progress every 5 steps
        if ((step + 1) % 5 == 0 || step == maxSteps - 1) {
            NLM_LOG_INFO("Step " + std::to_string(step + 1) + "/" + std::to_string(maxSteps) +
                        " | Time: " + std::to_string(clock.getTime()) + "s" +
                        " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Reward: " + std::to_string(environment->getLastReward()));
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Create a simple experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Creating test experiment...");
    ExperimentRunner runner;
    auto experiment = runner.createExperiment("Test_Experiment");
    experiment->setSeed(seed);
    experiment->recordMetric("total_reward", 0.0);
    experiment->recordMetric("steps_completed", maxSteps);
    experiment->addNote("This is a Phase 1 skeleton test.");
    NLM_LOG_INFO("Experiment created: " + experiment->getName());
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 1 Skeleton Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 1 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Project compiles with C++20");
    NLM_LOG_INFO("  ✓ Architecture exists with clean interfaces");
    NLM_LOG_INFO("  ✓ Core types defined (NeuronId, SynapseId, etc.)");
    NLM_LOG_INFO("  ✓ Random generator with deterministic seed");
    NLM_LOG_INFO("  ✓ Simulation clock implemented");
    NLM_LOG_INFO("  ✓ Brain skeleton with regions and populations");
    NLM_LOG_INFO("  ✓ Neuron and Synapse interfaces defined");
    NLM_LOG_INFO("  ✓ Environment interface defined");
    NLM_LOG_INFO("  ✓ Experiment framework initialized");
    NLM_LOG_INFO("  ✓ Configuration system operational");
    NLM_LOG_INFO("  ✓ Logging system functional");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Will Implement:");
    NLM_LOG_INFO("  - Real membrane potential dynamics");
    NLM_LOG_INFO("  - Real synaptic transmission");
    NLM_LOG_INFO("  - Real STDP and Hebbian plasticity");
    NLM_LOG_INFO("  - Real spiking neural computation");
    NLM_LOG_INFO("  - Real developmental processes");
    NLM_LOG_INFO("  - And more...");
    NLM_LOG_INFO("");
    
    return 0;
}
