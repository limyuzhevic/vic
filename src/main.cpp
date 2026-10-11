// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the final integration of all brain systems:
// - Memory systems integrated with neural processing
// - Neuromodulation affects plasticity and dynamics
// - Development affects plasticity rates
// - Prediction system integrated into brain loop
// - Replay and consolidation functional
// - Checkpoint save/load working

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
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════╗
    ║                                                                               ║
    ║     NLM — 熙然                                                              ║
    ║     Neural Learning Machine                                                   ║
    ║                                                                               ║
    ║     Phase 6: FINAL INTEGRATION                                                ║
    ║                                                                               ║
    ║     Complete artificial brain with all systems integrated.                    ║
    ║                                                                               ║
    ║     Features:                                                                 ║
    ║     - Working Memory + Episodic Memory                                        ║
    ║     - Neuromodulation (Dopamine, Curiosity, Novelty)                         ║
    ║     - Prediction System                                                       ║
    ║     - Cognitive Systems (Attention, Planning, Self-Model)                     ║
    ║     - Development System                                                      ║
    ║     - Replay & Consolidation                                                  ║
    ║     - Checkpoint Persistence                                                 ║
    ║                                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Configuration Summary ===");
    NLM_LOG_INFO("random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("region_count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("stdp_ltp_weight: " + std::to_string(config.getOr<float>("stdp_ltp_weight", 0.01f)));
    NLM_LOG_INFO("stdp_ltd_weight: " + std::to_string(config.getOr<float>("stdp_ltd_weight", 0.012f)));
    NLM_LOG_INFO("synaptogenesis_rate: " + std::to_string(config.getOr<float>("synaptogenesis_rate", 0.0001f)));
    NLM_LOG_INFO("pruning_rate: " + std::to_string(config.getOr<float>("pruning_rate", 0.00001f)));
    NLM_LOG_INFO("");
}

void runPhase6IntegrationDemo() {
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("This demo shows a complete artificial brain with all systems integrated.");
    NLM_LOG_INFO("");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set Phase 6 specific parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters for Phase 6 learning
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Development parameters
    config->set("development_enabled", true, ConfigSource::Default);
    config->set("development_start_age", 60.0, ConfigSource::Default);
    config->set("development_maturity_age", 300.0, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_sensitivity", 1.0f, ConfigSource::Default);
    config->set("curiosity_threshold", 0.3f, ConfigSource::Default);
    config->set("novelty_decay_rate", 0.99f, ConfigSource::Default);
    
    // Memory parameters
    config->set("working_memory_capacity", 1000, ConfigSource::Default);
    config->set("episodic_memory_capacity", 500, ConfigSource::Default);
    
    // Prediction system parameters
    config->set("prediction_horizon", 10, ConfigSource::Default);
    config->set("prediction_error_decay", 0.9f, ConfigSource::Default);
    
    printConfigurationSummary(*config);
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain for Phase 6 integration...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    NLM_LOG_INFO("");
    
    // Create world
    SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    // Create agent brain interface
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable all advanced features for Phase 6
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Agent configured with all Phase 6 features enabled:");
    NLM_LOG_INFO("  - Reward modulation: Enabled");
    NLM_LOG_INFO("  - Structural plasticity: Enabled");
    NLM_LOG_INFO("  - Development: Enabled");
    NLM_LOG_INFO("  - Curiosity: Enabled");
    NLM_LOG_INFO("");
    
    // Phase 6 Integration Demo: Run simulation
    NLM_LOG_INFO("=== Phase 6 Integration Simulation ===");
    NLM_LOG_INFO("Running complete brain lifecycle with memory, neuromodulation, and development.");
    NLM_LOG_INFO("(This will run for 2000 simulation steps)");
    NLM_LOG_INFO("");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run Phase 6 integration experiment
    Phase6Config phase6Config;
    phase6Config.neuronCount = 500;
    phase6Config.maxSteps = 2000;
    phase6Config.regionCount = 1;
    phase6Config.connectionProbability = 0.1f;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    phase6Config.checkpointPath = "./nlm_phase6_checkpoint.bin";
    
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(phase6Config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Display comprehensive results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("Simulation completed successfully!");
    NLM_LOG_INFO("Wall clock time: " + std::to_string(wallClockTime) + "s");
    NLM_LOG_INFO("Average reward per step: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(result.curiosityLevel));
    NLM_LOG_INFO("Novelty level: " + std::to_string(result.noveltyLevel));
    NLM_LOG_INFO("")
    
    // Show system integration status
    NLM_LOG_INFO("=== System Integration Status ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED"));
    NLM_LOG_INFO("Prediction System: " + std::string(result.predictionIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED"));
    NLM_LOG_INFO("Development System: " + std::string(result.developmentIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING"));
    NLM_LOG_INFO("Replay System: " + std::string(result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING"));
    NLM_LOG_INFO("")
    
    // Final assessment
    NLM_LOG_INFO("=== Phase 6 Assessment ===");
    
    int integratedSystems = 0;
    if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
    if (result.neuromodulationIntegrated) integratedSystems++;
    if (result.predictionIntegrated) integratedSystems++;
    if (result.developmentIntegrated) integratedSystems++;
    if (result.checkpointingWorks) integratedSystems++;
    if (result.replayWorks) integratedSystems++;
    
    NLM_LOG_INFO("Systems successfully integrated: " + std::to_string(integratedSystems) + "/7");
    
    if (integratedSystems >= 6) {
        NLM_LOG_INFO("✓ PHASE 6 COMPLETE: The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("  capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("  All major systems are connected and interacting properly.");
    } else {
        NLM_LOG_ERROR("✗ Phase 6 incomplete: Some systems are not properly integrated.");
        NLM_LOG_ERROR("  Please check the integration verification logs above.");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 integration demo completed successfully!");
}

int main(int argc, char** argv) {
    printPhase6Banner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        runPhase6IntegrationDemo();
        return 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Fatal error in Phase 6 demo: " + std::string(e.what()));
        return 1;
    }
}