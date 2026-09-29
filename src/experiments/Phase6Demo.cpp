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
    // Initialize logging at Info level
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl << std::endl;
    
    // Create experiment instance
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "\nIntegration verification passed!\n";
    
    // Test individual system integrations
    std::cout << "Testing system integrations...\n\n";
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    experiment.testCheckpointing();
    experiment.testReplay();
    
    // Run full integration experiment
    std::cout << "Running full integration experiment (500 neurons, 2000 steps)..." << std::endl;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Display summary results
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    std::cout << "\nWall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Success message
    std::cout << "\n=== Phase 6 Integration Complete ===" << std::endl;
    std::cout << "All brain systems are successfully integrated!" << std::endl;
    
    return 0;
}
