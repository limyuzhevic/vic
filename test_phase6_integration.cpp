#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main() {
    std::cout << "=== NLM Phase 6 Integration Test - Modular Implementation ===" << std::endl;
    std::cout << "Testing the integrated brain with new modular step implementation..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl << std::endl;
    
    // Test memory integration
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    // Test neuromodulation integration
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    // Test checkpointing
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << std::endl;
    
    // Test replay
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 1000;
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
    
    // Overall success check
    bool overallSuccess = integrationOK && memoryOK && neuromodulationOK && checkpointOK && replayOK && 
                         result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated &&
                         result.neuromodulationIntegrated && result.predictionIntegrated && 
                         result.developmentIntegrated && result.checkpointingWorks;
    
    std::cout << std::endl;
    std::cout << "=== OVERALL ASSESSMENT ===" << std::endl;
    if (overallSuccess) {
        std::cout << "✅ SUCCESS: All systems properly integrated with modular implementation!" << std::endl;
        std::cout << "✅ The Phase 6 integration is complete with improved code structure." << std::endl;
    } else {
        std::cout << "❌ ISSUES: Some systems may need further integration." << std::endl;
    }
    
    return overallSuccess ? 0 : 1;
}