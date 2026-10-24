/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <map>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --config <file>           Load configuration from file" << std::endl;
    std::cout << "  --neurons <N>             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps <N>               Number of simulation steps (default: 10000)" << std::endl;
    std::cout << "  --region_count <N>        Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection_prob <P>     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable_checkpointing     Enable checkpoint save/load (default: true)" << std::endl;
    std::cout << "  --enable_replay            Enable memory replay (default: true)" << std::endl;
    std::cout << "  --enable_development      Enable developmental system (default: true)" << std::endl;
    std::cout << "  --checkpoint_dir <dir>     Directory for checkpoints (default: ./checkpoints)" << std::endl;
    std::cout << "  --quick                   Run only integration verification" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
}

Phase6Config parseConfig(int argc, char* argv[]) {
    Phase6Config config;
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quick") {
            // Keep default config for quick mode
        } else if (arg == "--enable_checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--disable_checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--enable_replay") {
            config.enableReplay = true;
        } else if (arg == "--disable_replay") {
            config.enableReplay = false;
        } else if (arg == "--enable_development") {
            config.enableDevelopment = true;
        } else if (arg == "--disable_development") {
            config.enableDevelopment = false;
        } else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 9) == "--neurons=") {
            config.neuronCount = std::stoull(arg.substr(9));
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg.substr(0, 7) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg.substr(0, 13) == "--region_count=") {
            config.regionCount = std::stoul(arg.substr(14));
        } else if (arg == "--region_count" && i + 1 < argc) {
            config.regionCount = std::stoul(argv[++i]);
        } else if (arg.substr(0, 18) == "--connection_prob=") {
            config.connectionProbability = std::stof(arg.substr(19));
        } else if (arg == "--connection_prob" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        } else if (arg.substr(0, 16) == "--checkpoint_dir=") {
            config.checkpointPath = arg.substr(17);
        } else if (arg == "--checkpoint_dir" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    printUsage(argv[0]);
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse configuration
    Phase6Config config = parseConfig(argc, argv);
    
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
    
    // Run full experiment with parsed configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Config: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    
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
    
    return 0;
}
