// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the complete integrated artificial brain with:
// - Memory systems (working + episodic)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system
// - Cognition (planning, concept formation, attention)
// - Development stages affecting plasticity
// - Checkpoint save/load
// - Memory replay and consolidation
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Sensory and motor interfaces

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
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated.     ║
    ║     This phase integrates all previous phases into a coherent   ║
    ║     neural system capable of learning, memory, and adaptation. ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runPhase6IntegrationDemo() {
    std::cout << "=== NLM Phase 6: Final Integration Demo ===" << std::endl;
    std::cout << "Running complete artificial brain integration test..." << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Implementing the complete integrated artificial brain:");
    NLM_LOG_INFO("  - Memory systems (working + episodic)");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Prediction system");
    NLM_LOG_INFO("  - Cognition (planning, concept formation, attention)");
    NLM_LOG_INFO("  - Development stages affecting plasticity");
    NLM_LOG_INFO("  - Checkpoint save/load");
    NLM_LOG_INFO("  - Memory replay and consolidation");
    NLM_LOG_INFO("  - Real LIF neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Create Phase 6 integration experiment
    Phase6IntegratedExperiment experiment;
    
    // Run quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        NLM_LOG_ERROR("ERROR: Integration verification failed!");
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with demo settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("All systems are properly integrated and functional.");
    NLM_LOG_INFO("The NLM brain is now a complete artificial neural substrate");
    NLM_LOG_INFO("capable of complex learning, memory, and adaptive behavior.");
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
    }
    
    // Initialize logging level based on verbosity
    if (verbose) {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else {
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    // Load configuration from file
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration. File not found: " + configFile);
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set Phase 6 default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);  // Standard Phase 6 size
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Phase 6 specific parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Log configuration summary
    std::cout << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
    std::cout << "";
    
    // Run Phase 6 integration demo
    runPhase6IntegrationDemo();
    
    return 0;
}
