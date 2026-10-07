// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements a comprehensive artificial brain with:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - All memory systems connected to neural processing
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system integrated into brain loop
// - Cognitive mechanisms (planning, concept formation, attention)
// - Developmental system affecting plasticity rates
// - Checkpoint save/load for persistence
// - Replay and consolidation for memory strengthening
// - Agent brain interface for perception and action

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

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
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Implementing complete artificial brain with all systems integrated");
    NLM_LOG_INFO("");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("reward_discount_factor", 0.99f, ConfigSource::Default);
    
    // Log configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Create Phase 6 integrated experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration test
    Phase6Config phase6Config;
    phase6Config.neuronCount = 500;
    phase6Config.regionCount = 1;
    phase6Config.connectionProbability = 0.15f;
    phase6Config.maxSteps = 1000;
    phase6Config.enableDevelopment = true;
    phase6Config.checkpointPath = "nlm_checkpoint_test.bin";
    phase6Config.enableCheckpointing = false;
    
    NLM_LOG_INFO("Running Phase 6 Integration Test...");
    NLM_LOG_INFO("This test verifies all brain systems are properly connected.");
    NLM_LOG_INFO("");
    
    auto result = experiment.run(phase6Config);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Prediction System: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Development System: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("");
    
    bool allIntegrated = result.memoryWorkingMemoryIntegrated && 
                        result.memoryEpisodicMemoryIntegrated &&
                        result.neuromodulationIntegrated &&
                        result.predictionIntegrated &&
                        result.developmentIntegrated;
    
    if (allIntegrated) {
        NLM_LOG_INFO("✓ ALL SYSTEMS SUCCESSFULLY INTEGRATED");
        NLM_LOG_INFO("The NLM artificial brain is now a coherent, integrated system");
        NLM_LOG_INFO("capable of changing its synaptic connections through experience.");
    } else {
        NLM_LOG_ERROR("✗ SOME SYSTEMS NOT INTEGRATED");
        NLM_LOG_ERROR("The NLM artificial brain requires further integration work.");
        return 1;
    }
    
    return 0;
}

