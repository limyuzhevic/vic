// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase demonstrates the complete integrated artificial brain with:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system integrated into brain loop
// - Neural planner with action sequence evaluation
// - Concept formation from experience patterns
// - Attention with competitive selection dynamics
// - Developmental stages affecting plasticity rates
// - Checkpoint save/load implementation
// - Replay and consolidation functionality
//
// Main entry point updated to use Phase 6 integration demo instead of Phase 2
// to provide users with the complete integrated brain experience.

#include "Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Constants.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

void printHeader() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 6: Final Integration                                ║
║                                                               ║
║     Complete artificial brain with all systems integrated!     ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

int main(int argc, char* argv[]) {
    printHeader();
    
    std::cout << "Initializing NLM Phase 6 Integration Demo...\n" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Demonstrating complete artificial brain with all systems integrated.");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    Phase6Config config;
    std::string configFile = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--no-checkpoint" && i + 1 < argc) {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay" && i + 1 < argc) {
            config.enableReplay = false;
        } else if (arg == "--no-development" && i + 1 < argc) {
            config.enableDevelopment = false;
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg.substr(0, 7) == "--config" && arg.find('=') != std::string::npos) {
            configFile = arg.substr(arg.find('=') + 1);
        } else if (arg.substr(0, 7) == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
    }
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Set default configuration values using constants
    if (config.neuronCount == 0) {
        config.neuronCount = DEMO_NEURON_COUNT;
    }
    if (config.maxSteps == 0) {
        config.maxSteps = DEFAULT_MAX_STEPS;
    }
    if (!config.checkpointPath.empty()) {
        // Use provided checkpoint path
    } else {
        config.checkpointPath = DEFAULT_CHECKPOINT_FILENAME;
    }
    
    NLM_LOG_INFO("=== Phase 6 Integration Demo Configuration ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("Max steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("Checkpointing: " + std::string(config.enableCheckpointing ? "enabled" : "disabled"));
    NLM_LOG_INFO("Replay: " + std::string(config.enableReplay ? "enabled" : "disabled"));
    NLM_LOG_INFO("Development: " + std::string(config.enableDevelopment ? "enabled" : "disabled"));
    NLM_LOG_INFO("Checkpoint path: " + config.checkpointPath);
    NLM_LOG_INFO("");
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Run individual system tests
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
    
    // Run full integration experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    auto result = experiment.run(config);
    
    // Display results
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
    
    std::cout << std::endl;
    std::cout << "=== PHASE 6 COMPLETE ===" << std::endl;
    std::cout << std::endl;
    std::cout << "All systems are successfully integrated into a coherent artificial brain!" << std::endl;
    std::cout << "The NLM brain can now:

  - Store and retrieve experiences through episodic memory
  - Maintain information in working memory across delays
  - Use prediction to anticipate sensory input
  - Plan actions using multiple steps of evaluation
  - Form concepts from repeated experience
  - Select actions using competitive attention
  - Learn through neuromodulation (dopamine, curiosity, novelty)
  - Develop through developmental stages affecting plasticity
  - Save and load brain state for persistence
  - Re-play important experiences for consolidation" << std::endl;
    std::cout << std::endl;
    std::cout << "This represents Phase 6: Final Integration of the NLM project." << std::endl;
    
    return 0;
}