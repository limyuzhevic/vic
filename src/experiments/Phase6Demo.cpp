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

// Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    NLM_LOG_INFO("=== Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Testing the integrated artificial brain...");
    
    NLM_LOG_INFO("");
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    NLM_LOG_INFO("--- Integration Verification ---");
    bool integrationOK = experiment.verifyIntegration();
    
    NLM_LOG_INFO("");
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    NLM_LOG_INFO("");
    
    // Test individual systems
    NLM_LOG_INFO("--- Memory Integration Test ---");
    experiment.testMemoryIntegration();
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
    experiment.testNeuromodulationIntegration();
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("--- Checkpoint Test ---");
    experiment.testCheckpointing();
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("--- Replay Test ---");
    experiment.testReplay();
    NLM_LOG_INFO("");
    
    // Run full experiment with smaller settings for demo
    NLM_LOG_INFO("--- Full Integration Experiment ---");
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL RESULTS ===");
    NLM_LOG_INFO("Total reward: " << std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " << std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " << std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " << std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== INTEGRATION STATUS ===");
    NLM_LOG_INFO("Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO("Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO("Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO("Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO("Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO("Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING")));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("Wall clock time: " << std::to_string(result.totalWallClockTime) << "s");
    
    return 0;
