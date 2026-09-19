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
    bool skipQuickTests = false;
    bool runFullDemo = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--skip-quick-tests" || arg == "-q") {
            skipQuickTests = true;
        } else if (arg == "--no-full-demo" || arg == "-n") {
            runFullDemo = false;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -q, --skip-quick-tests  Skip quick integration tests" << std::endl;
            std::cout << "  -n, --no-full-demo      Don't run full demo" << std::endl;
            std::cout << "  --neurons N             Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps N               Number of steps (default: 2000)" << std::endl;
            std::cout << "  --checkpoint-path P      Path for checkpoint file (default: ./checkpoint_test.bin)" << std::endl;
            std::cout << "  -h, --help              Show this help" << std::endl;
            return 0;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification (unless skipped)
    if (!skipQuickTests) {
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    if (!skipQuickTests) {
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
    }
    
    // Run full experiment with demo settings (unless disabled)
    if (runFullDemo) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config demoConfig;
        demoConfig.neuronCount = config.neuronCount;
        demoConfig.maxSteps = config.maxSteps;
        demoConfig.enableCheckpointing = true;
        demoConfig.enableReplay = true;
        demoConfig.enableDevelopment = true;
        
        auto result = experiment.run(demoConfig);
        
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
    }
    
    return 0;
}
