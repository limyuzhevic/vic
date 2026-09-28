/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * IMPROVED: Now includes advanced configuration options for users
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace nlm;

// Helper function to parse command line arguments
Phase6Config parsePhase6Config(int argc, char* argv[]) {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    config.connectionProbability = 0.15f;
    config.regionCount = 1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --neuron-count N      Number of neurons (default: 500)" << std::endl;
            std::cout << "  --max-steps S         Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint       Disable checkpointing" << std::endl;
            std::cout << "  --no-replay           Disable replay" << std::endl;
            std::cout << "  --no-development      Disable development" << std::endl;
            std::cout << "  --checkpoint-path P   Checkpoint file path (default: /tmp/nlm_checkpoint_demo.bin)" << std::endl;
            std::cout << "  --connection-prob P   Connection probability (default: 0.15)" << std::endl;
            std::cout << "  --regions R           Number of regions (default: 1)" << std::endl;
            std::cout << "  --verbose             Enable verbose logging" << std::endl;
            std::cout << "  --help, -h            Show this help message" << std::endl;
            std::exit(0);
        }
        
        if (arg.find("--neuron-count=") == 0) {
            try {
                config.neuronCount = std::stoi(arg.substr(16));
            } catch (...) {
                std::cerr << "Invalid neuron count: " << arg.substr(16) << std::endl;
            }
        } else if (arg.find("--max-steps=") == 0) {
            try {
                config.maxSteps = std::stoi(arg.substr(11));
            } catch (...) {
                std::cerr << "Invalid max steps: " << arg.substr(11) << std::endl;
            }
        } else if (arg.find("--checkpoint-path=") == 0) {
            config.checkpointPath = arg.substr(19);
        } else if (arg.find("--connection-prob=") == 0) {
            try {
                config.connectionProbability = std::stof(arg.substr(20));
            } catch (...) {
                std::cerr << "Invalid connection probability: " << arg.substr(20) << std::endl;
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                config.regionCount = std::stoi(arg.substr(10));
            } catch (...) {
                std::cerr << "Invalid regions: " << arg.substr(10) << std::endl;
            }
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--verbose") {
            Logger::getInstance().setLevel(Logger::Level::Debug);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config = parsePhase6Config(argc, argv);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
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
    
    // Run full experiment with user-specified settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neuron Count: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Enable Checkpointing: " << (config.enableCheckpointing ? "YES" : "NO") << std::endl;
    std::cout << "  Enable Replay: " << (config.enableReplay ? "YES" : "NO") << std::endl;
    std::cout << "  Enable Development: " << (config.enableDevelopment ? "YES" : "NO") << std::endl;
    std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
    std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "  Region Count: " << config.regionCount << std::endl;
    std::cout << std::endl;
    
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
