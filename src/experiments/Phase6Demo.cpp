/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Exceptions.hpp"
#include <iostream>
#include <stdexcept>

using namespace nlm;

int main(int argc, char* argv[]) {
    try {
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
        
        // Test individual systems with error handling
        std::cout << "--- Memory Integration Test ---" << std::endl;
        try {
            experiment.testMemoryIntegration();
            std::cout << "Memory integration test passed!" << std::endl;
        } catch (const nlm::MemoryException& e) {
            std::cerr << "Memory integration test failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        try {
            experiment.testNeuromodulationIntegration();
            std::cout << "Neuromodulation integration test passed!" << std::endl;
        } catch (const nlm::IntegrationTestException& e) {
            std::cerr << "Neuromodulation integration test failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        try {
            experiment.testCheckpointing();
            std::cout << "Checkpoint test passed!" << std::endl;
        } catch (const nlm::RecoveryException& e) {
            std::cerr << "Checkpoint test failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        try {
            experiment.testReplay();
            std::cout << "Replay test passed!" << std::endl;
        } catch (const nlm::MemoryException& e) {
            std::cerr << "Replay test failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << std::endl;
        
        // Run full experiment with smaller settings for demo
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        try {
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
            std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << std::endl;
            
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            
            // Calculate integration score from integration status
            int integratedSystems = 0;
            if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
            if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
            if (result.neuromodulationIntegrated) integratedSystems++;
            if (result.predictionIntegrated) integratedSystems++;
            if (result.developmentIntegrated) integratedSystems++;
            if (result.checkpointingWorks) integratedSystems++;
            if (result.replayWorks) integratedSystems++;
            
            float integrationScore = (static_cast<float>(integratedSystems) / 7.0f) * 100.0f;
            std::cout << std::endl;
            std::cout << "=== INTEGRATION SUMMARY ===" << std::endl;
            std::cout << "Systems Integrated: " << integratedSystems << "/7 (" << std::fixed << std::setprecision(1) << integrationScore << "%)" << std::endl;
            
            if (integrationScore >= 85.0f) {
                std::cout << "✓ SUCCESS: Brain systems are well integrated (Phase 6 completed)" << std::endl;
            } else {
                std::cout << "⚠ WARNING: Some systems not fully integrated (Phase 6 partial)" << std::endl;
            }
            
            std::cout << "=== Phase 6 Integration Complete ===" << std::endl;
            
            return 0;
            
        } catch (const nlm::BrainException& e) {
            std::cerr << "Full integration test failed: " << e.what() << std::endl;
            return 1;
        } catch (const nlm::IntegrationTestException& e) {
            std::cerr << "Integration test verification failed: " << e.what() << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error during integration test: " << e.what() << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error in Phase 6 demo: " << e.what() << std::endl;
        return 1;
    }
}
