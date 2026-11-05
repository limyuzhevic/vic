/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

using namespace nlm;

// Create a standard configuration template for Phase 6 experiments
Phase6Config createPhase6Config() {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
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
    
    // Parse command line arguments for configuration
    std::cout << "--- Configuration ---" << std::endl;
    auto config = std::make_shared<Config>();
    
    // Load from default config file
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config=file.cfg      Load configuration from file" << std::endl;
            std::cout << "  --neurons=N            Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps=N              Max simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint        Disable checkpointing" << std::endl;
            std::cout << "  --no-replay            Disable replay" << std::endl;
            std::cout << "  --no-development       Disable development" << std::endl;
            std::cout << "  --checkpoint=path      Set checkpoint file path" << std::endl;
            std::cout << "  --help, -h             Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Load configuration from file
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Extract configuration values
    uint64_t neuronCount = config->getOr<uint64_t>("neuron_count", 500);
    uint64_t maxSteps = config->getOr<uint64_t>("max_simulation_steps", 2000);
    bool enableCheckpointing = config->getOr<bool>("enable_checkpointing", true);
    bool enableReplay = config->getOr<bool>("enable_replay", true);
    bool enableDevelopment = config->getOr<bool>("enable_development", true);
    std::string checkpointPath = config->getOr<std::string>("checkpoint_path", "./checkpoint_test.bin");
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Max steps: " << maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << "  Checkpoint path: " << checkpointPath << std::endl;
    std::cout << std::endl;
    
    // Run full experiment with user-configurable parameters
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config phase6Config;
    phase6Config.neuronCount = neuronCount;
    phase6Config.maxSteps = maxSteps;
    phase6Config.enableCheckpointing = enableCheckpointing;
    phase6Config.enableReplay = enableReplay;
    phase6Config.enableDevelopment = enableDevelopment;
    phase6Config.checkpointPath = checkpointPath;
    
    auto result = experiment.run(phase6Config);
    
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
    
    return 0;
}
