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
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    // Command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Integration Demo Usage:" << std::endl;
            std::cout << "  ./nlm_phase6_demo [options]" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -h, --help              Show this help message" << std::endl;
            std::cout << "  --neurons N             Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps S               Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpointing      Disable checkpointing" << std::endl;
            std::cout << "  --no-replay             Disable replay system" << std::endl;
            std::cout << "  --no-development        Disable development" << std::endl;
            std::cout << "  --checkpoint-path P     Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
            std::cout << "  --checkpoint-dir D      Directory for checkpoints (default: ./checkpoints)" << std::endl;
            std::cout << "  --seed S                Random seed for reproducibility" << std::endl;
            std::cout << "" << std::endl;
            return 0;
        }
        
        if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
        
        if (arg.substr(0, 12) == "--neurons=") {
            config.neuronCount = std::stoi(arg.substr(12));
        } else if (arg.substr(0, 8) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg.substr(0, 19) == "--checkpoint-path=") {
            config.checkpointPath = arg.substr(19);
        } else if (arg.substr(0, 17) == "--checkpoint-dir=") {
            // Extract directory from path
            std::string dir = arg.substr(17);
            if (dir.back() != '/') {
                dir += '/';
            }
            config.checkpointPath = dir + "checkpoint_test.bin";
        } else if (arg.substr(0, 9) == "--seed=") {
            uint64_t seed = std::stoull(arg.substr(9));
            config.randomSeed = seed;
        }
    }
    
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
    
    // Run full experiment with user-configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "YES" : "NO") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "YES" : "NO") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "YES" : "NO") << std::endl;
    std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
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
