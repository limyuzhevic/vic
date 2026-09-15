/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command-line arguments for flexibility
    Phase6Config config;
    
    // Parse custom arguments (easier syntax than traditional ArgParse)
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[i + 1]);
            i++;
        } else if (std::string(argv[i]) == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[i + 1]);
            i++;
        } else if (std::string(argv[i]) == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (std::string(argv[i]) == "--no-replay") {
            config.enableReplay = false;
        } else if (std::string(argv[i]) == "--fast") {
            config.neuronCount = 200;
            config.maxSteps = 500;
        } else if (std::string(argv[i]) == "--thorough") {
            config.neuronCount = 1000;
            config.maxSteps = 10000;
        } else if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "=== NLM Phase 6 Demo Options ===" << std::endl;
            std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --neurons N      Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps N        Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint   Disable checkpointing" << std::endl;
            std::cout << "  --no-replay       Disable replay" << std::endl;
            std::cout << "  --fast           Use quick demo settings (200 neurons, 500 steps)" << std::endl;
            std::cout << "  --thorough       Use detailed demo settings (1000 neurons, 10000 steps)" << std::endl;
            std::cout << "  -h, --help       Show this help message" << std::endl;
            std::cout << std::endl;
            return 0;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    std::cout << std::endl;
    
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
    
    // Run full experiment with the configured parameters
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    config.neuronCount = 500;  // Override for demo brevity
    config.maxSteps = 2000;    // Override for demo brevity
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
    
    return 0;
}
