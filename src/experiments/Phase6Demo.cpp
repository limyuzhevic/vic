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
    // Parse command-line arguments for flexible demo configuration
    int demoMode = 0;  // 0=full demo, 1=quick verification, 2=integration only
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    
    // Simple command-line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quick") {
            demoMode = 1;
        } else if (arg == "--integration-only") {
            demoMode = 2;
        } else if (arg == "--neurons") {
            if (i + 1 < argc) neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps") {
            if (i + 1 < argc) maxSteps = std::stoul(argv[++i]);
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    if (demoMode == 1) std::cout << "[Mode: Quick Verification]" << std::endl;
    else if (demoMode == 2) std::cout << "[Mode: Integration Only]" << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment with error checking
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "This indicates missing or disconnected brain systems." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    if (demoMode != 2) {  // Skip additional tests in integration-only mode
        // Test individual systems with enhanced error handling and logging
        std::cout << "--- Memory Integration Test ---" << std::endl;
        bool memoryTestOK = false;
        try {
            memoryTestOK = experiment.testMemoryIntegration();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Memory integration test failed with exception: " << e.what() << std::endl;
        }
        
        if (memoryTestOK) {
            std::cout << "Memory integration test completed successfully." << std::endl << std::endl;
        } else {
            std::cerr << "WARNING: Memory integration test reported issues." << std::endl << std::endl;
        }

        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        bool neuromodTestOK = false;
        try {
            neuromodTestOK = experiment.testNeuromodulationIntegration();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Neuromodulation integration test failed with exception: " << e.what() << std::endl;
        }
        
        if (neuromodTestOK) {
            std::cout << "Neuromodulation integration test completed successfully." << std::endl << std::endl;
        } else {
            std::cerr << "WARNING: Neuromodulation integration test reported issues." << std::endl << std::endl;
        }

        std::cout << "--- Checkpoint Test ---" << std::endl;
        bool checkpointTestOK = false;
        try {
            checkpointTestOK = experiment.testCheckpointing();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Checkpoint test failed with exception: " << e.what() << std::endl;
        }
        
        if (checkpointTestOK) {
            std::cout << "Checkpoint test completed successfully." << std::endl << std::endl;
        } else {
            std::cerr << "WARNING: Checkpoint test reported issues." << std::endl << std::endl;
        }

        std::cout << "--- Replay Test ---" << std::endl;
        bool replayTestOK = false;
        try {
            replayTestOK = experiment.testReplay();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Replay test failed with exception: " << e.what() << std::endl;
        }
        
        if (replayTestOK) {
            std::cout << "Replay test completed successfully." << std::endl << std::endl;
        } else {
            std::cerr << "WARNING: Replay test reported issues." << std::endl << std::endl;
        }
    } else {
        std::cout << "Skipping individual system tests (integration-only mode)." << std::endl << std::endl;
    }
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_phase6_demo.bin";
    
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
    
    // Exit with error code if critical systems are disconnected
    bool criticalSuccess = result.memoryWorkingMemoryIntegrated && 
                          result.memoryEpisodicMemoryIntegrated && 
                          result.neuromodulationIntegrated;
    
    if (!criticalSuccess) {
        std::cerr << "WARNING: Some critical brain systems are not properly integrated!" << std::endl;
        return 1;
    }
    
    return 0;
}
