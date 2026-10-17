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
    try {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging with error handling
        try {
            Logger::getInstance().setLevel(Logger::Level::Info);
            NLM_LOG_INFO("Logging system initialized for Phase 6 demo");
        } catch (const std::exception& e) {
            std::cerr << "WARNING: Failed to initialize logging: " << e.what() << std::endl;
            std::cerr << "Continuing with basic console output..." << std::endl;
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
        
        // Test individual systems with error handling
        std::cout << "--- Memory Integration Test ---" << std::endl;
        try {
            bool memoryOK = experiment.testMemoryIntegration();
            if (!memoryOK) {
                std::cerr << "WARNING: Memory integration test completed with warnings" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Memory integration test failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        try {
            bool neuromodOK = experiment.testNeuromodulationIntegration();
            if (!neuromOK) {
                std::cerr << "WARNING: Neuromodulation integration test completed with warnings" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Neuromodulation integration test failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        try {
            bool checkpointOK = experiment.testCheckpointing();
            if (!checkpointOK) {
                std::cerr << "WARNING: Checkpoint test completed with warnings" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Checkpoint test failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        try {
            bool replayOK = experiment.testReplay();
            if (!replayOK) {
                std::cerr << "WARNING: Replay test completed with warnings" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Replay test failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        
        // Run full experiment with smaller settings for demo
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
        
        // Report any integration issues
        bool hasIssues = !result.memoryWorkingMemoryIntegrated || 
                       !result.memoryEpisodicMemoryIntegrated ||
                       !result.neuromodulationIntegrated ||
                       !result.predictionIntegrated ||
                       !result.developmentIntegrated ||
                       !result.checkpointingWorks;
        
        if (hasIssues) {
            std::cerr << "\nWARNING: Some integration tests failed or showed issues!" << std::endl;
            if (result.totalWallClockTime > 60.0) {
                std::cerr << "WARNING: Integration test took a long time - consider adjusting parameters" << std::endl;
            }
        } else {
            std::cout << "\nSUCCESS: All integration tests passed!" << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unhandled exception in Phase6Demo: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred in Phase6Demo" << std::endl;
        return 1;
    }
}
