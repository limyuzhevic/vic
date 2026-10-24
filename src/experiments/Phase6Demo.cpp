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
    Logger::getGlobal().setLevel(LogLevel::Info);
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Testing the integrated artificial brain...");
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    NLM_LOG_INFO("--- Integration Verification ---");
    bool integrationOK = experiment.verifyIntegration();
    
    NLM_LOG_INFO("");
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        NLM_LOG_ERROR("Integration verification failed!");
        return 1;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    std::cout << std::endl;
    
    // Test individual systems
    NLM_LOG_INFO("--- Memory Integration Test ---");
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    NLM_LOG_INFO("--- Checkpoint Test ---");
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    NLM_LOG_INFO("--- Replay Test ---");
    experiment.testReplay();
    std::cout << std::endl;
    
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
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("=== INTEGRATION STATUS ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
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
